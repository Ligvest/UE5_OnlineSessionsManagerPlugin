// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Menu.h"
#include "FoundSessionData.generated.h"

/**
 *	A supporting data structure to pass data to newly create Item 
 * in ListViewWidget (passed as argument on ListViewWidget->AddItem )
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UFoundSessionData : public UObject
{
	GENERATED_BODY()

public:
	// A reference to use functions to have access to 
	// methods in the parent Menu from ListViewItem
	class UMenu* MenuReference;

	// Session description
	FString ShortDescription;

	// Session Index
	// This is the index of current ListViewItem in 
	// Array in the parent UMenu
	int Index;
};
