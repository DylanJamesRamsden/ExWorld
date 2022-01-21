// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DSpellComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

#include "DCharacter.generated.h"

UCLASS()
class EXWORLD_TEST_API ADCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
    UCameraComponent* CameraComp;
    
    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArmComp;

	/*The spell component is responsible for spawning a projectile when the character attacks as well as handling the cooldown timer.*/
    UPROPERTY(VisibleAnywhere)
	UDSpellComponent* SpellComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Move the character forward and backwards based on the provided axis.*/
	void MoveForward(float Value);

	/*Move the character left and right based on the provided axis.*/
	void MoveRight(float Value);

	/*Initiates a primary attack.*/
	void PrimaryAttack();

	/*Initiate a primary attack on the server.*/
	UFUNCTION(Server, Reliable)
	void ServerPrimaryAttack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*Apply damage to the character. This function is bound to OnTakeAnyDamage.*/
	UFUNCTION(BlueprintCallable)
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	/*Get the spell component of the character.*/
	UFUNCTION(BlueprintPure)
	UDSpellComponent* GetSpellComponent() const;
};
