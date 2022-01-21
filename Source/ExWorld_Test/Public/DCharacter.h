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
    
    UPROPERTY(VisibleAnywhere)
	UDSpellComponent* SpellComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Moves the character forward and backwards
	void MoveForward(float Value);

	//Moves the character left and right
	void MoveRight(float Value);

	//Initiates a primary attack timer and animation
	void PrimaryAttack();

	UFUNCTION(Server, Reliable)
	void ServerPrimaryAttack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintPure)
	UDSpellComponent* GetSpellComponent() const;
};
