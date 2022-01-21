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
	/*The current health of the player.*/
	UPROPERTY(EditDefaultsOnly, Replicated)
	float Health;
	
	/*The current mana of the player.*/
	UPROPERTY(EditDefaultsOnly, Replicated)
	float Mana;

	public:

	/*Set the health of the player.*/
	UFUNCTION(BlueprintCallable)
	void SetHealth(float Value);

	/*Get the health of the player.*/
	UFUNCTION(BlueprintPure)
	float GetHealth() const;

	/*The health of the player as a decimal. Used mostly for UI where widgets require a value between 0 and 1.*/
	UFUNCTION(BlueprintPure)
	float GetHealthAsDecimal() const;

	/*Add more health to the player.*/
	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount);

	/*Remove health from the player.*/
	UFUNCTION(BlueprintCallable)
	void RemoveHealth(float Amount);
	
	/*Set the mana of the player.*/
	UFUNCTION(BlueprintCallable)
	void SetMana(float Value);

	/*Get the mana of the player.*/
	UFUNCTION(BlueprintPure)
	float GetMana() const;

	/*The mana of the player as a decimal. Used mostly for UI where widgets require a value between 0 and 1.*/
	UFUNCTION(BlueprintPure)
	float GetManaAsDecimal() const;

	/*Add more mana to the player.*/
	UFUNCTION(BlueprintCallable)
	void AddMana(float Amount);

	/*Remove mana from the player.*/
	UFUNCTION(BlueprintCallable)
	void RemoveMana(float Amount);
};
