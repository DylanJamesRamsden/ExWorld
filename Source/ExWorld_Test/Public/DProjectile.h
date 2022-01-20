// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Structs/DEffect.h"

#include "DProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FApplyEffect, AActor*, AffectedActor, FDEffect, EffectApplied);

UCLASS()
class EXWORLD_TEST_API ADProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, Category = "Projectile traits:")
	FDataTableRowHandle EffectHandle;

	FDEffect* Effect;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsEffectableActor(UPrimitiveComponent* OverlappedComp);

public:
	UPROPERTY(BlueprintAssignable)
	FApplyEffect OnApplyEffect;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void ServerDestroyHitActor(AActor* Actor);

	UFUNCTION(Client, Reliable)
	void ClientDestroyHitActor(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void ServerApplyEffect(FHitResult HitResult);

	UFUNCTION()
	void ApplyEffect(FHitResult HitResult);

	UFUNCTION()
	void AOEOverlap(FVector OverlapOrigin, EDAreaTraceShape OverlapShape, EDEffectType EffectType);
};
