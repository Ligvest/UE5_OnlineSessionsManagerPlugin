// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerSessionsSubsystem.h"
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
	void CreateGameSession();

	UFUNCTION(BlueprintCallable)
	void FindSessions(int MaxEntriesNumber, const FSearchFilter& Filter);

	//UFUNCTION(BlueprintCallable)
	//void JoinGameSession(const FOnlineSessionSearchResult& SessionToJoin);

	//class FOnlineSessionSearchResult* CurrentSession;

};
