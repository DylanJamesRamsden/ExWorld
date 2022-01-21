// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY()
	ADCharacter* OwningCharacter;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
    
	UPROPERTY(EditAnywhere, Category = "Attack")
	FName PrimaryAttackSocket;
    
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* CastAnimation;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float SpellCooldownTime;

	UPROPERTY()
	float CurrentSpellCooldownTime;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float ManaCost = 20;

	UPROPERTY()
	bool bIsAvailable = false;

	UPROPERTY()
	FTimerHandle WindUpTimerHandle;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnProjectile() const;

	UFUNCTION(NetMulticast, Reliable)
	void ClientPlayAttackAnimation();

	UFUNCTION(Client, Reliable)
	void ClientResetCooldownTimer();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool IsSpellAffordable(float CurrentMana) const;

	UFUNCTION()
	void CastSpell();

	UFUNCTION()
	bool IsSpellAvailable();

	UFUNCTION(BlueprintPure)
	int GetCurrentSpellCooldownTime();
};
