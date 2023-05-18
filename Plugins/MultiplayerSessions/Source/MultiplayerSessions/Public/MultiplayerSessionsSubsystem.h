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

// Delegates to pass info from this class to its users.
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsResultReady, const TArray<UFoundSessionData*>&, SearchResults, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsResultReady, TArray<FOnlineSessionSearchResult> SearchResults, bool bWasSuccessful);

// All custom session settings
enum ESessionSettings {
	ESS_GameMode,

	ESessionSettingsSize,
};

// All gamemodes
UENUM()
enum EGameModes {
	EGM_Default UMETA(DisplayName = "Default"),

	EGameModesSize UMETA(DisplayName = "EGameModesSize"),
};

// A filter to reduce a number of found entries
USTRUCT(BlueprintType)
struct FSearchFilter 
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EGameModes> GameMode = EGameModes::EGM_Default;
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
	void FindSessions(int MaxSearchResults, const FSearchFilter& Filter);
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void HostLobby(int NumPublicConnections, EGameModes GameMode, const FString& LobbyMapURL);

	FOnFindSessionsResultReady OnFindSessionsResultReadyDelegate;

	// Convert Enumeration values to strings
	TArray<FName, TFixedAllocator<ESessionSettings::ESessionSettingsSize>> SessionSettingsKeys;
	TArray<FString, TFixedAllocator<EGameModes::EGameModesSize>> GameModesArray;


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

};

