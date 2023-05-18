// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "FoundSessionListViewEntry.generated.h"

/**
 * A class that represents ListViewItem with session info in ListViewWidget
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UFoundSessionListViewEntry : public UUserWidget, public IUserObjectListEntry
{

	GENERATED_BODY()


protected:
	/* We can't cast in blueprint from UObject which is passed in AddItem function to 
	 * UFoundSessionData. So we just get this passed object in blueprints and 
	 * forward it to our FillWithData function which will set all necessary variables ( including widget once )
	 */
	UFUNCTION(BlueprintCallable)
	void FillWithData(UObject* ListItemObject);

	// Session description text field in child widget blueprint
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Text_SessionShortDescription;

	// Session index text field in child widget blueprint
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Text_SessionIndex;

	// A reference to have access to the parent Menu methods from the ListViewEntry
	UPROPERTY(BlueprintReadOnly)
	class UMenu* MenuReference;
};
