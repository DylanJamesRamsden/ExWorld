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
	
	/*The data table row of the effect you want to apply with this projectile.*/
	UPROPERTY(EditAnywhere, Category = "Projectile traits:")
	FDataTableRowHandle EffectHandle;

	/*A strict containing all the information required about the selected effect this projectile will apply.*/
	FDEffect* Effect;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Handles the collision of the projectile when it overlaps another object. This function is bound to the OnOverlapBegin signature of the projectiles sphere component.*/
	UFUNCTION()
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*Runs ApplyEffect() on the server. Ensures that the server verifies that the effect can and is applied.*/
	UFUNCTION(Server, Reliable)
	void ServerApplyEffect(FHitResult HitResult);

	/*Applies the selected effect to a single given actor*/
	UFUNCTION()
	void ApplyEffectToSingleActor(AActor* HitActor);

	/*Preform a overlap check for multiple objects based on a channel. All overlapped actors have the selected effect applied to them if they satisfy the effect's conditions.*/
	UFUNCTION()
	void ApplyEffectAsAOE(FVector OverlapOrigin);

	/*Calls for the destruction of an actor over the server, resulting in a client RPC destroying the actor locally.*/
	UFUNCTION(Server, Reliable)
	void ServerDestroyHitActor(AActor* Actor);
	
	/*Destroys a given actor on the client.*/
	UFUNCTION(Client, Reliable)
	void ClientDestroyHitActor(AActor* Actor);

	/*Checks if the actor collided with is an object type that the effect is targeting.*/
	bool IsActorEffectTargetObjectType(UPrimitiveComponent* OverlappedComp) const;

	/*Draw debug shape at the overlap origin point. Drawn shape is based on the given effect's area trace shape*/
	void DebugAOETrace(FVector OverlapOrigin) const;

public:
	/*Signature of the delegate that is broadcasted when an effect is applied to another object.*/
	UPROPERTY(BlueprintAssignable)
	FApplyEffect OnApplyEffect;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*Applies the selected effect to a given object.*/
	UFUNCTION()
	void ApplyEffect(FHitResult HitResult);
};
