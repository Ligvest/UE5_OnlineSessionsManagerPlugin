// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#define DEBUG_MESSAGE(FString_MessageText, FColor_MessageColor) { \
		if (GEngine) {\
			GEngine->AddOnScreenDebugMessage(\
					- 1, \
					8.f, \
					FColor_MessageColor, \
					FString_MessageText, true, FVector2D(1.5f, 1.5f)\
				); \
		} \
}

#include "MultiplayerSessionsSubsystem.generated.h"


// All custom session settings
enum ESessionSettings {
	GameMode,

	ESessionSettingsSize,
};

// All gamemodes
UENUM()
enum EGameModes {
	Default UMETA(DisplayName = "Default"),

	EGameModesSize UMETA(DisplayName = "EGameModesSize"),
};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

// Ctors, Dtors
public:
	UMultiplayerSessionsSubsystem();


// Interface
public:
	void CreateSession(int NumPublicConnections, EGameModes GameMode);
	void StartSession();
	void FindSessions(int MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void HostLobby(int NumPublicConnections, EGameModes GameMode, const FString& LobbyMapURL);


protected:
	// On sessions operations complete callbacks
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	

private:
	// On sessions operations complete delegates
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;


	// Creating delegate handles to be able to clear delegates
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;


	IOnlineSessionPtr OnlineSessionPtr;
	TSharedPtr<FOnlineSessionSearch> SessionsSearchSettingsPtr;

	FName CurrentSessionName;
	FName SubsystemName;
	TArray<FName, TFixedAllocator<ESessionSettings::ESessionSettingsSize>> SessionSettingsKeys;
	TArray<FString, TFixedAllocator<EGameModes::EGameModesSize>> GameModesArray;
};

