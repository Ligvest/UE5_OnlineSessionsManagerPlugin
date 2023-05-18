// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Menu.h"
#include "FoundSessionData.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UFoundSessionData : public UObject
{
	GENERATED_BODY()

public:
	class UMenu* MenuReference;
	FString ShortDescription;
	int Index;
};
