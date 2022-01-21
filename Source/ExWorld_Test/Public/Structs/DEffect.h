// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Enums/DAreaTraceShape.h"
#include "Enums/DEffectType.h"
#include "DEffect.generated.h"

USTRUCT(BlueprintType)
struct FDEffect : public FTableRowBase
{
	GENERATED_BODY()
	
	/*The name of the effect.*/
	UPROPERTY(EditAnywhere)
	FString EffectName;

	/*What happens when the effect is applied to object collided with e.g. 'Destroy' will destroy the object collided with.*/
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDEffectType> EffectType;

	/*The collision channels that this effect can be applied to.*/
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypesToEffect;

	/*How much damage this effect should apply to the object collided with.*/
	UPROPERTY(EditAnywhere, meta=(EditCondition="EffectType == EDEffectType::ApplyDamage", EditConditionHides))
	float DamageToApply;

	/*The number of objects this effect can influence. If >1 you will be required to select am area trace shape.*/
	UPROPERTY(EditAnywhere, meta=(ClampMin="1"))
	int NumberOfObjectsToEffect;

	UPROPERTY(EditAnywhere, meta=(EditCondition="NumberOfObjectsToEffect > 1", EditConditionHides, DisplayName="AOE Target Caps"))
	TMap<TEnumAsByte<ECollisionChannel>, int> AoETargetCaps;

	/*The shape in which an area will be sweeped.*/
	UPROPERTY(EditAnywhere, meta=(EditCondition="NumberOfObjectsToEffect > 1", EditConditionHides))
	TEnumAsByte<EDAreaTraceShape> AreaTraceShape;

	/*The size of the box collision shape (width, height, length).*/
	UPROPERTY(EditAnywhere, meta=(EditCondition="AreaTraceShape == EDAreaTraceShape::Box && NumberOfObjectsToEffect > 1", EditConditionHides))
	FVector BoxSize;

	/*The size of the sphere collision shape (radius).*/
	UPROPERTY(EditAnywhere, meta=(EditCondition="AreaTraceShape == EDAreaTraceShape::Sphere && NumberOfObjectsToEffect > 1", EditConditionHides))
	float SphereRadius;

	/*Should the projectile be destroyed when applying this effect.*/
	UPROPERTY(EditAnywhere)
	bool bDestroyOnCollision;

	/*Determines whether or not the collision shape should be drawn as a debug shape*/
	UPROPERTY(EditAnywhere, meta=(EditCondition="NumberOfObjectsToEffect > 1 && NumberOfObjectsToEffect > 1", EditConditionHides))
	bool bDebugAreaCollision;

	UPROPERTY(EditAnywhere)
	bool bApplyEffectToInstigator;
};
