// Fill out your copyright notice in the Description page of Project Settings.


#include "DPlayerState.h"

#include "Net/UnrealNetwork.h"

void ADPlayerState::SetHealth(float NewAmount)
{
	Health = NewAmount;
}

float ADPlayerState::GetHealth() const
{
	return Health;
}

float ADPlayerState::GetHealthAsDecimal() const
{
	return Health * 0.01f;
}

void ADPlayerState::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, 100.0f);
}

void ADPlayerState::RemoveHealth(float Amount)
{
	Health = FMath::Clamp(Health - Amount, 0.0f, 100.0f);
}

void ADPlayerState::SetMana(float NewAmount)
{
	Mana = NewAmount;
}

float ADPlayerState::GetMana() const
{
	return Mana;
}

float ADPlayerState::GetManaAsDecimal() const
{
	return Mana * 0.01f;
}

void ADPlayerState::AddMana(float Amount)
{
	Mana = FMath::Clamp(Mana + Amount, 0.0f, 100.0f);
}

void ADPlayerState::RemoveMana(float Amount)
{
	Mana = FMath::Clamp(Mana - Amount, 0.0f, 100.0f);
}

void ADPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPlayerState, Health);
	DOREPLIFETIME(ADPlayerState, Mana);
}
