// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundSessionListViewEntry.h"
#include "Components/TextBlock.h"
#include "FoundSessionData.h"

void UFoundSessionListViewEntry::FillWithData(UObject* ListItemObject)
{
	//UFoundSessionData* SessionData = Cast<UFoundSessionData*>(ListItemObject);
	UFoundSessionData* SessionData = dynamic_cast<UFoundSessionData*>(ListItemObject);


	FText NewText = FText::FromString(TEXT("ERROR: Passed not SessionData type"));
	int NewIndex = -1;
	if (SessionData) {
		//Text_SessionShortDescription->SetText(FText(TEXT("Hello from C++ and bound variable")));
		NewText = FText::FromString(SessionData->ShortDescription);
		NewIndex = SessionData->Index;
		MenuReference = SessionData->MenuReference;
	}
	
	if (Text_SessionShortDescription) {
		Text_SessionShortDescription->SetText(NewText);
	}

	if (Text_SessionIndex) {
		Text_SessionIndex->SetText(FText::AsNumber(NewIndex));
	}
}
