// Fill out your copyright notice in the Description page of Project Settings.


#include "DCharacter.h"

#include "DPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADCharacter::ADCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//The spring arm component is created and attached to the root component of the character.
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	//The camera component is created and attached to the spring arm component.
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	//A spell component is created. This component will handle the spawning of a projectile and it's cooldown.
	SpellComponent = CreateDefaultSubobject<UDSpellComponent>("SpellComp");
	
	//Rotates the character to face the direction in which they are moving.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//Prevents the character from inheriting its controller's Yaw rotation.
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ADCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Binds the function that applies the damage to our character to the OnTakeAnyDamage signature.
	OnTakeAnyDamage.AddDynamic(this, &ADCharacter::TakeDamage);
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

	//Gets the right vector of the pawn's controller (using the Y axis).
	FVector RightVector = FRotationMatrix(ControllerRot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector, Value);
}

void ADCharacter::PrimaryAttack()
{
	//A primary attack must be called on the server, so if the server has not called it then a server RPC of the primary attack is forced.
	if (HasAuthority())
	{
		if (SpellComponent)
		{
			if (SpellComponent->CanCastSpell())
			{
				SpellComponent->CastSpell();
			}	
		}
	}
	else
	{
		ServerPrimaryAttack();
	}
}

void ADCharacter::ServerPrimaryAttack_Implementation()
{
	PrimaryAttack();
}

// Called every frame
void ADCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADCharacter::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	ADPlayerState* OwningPlayerState = Cast<ADPlayerState>(GetPlayerState());

	if (OwningPlayerState)
	{
		OwningPlayerState->RemoveHealth(Damage);
	}
}

UDSpellComponent* ADCharacter::GetSpellComponent() const
{
	return SpellComponent;
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

