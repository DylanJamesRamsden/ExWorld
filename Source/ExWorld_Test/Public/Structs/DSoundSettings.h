// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DSoundSettings.generated.h"

USTRUCT(BlueprintType)
struct FDSoundSettings
{
	GENERATED_BODY()
	
	/*The volume of the master audio.*/
	UPROPERTY(BlueprintReadWrite)
	float MasterVolume;

	/*The volume of the music audio.*/
	UPROPERTY(BlueprintReadWrite)
	float MusicVolume;

	/*The volume of the SFX audio.*/
	UPROPERTY(BlueprintReadWrite)
	float SFXVolume;

	/*The volume of the dialogue audio.*/
	UPROPERTY(BlueprintReadWrite)
	float DialogueVolume;
};
