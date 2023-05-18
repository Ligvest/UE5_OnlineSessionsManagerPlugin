// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "FoundSessionListViewEntry.generated.h"


// A filter to reduce a number of found entries
USTRUCT(BlueprintType)
struct FFoundSessionListViewEntryData 
{
	GENERATED_BODY()

public:
	FString Text;
	//UPROPERTY(BlueprintReadWrite)
	//TEnumAsByte<EGameModes> GameMode = EGameModes::EGM_Default;
};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UFoundSessionListViewEntry : public UUserWidget, public IUserObjectListEntry
{

	GENERATED_BODY()


protected:
	UFUNCTION(BlueprintCallable)
	void FillWithData(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Text_SessionShortDescription;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Text_SessionIndex;

	UPROPERTY(BlueprintReadOnly)
	class UMenu* MenuReference;

	UPROPERTY(BlueprintReadOnly)
		int32 SessionIndex;
};
