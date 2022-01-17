// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
    
    UPROPERTY(EditAnywhere, Category = "Attack")
    TSubclassOf<AActor> ProjectileClass;
    
    UPROPERTY(EditAnywhere, Category = "Attack")
    FName PrimaryAttackSocket;
    
    UPROPERTY(EditAnywhere, Category = "Attack")
    UAnimMontage* AttackAnim;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Moves the character forward and backwards
	void MoveForward(float Value);

	//Moves the character left and right
	void MoveRight(float Value);

	//Initiates a primary attack timer and animation
	void PrimaryAttack();

	//Executes after an elapsed time. Calls an RPC to the server to spawn a projectile
	void PrimaryAttack_TimeElapsed();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
