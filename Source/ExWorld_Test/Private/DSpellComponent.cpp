// Fill out your copyright notice in the Description page of Project Settings.


#include "DSpellComponent.h"

#include "DCharacter.h"
#include "DPlayerState.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UDSpellComponent::UDSpellComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	/*Component set to tick once per a second.*/
	SetComponentTickInterval(1.0f);
}


// Called when the game starts
void UDSpellComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ADCharacter>(GetOwner());

	/*The spell starts on cooldown*/
	CurrentSpellCooldownTime = SpellCooldownTime;
}

void UDSpellComponent::SpawnProjectile() const
{
	/*If a socket name is not provided, the projectile will be spawned at the location of the owning character*/
	FVector HandLocation;
	if (PrimaryAttackSocket != "")
	{
		HandLocation = OwningCharacter->GetMesh()->GetSocketLocation(PrimaryAttackSocket);
	}
	else
	{
		HandLocation = OwningCharacter->GetActorLocation();
	}

	/*Spawn Transform Matrix*/
	FTransform SpawnTM = FTransform(OwningCharacter->GetControlRotation() ,HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = OwningCharacter;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void UDSpellComponent::ClientPlayAttackAnimation_Implementation()
{
	OwningCharacter->PlayAnimMontage(CastAnimation);
}


void UDSpellComponent::ClientResetCooldownTimer_Implementation()
{
	CurrentSpellCooldownTime = SpellCooldownTime;
}

// Called every frame
void UDSpellComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bCanCast)
	{
		if (OwningCharacter->HasAuthority())
		{
			CurrentSpellCooldownTime--;
			if (CurrentSpellCooldownTime <= 0)
			{
				bCanCast = true;
			}
		}

		/*Client cooldown is predicted for UI purposes. The server's cooldown determines whether or not the spell can be cast.*/
		if (!IsRunningDedicatedServer() && CurrentSpellCooldownTime > 0)
		{
			/*The cooldown is predicted on the client*/
			if (!OwningCharacter->HasAuthority())
			{
				CurrentSpellCooldownTime--;
			}
		}
	}
}

bool UDSpellComponent::IsSpellAffordable(float CurrentMana) const
{
	if (CurrentMana > ManaCost)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UDSpellComponent::CastSpell()
{
	if (OwningCharacter)
	{
		if (bCanCast == true)
		{
			ADPlayerState* OwningPlayerState = Cast<ADPlayerState>(OwningCharacter->GetPlayerState());

			if (OwningPlayerState)
			{
				if (IsSpellAffordable(OwningPlayerState->GetMana()))
				{
					if (CastAnimation)
					{
						OwningCharacter->GetWorldTimerManager().SetTimer(WindUpTimerHandle, this, &UDSpellComponent::SpawnProjectile, CastAnimation->CalculateSequenceLength()/2);
						ClientPlayAttackAnimation();
					}
					else
					{
						SpawnProjectile();
					}

					CurrentSpellCooldownTime = SpellCooldownTime;
					ClientResetCooldownTimer();
					
					bCanCast = false;
					OwningPlayerState->RemoveMana(ManaCost);
				}
			}
		}	
	}
}

bool UDSpellComponent::CanCastSpell()
{
	return bCanCast;
}

int UDSpellComponent::GetCurrentSpellCooldownTime()
{
	return CurrentSpellCooldownTime;
}

