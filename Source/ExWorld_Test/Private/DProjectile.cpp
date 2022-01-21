// Fill out your copyright notice in the Description page of Project Settings.


#include "DProjectile.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADProjectile::ADProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	/*A sphere component that will act as the projectiles main collision detection source is created*/
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	StaticMeshComp->SetupAttachment(RootComponent);

	/*Default values are set for the projectile's movement component*/
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void ADProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	//The selected effect that this projectile will apply is obtained from the data table
	Effect = EffectHandle.GetRow<FDEffect>("");

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ADProjectile::OnProjectileBeginOverlap);
}

// Called every frame
void ADProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ApplyEffect(SweepResult);
}

void ADProjectile::ApplyEffect_Implementation(FHitResult HitResult)
{
	/*If an effect is being applied to another actor, it must always be verified and executed by the server to ensure there is no client-side exploiting or cheating.*/
	/*If the client calls for an effect to be applied, it is forced to run on the server.*/
	if (HasAuthority())
	{
		/*Ensures that the instigator,the Pawn that spawned this projectile, will not be effected by it's collision logic.*/
		if (GetInstigator() != HitResult.GetActor())
		{
			if (Effect)
			{
				if (Effect->NumberOfObjectsToEffect <= 1)
				{
					if (IsActorEffectTargetObjectType(HitResult.GetComponent()))
					{
						ApplyEffectToSingleActor(HitResult.GetActor());
					}
						
					if (Effect->bDestroyOnCollision)
					{
						ClientDestroyHitActor(this);	
					}
				}
				else
				{
					ApplyEffectAsAOE(HitResult.ImpactPoint);

					if (Effect->bDestroyOnCollision)
					{
						ClientDestroyHitActor(this);
					}
				}
			}
		}	
	}
	else
	{
		ServerApplyEffect(HitResult);
	}
}

void ADProjectile::SetProjectileEffect(FDEffect NewEffect)
{
	Effect = &NewEffect;
}

void ADProjectile::ServerApplyEffect_Implementation(FHitResult HitResult)
{
	ApplyEffect(HitResult);
}

void ADProjectile::ApplyEffectToSingleActor(AActor* HitActor)
{
	switch (Effect->EffectType)
	{
		case EDEffectType::Destroy:
			ClientDestroyHitActor(HitActor);
			break;
		case EDEffectType::ApplyDamage:
			UGameplayStatics::ApplyDamage(HitActor, Effect->DamageToApply, GetInstigator()->GetController(), this, UDamageType::StaticClass());
			OnApplyEffect.Broadcast(HitActor, *Effect);
			break;
	}
}

void ADProjectile::ApplyEffectAsAOE(FVector OverlapOrigin)
{
	FCollisionShape Shape;
	if (Effect->AreaTraceShape == EDAreaTraceShape::Box)
	{
		Shape = FCollisionShape::MakeBox(Effect->BoxSize);
	}
	else
	{
		Shape = FCollisionShape::MakeSphere(Effect->SphereRadius);
	}

	TArray<FOverlapResult> SweepResults;

	TMap<TEnumAsByte<ECollisionChannel>, int> CurrentAoETargetCaps = Effect->AoETargetCaps;

	//Conducts a overlap trace for multiple objects in the shape of the effect's area trace shape
	GetWorld()->OverlapMultiByChannel(SweepResults, OverlapOrigin,
		FQuat::Identity, ECC_WorldStatic, Shape);

	for (auto SweepResult : SweepResults)
	{
		//Ensures that the projectile is not included when the effect is applied
		if (SweepResult.GetActor() != this && IsActorEffectTargetObjectType(SweepResult.GetComponent()))
		{
			//A check to see if the object hit falls in to any of the effect's target object types
			if (CurrentAoETargetCaps.Contains(SweepResult.GetComponent()->GetCollisionObjectType()))
			{
				int CurrentObjectsLeftToDestroy = *CurrentAoETargetCaps.Find(SweepResult.GetComponent()->GetCollisionObjectType());
				if (CurrentObjectsLeftToDestroy > 0)
				{
					//Decreases the number of objects left to destroy that fall in the given object type
					CurrentAoETargetCaps.Add(SweepResult.GetComponent()->GetCollisionObjectType(), CurrentObjectsLeftToDestroy - 1);

					if (Effect->EffectType == EDEffectType::Destroy)
					{
						ClientDestroyHitActor(SweepResult.GetActor());
					}
					else
					{
						UGameplayStatics::ApplyDamage(SweepResult.GetActor(), Effect->DamageToApply, GetInstigator()->GetController(), this, UDamageType::StaticClass());
						OnApplyEffect.Broadcast(SweepResult.GetActor(), *Effect);
					}
				}
			}
		}
	}

	if (Effect->bDebugAreaCollision)
	{
		DebugAOETrace(OverlapOrigin);
	}
}

void ADProjectile::ServerDestroyHitActor_Implementation(AActor* Actor)
{
	ClientDestroyHitActor(Actor);
}

void ADProjectile::ClientDestroyHitActor_Implementation(AActor* Actor)
{
	Actor->Destroy();
}

bool ADProjectile::IsActorEffectTargetObjectType(UPrimitiveComponent* OverlappedComp) const
{
	bool bIsEffectable = false;
	
	if (Effect)
	{
		if(OverlappedComp)
		{
			for (auto ObjectType : Effect->ObjectTypesToEffect)
			{
				/*A check to see if the provided object falls into any of the effect's target object types.*/
				if (OverlappedComp->GetCollisionObjectType() == ObjectType)
				{
					bIsEffectable = true;
					break;
				}
			}	
		}
	}

	return bIsEffectable;
}

void ADProjectile::DebugAOETrace(FVector OverlapOrigin) const
{
	if (Effect->AreaTraceShape == EDAreaTraceShape::Box)
	{
		DrawDebugBox(GetWorld(), OverlapOrigin, Effect->BoxSize, FColor::Red, false, 2, 0, 3);	
	}
	else
	{
		DrawDebugSphere(GetWorld(), OverlapOrigin, Effect->SphereRadius, 32, FColor::Red, false, 2, 0, 3);	
	}
}

