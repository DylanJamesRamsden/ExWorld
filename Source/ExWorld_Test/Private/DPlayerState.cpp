// Fill out your copyright notice in the Description page of Project Settings.


#include "DPlayerState.h"

#include "Net/UnrealNetwork.h"

void ADPlayerState::SetHealth(float NewHealth)
{
	Health = NewHealth;
}

float ADPlayerState::GetHealth() const
{
	return Health;
}

float ADPlayerState::GetHealthAsDecimal() const
{
	return Health * 0.01f;
}

void ADPlayerState::AddHealth(float HealthToAdd)
{
	Health = FMath::Clamp(Health + HealthToAdd, 0.0f, 100.0f);
}

void ADPlayerState::SetMana(float NewMana)
{
	Mana = NewMana;
}

float ADPlayerState::GetMana() const
{
	return Mana;
}

float ADPlayerState::GetManaAsDecimal() const
{
	return Mana * 0.01f;
}

void ADPlayerState::AddMana(float ManaToAdd)
{
	Mana = FMath::Clamp(Mana + ManaToAdd, 0.0f, 100.0f);
}

void ADPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPlayerState, Health);
	DOREPLIFETIME(ADPlayerState, Mana);
}
