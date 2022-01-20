// Fill out your copyright notice in the Description page of Project Settings.


#include "DCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADCharacter::ADCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	
	//Rotates the character to face the direction in which they are moving
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//Prevents the character from inheriting its controller's Yaw rotation
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ADCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADCharacter::MoveForward(float Value)
{
	FRotator ControllerRot = GetControlRotation();
	ControllerRot.Pitch = 0.0f;
	ControllerRot.Roll = 0.0f;
	
	AddMovementInput(ControllerRot.Vector(), Value);
}

void ADCharacter::MoveRight(float Value)
{
	FRotator ControllerRot = GetControlRotation();
	ControllerRot.Pitch = 0.0f;
	ControllerRot.Roll = 0.0f;

	//Gets the right vector of the pawn's controller (using the Y axis)
	FVector RightVector = FRotationMatrix(ControllerRot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector, Value);
}

void ADCharacter::PrimaryAttack()
{
	if (HasAuthority())
	{
		if (bCanAttack == true)
		{
			PlayAnimMontage(AttackAnim);

			FVector HandLocation = GetMesh()->GetSocketLocation(PrimaryAttackSocket);

			//Spawn Transform Matrix
			FTransform SpawnTM = FTransform(GetControlRotation() ,HandLocation);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = this;
	
			GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

			bCanAttack = false;
			CurrentCooldown = 0;
		}
	}
	else
	{
		ServerPrimaryAttack();
	}
}

void ADCharacter::ServerPrimaryAttack_Implementation()
{
	//if (bCanAttack == true)
	//{
		PrimaryAttack();
	//}
}

void ADCharacter::PrimaryAttack_TimeElapsed()
{
	
}

// Called every frame
void ADCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCanAttack)
	{
		if (HasAuthority())
		{
			CurrentCooldown++;
			if (CurrentCooldown >= AttackCooldown)
			{
				bCanAttack = true;
			}
		}

		if (!IsRunningDedicatedServer() && CurrentCooldown < AttackCooldown)
		{
			if (!HasAuthority())
			{
				CurrentCooldown++;
			}
		}
		
	}
}

// Called to bind functionality to input
void ADCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADCharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ADCharacter::PrimaryAttack);
}

