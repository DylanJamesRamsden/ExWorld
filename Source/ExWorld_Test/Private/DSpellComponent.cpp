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

	SetComponentTickInterval(1.0f);
}


// Called when the game starts
void UDSpellComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ADCharacter>(GetOwner());

	CurrentSpellCooldownTime = SpellCooldownTime;
}

void UDSpellComponent::SpawnProjectile() const
{
	FVector HandLocation = OwningCharacter->GetMesh()->GetSocketLocation(PrimaryAttackSocket);

	//Spawn Transform Matrix
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

	if (!bIsAvailable)
	{
		if (OwningCharacter->HasAuthority())
		{
			CurrentSpellCooldownTime--;
			if (CurrentSpellCooldownTime <= 0)
			{
				bIsAvailable = true;
			}
		}

		if (!IsRunningDedicatedServer() && CurrentSpellCooldownTime > 0)
		{
			//The cooldown is predicted on the client
			if (!OwningCharacter->HasAuthority())
			{
				CurrentSpellCooldownTime--;
			}
		}
		
	}
	// ...
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
		if (bIsAvailable == true)
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
					
					bIsAvailable = false;
					OwningPlayerState->RemoveMana(ManaCost);
				}
			}
		}	
	}
}

bool UDSpellComponent::IsSpellAvailable()
{
	return bIsAvailable;
}

int UDSpellComponent::GetCurrentSpellCooldownTime()
{
	return CurrentSpellCooldownTime;
}

