// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DProjectile.h"
#include "Components/ActorComponent.h"
#include "DSpellComponent.generated.h"


class ADCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXWORLD_TEST_API UDSpellComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDSpellComponent();

protected:
	/*The owning character of this component.*/
	UPROPERTY()
	ADCharacter* OwningCharacter;

	/*The projectile class that will be spawned as a projectile when a spell is cast.*/
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	/*The socket from where the projectile will spawn from.*/
	UPROPERTY(EditAnywhere, Category = "Attack")
	FName PrimaryAttackSocket;

	/*An optional animation that will be played when a spell is cast.*/
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* CastAnimation;

	/*The time in which a spell can be recast.*/
	UPROPERTY(EditAnywhere, Category = "Attack")
	float SpellCooldownTime;

	UPROPERTY()
	float CurrentSpellCooldownTime;

	/*The mana cost of the spell.*/
	UPROPERTY(EditAnywhere, Category = "Attack")
	float ManaCost = 20;

	/*If the spell can be casted or not.*/
	UPROPERTY()
	bool bCanCast = false;

	/*A handle to the timer that is used to spawn a projectile during the cast animation.*/
	UPROPERTY()
	FTimerHandle WindUpTimerHandle;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	/*Spawns a projectile into the world.*/
	void SpawnProjectile() const;

	/*Plays the cast animation on all of the clients.*/
	UFUNCTION(NetMulticast, Reliable)
	void ClientPlayAttackAnimation();

	/*Resets the spell's cooldown timer on the client.*/
	UFUNCTION(Client, Reliable)
	void ClientResetCooldownTimer();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/*Whether or not the spell can be casted based on the given mana and the mana cost of the spell.*/
	UFUNCTION(BlueprintPure)
	bool IsSpellAffordable(float CurrentMana) const;

	/*Initiates the casting of a spell.*/
	UFUNCTION(BlueprintNativeEvent)
	void CastSpell();

	UFUNCTION(BlueprintPure)
	bool CanCastSpell();

	UFUNCTION(BlueprintPure)
	int GetCurrentSpellCooldownTime();

	UFUNCTION(BlueprintCallable)
	void SetProjectile(TSubclassOf<AActor> NewProjectile);
};
