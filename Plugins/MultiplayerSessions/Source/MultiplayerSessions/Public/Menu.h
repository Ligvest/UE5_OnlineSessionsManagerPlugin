// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	UFUNCTION(BlueprintCallable)
	void SetupMenu();

	UFUNCTION(BlueprintCallable)
	void BeforeRemoval();


	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	inline class UMultiplayerSessionsSubsystem* GetSessionsSubsystem();

	UFUNCTION(BlueprintCallable)
	void HostSession();

	UFUNCTION(BlueprintCallable)
	void SearchSessions(int MaxEntriesNumber, const FSearchFilter& Filter);

	UFUNCTION(BlueprintCallable)
	void JoinSession(int32 ID);//const UFoundSessionData* SessionToJoin);

	void OnSearchSessionsComplete(TArray<FOnlineSessionSearchResult> SearchResults, bool bWasSuccessful);

	TArray<FOnlineSessionSearchResult> RecentSearchResults;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UListView* ListView_Sessions;

	//TArray<UFoundSessionData> FoundSessionDataArray;

	//UFUNCTION(BlueprintCallable)
	//void JoinSession(const FOnlineSessionSearchResult* SessionToJoin);

	//class FOnlineSessionSearchResult* CurrentSession;
};
