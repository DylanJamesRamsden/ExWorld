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
	SetReplicateMovement(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	StaticMeshComp->SetupAttachment(RootComponent);

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

	//Get effect struct from DataTable
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

void ADProjectile::ApplyEffect(FHitResult HitResult)
{
	if (HasAuthority())
	{
		if (GetInstigator() != HitResult.GetActor())
		{
			if (Effect)
			{
				if (Effect->NumberOfObjectsToEffect <= 1)
				{
					if (IsAffectableActor(HitResult.GetComponent()))
					{
						ApplyEffectToSingleHitActor(HitResult.GetActor(), Effect->EffectType);
					}
						
					if (Effect->bDestroyOnCollision)
					{
						ClientDestroyHitActor(this);	
					}
				}
				else
				{
					//An an AOE check is done on the server to verify the collision
					AOEOverlap(HitResult.ImpactPoint, Effect->AreaTraceShape, Effect->EffectType);
				}
			}
		}	
	}
	else
	{
		ServerApplyEffect(HitResult);
	}
}

void ADProjectile::ServerApplyEffect_Implementation(FHitResult HitResult)
{
	ApplyEffect(HitResult);
}

void ADProjectile::ApplyEffectToSingleHitActor(AActor* HitActor, EDEffectType EffectType)
{
	switch (EffectType)
	{
		case EDEffectType::Destroy:
			ClientDestroyHitActor(HitActor);
			break;
		case EDEffectType::ApplyDamage:
			UGameplayStatics::ApplyDamage(HitActor, Effect->DamageToApply, GetInstigator()->GetController(), this, UDamageType::StaticClass());
			break;
	}
}

void ADProjectile::AOEOverlap(FVector OverlapOrigin, EDAreaTraceShape OverlapShape, EDEffectType EffectType)
{
	FCollisionShape Shape;
	if (OverlapShape == EDAreaTraceShape::Box)
	{
		Shape = FCollisionShape::MakeBox(Effect->BoxSize);
	}
	else
	{
		Shape = FCollisionShape::MakeSphere(Effect->SphereRadius);
	}

	TArray<FOverlapResult> SweepResults;

	TMap<TEnumAsByte<ECollisionChannel>, int> CurrentAoETargetCaps = Effect->AoETargetCaps;
	
	GetWorld()->OverlapMultiByChannel(SweepResults, OverlapOrigin,
		FQuat::Identity, ECC_WorldStatic, Shape);

	for (auto SweepResult : SweepResults)
	{
		if (SweepResult.GetActor() != this && IsAffectableActor(SweepResult.GetComponent()))
		{
			if (CurrentAoETargetCaps.Contains(SweepResult.GetComponent()->GetCollisionObjectType()))
			{
				int CurrentObjectsLeftToDestroy = *CurrentAoETargetCaps.Find(SweepResult.GetComponent()->GetCollisionObjectType());
				if (CurrentObjectsLeftToDestroy > 0)
				{
					CurrentAoETargetCaps.Add(SweepResult.GetComponent()->GetCollisionObjectType(), CurrentObjectsLeftToDestroy - 1);

					if (EffectType == EDEffectType::Destroy)
					{
						ClientDestroyHitActor(SweepResult.GetActor());	
					}
					else
					{
						UGameplayStatics::ApplyDamage(SweepResult.GetActor(), Effect->DamageToApply, GetInstigator()->GetController(), this, UDamageType::StaticClass());
					}
				}
			}
		}
	}

	if (Effect->bDestroyOnCollision)
	{
		ClientDestroyHitActor(this);
	}

	if (Effect->bDebugAreaCollision)
	{
		if (OverlapShape == EDAreaTraceShape::Box)
		{
			DrawDebugBox(GetWorld(), OverlapOrigin, Effect->BoxSize, FColor::Red, false, 2, 0, 3);	
		}
		else
		{
			DrawDebugSphere(GetWorld(), OverlapOrigin, Effect->SphereRadius, 32, FColor::Red, false, 2, 0, 3);	
		}
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

bool ADProjectile::IsAffectableActor(UPrimitiveComponent* OverlappedComp) const
{
	bool bIsEffectable = false;
	
	if(OverlappedComp)
	{
		for (auto ObjectType : Effect->ObjectTypesToEffect)
		{
			if (OverlappedComp->GetCollisionObjectType() == ObjectType)
			{
				bIsEffectable = true;
				break;
			}
		}	
	}

	return bIsEffectable;
}

