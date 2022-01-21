// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structs/DSoundSettings.h"

#include "DSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class EXWORLD_TEST_API UDSaveGame : public USaveGame
{
	GENERATED_BODY()

	public:

	UDSaveGame();
	
	UPROPERTY(BlueprintReadWrite)
	FDSoundSettings SoundSettings;

	//Can be extended to include other game values
};
