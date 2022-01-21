// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class EXWORLD_TEST_API ADPlayerState : public APlayerState
{
	GENERATED_BODY()

	protected:
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	float Health;

	UPROPERTY(EditDefaultsOnly, Replicated)
	float Mana;

	public:

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewAmount);

	UFUNCTION(BlueprintPure)
	float GetHealth() const;

	UFUNCTION(BlueprintPure)
	float GetHealthAsDecimal() const;

	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void RemoveHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void SetMana(float NewAmount);

	UFUNCTION(BlueprintPure)
	float GetMana() const;

	UFUNCTION(BlueprintPure)
	float GetManaAsDecimal() const;

	UFUNCTION(BlueprintCallable)
	void AddMana(float Amount);

	UFUNCTION(BlueprintCallable)
	void RemoveMana(float Amount);
};
