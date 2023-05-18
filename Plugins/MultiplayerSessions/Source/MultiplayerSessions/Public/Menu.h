// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"

#include "Menu.generated.h"

/**
 * A class to be a child for Menus which implement functionality of
 * working with sessions ( CreateSession, SearchSessions, JoinSession )
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

// Methods
protected:
	// Set the menu visible, change input mode etc.
	// Call whenever you want to begin to interact with the menu
	UFUNCTION(BlueprintCallable)
	void SetupMenu();

	// Before you destroy the menu you should call this 
	// To disable visibility, change input mode back etc.
	UFUNCTION(BlueprintCallable)
	void BeforeRemoval();

	// Override function which is called when current level is destroyed
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;


	/* Create session and move to the lobby
	 * which is located here FString(TEXT("/Game/Maps/Lobby?listen"))
     * To Implement: later could be added as parameter of the method
	 */ 
	UFUNCTION(BlueprintCallable)
	void HostSession();

	/* SearchSessions calls OnSearchSessionsComplete after the search completed
	 * MaxEntriesNumber - Amount of results which could be found. Should be 10 000+
	 * Filter - Some rules to specify a game we are looking for. To Implement
	 */
	UFUNCTION(BlueprintCallable)
	void SearchSessions(int MaxEntriesNumber, const FSearchFilter& Filter);


	// Function to work with search results after SearchSessions completed
	void OnSearchSessionsComplete(TArray<FOnlineSessionSearchResult> SearchResults, bool bWasSuccessful);

	/*
	 * Join a session.
	 * ID - this is the ID from UFoundSessionListViewEntry::Text_SessionIndex. 
	 * This is the index of the session in RecentSearchResults array
	 */
	UFUNCTION(BlueprintCallable)
	void JoinSession(int32 ID);

	// Returns UMultiplayerSessionSubsystem* if success or nullptr otherwise
	inline class UMultiplayerSessionsSubsystem* GetSessionsSubsystem();

// Members
public:

	// a reference to ListView of sessions in UserWidget
	UPROPERTY(meta = (BindWidget))
	class UListView* ListView_Sessions;

protected:
	// An array which holds a copy of all searchresults to be able to 
	// join any game having only the index of the game 
	TArray<FOnlineSessionSearchResult> RecentSearchResults;
};
