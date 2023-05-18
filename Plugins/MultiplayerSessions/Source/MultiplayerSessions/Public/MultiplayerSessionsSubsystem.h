// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

/* Just a wrapper over GEngine->AddOnScreenDebugMessage
 * FString_MessageText - A text to be displayed ( FString type )
 * FColor_MessageColor - A color of the text (FColor type )
 */
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

// Passes SearchResults array when FindSessions completes
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsResultReady, TArray<FOnlineSessionSearchResult> SearchResults, bool bWasSuccessful);

// Enumeration of custom settings which can be added on session creation. 
// FOnlineSessionSettings->Set can't use enum as input parameters so we must convert them to string
// To get theirs FName version use SessionSettingsKeys array
enum ESessionSettings {
	ESS_GameMode,

	ESessionSettingsSize,
};

// Enumeration of game modes which can be added on session creation.
// FOnlineSessionSettings->Set can't use enum as input parameters so we must convert them to string
// To get theirs FString version use GameModesArray array
UENUM()
enum EGameModes {
	EGM_Default UMETA(DisplayName = "Default"),

	EGameModesSize UMETA(DisplayName = "EGameModesSize"),
};

// A filter to reduce a number of found entries
// Used to set its variables and then pass whole structure to 
// create/find session with the parameters
// To Implement: can be implemented in future if needed
USTRUCT(BlueprintType)
struct FSearchFilter 
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EGameModes> GameMode = EGameModes::EGM_Default;
};

/**
 * A class dedicated to manage sessions ( create, find, join, destroy )
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

// Ctors, Dtors
public:
	UMultiplayerSessionsSubsystem();


public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

// Methods
public:
	/*
	Creates a session
	int NumPublicConnections - how much people can connect
	EGameModes GameMode - game mode to create. Probably should be replaced with Filter structure. 
	*/
	void CreateSession(int NumPublicConnections, EGameModes GameMode);

	// To Implement: Not implemented
	void StartSession();

	// To Implement: Should destroy a session 
	// and travel a player back to main menu
	void Disconnect();

	/*
	Search for sessions
	int MaxSearchResultes - number of results could be found. Should be 10 000+ for some reason.
	const FSearchFilter& Filter - filter structure to reduce number of results
	*/
	void FindSessions(int MaxSearchResults, const FSearchFilter& Filter);

	/*
	Join a session
	const FOnlineSessionSearchResult& SearchResult - a session to connect to
	*/
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);

	/*
	 Destroys a session if it's existing
	*/
	void DestroySessionIfCreated();

	/*
	Combine CreateSession and ServerTravel to travel to a lobby. Traveling to a lobby is done in 
	OnCreateSessionComplete method. But if you want not to travel then just pass empty string in LobbyMapURL parameter
	int NumPublicConnections - how much people can connect
	EGameModes GameMode - game mode to create. Probably should be replaced with Filter structure. 
	const FString& LobbyMapURL - Path to the map to be used as a lobby to travel to
	*/
	UFUNCTION(BlueprintCallable)
	void HostLobby(int NumPublicConnections, EGameModes GameMode, const FString& LobbyMapURL);

protected:
	// Called after CreateSession is completed
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// Called after StartSession is completed
	// To Implement: Not implemented
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	// Called after FindSession is completed
	void OnFindSessionsComplete(bool bWasSuccessful);

	// Called after JoinSession is completed
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult);

	// Called after DestroySession is completed
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);


// Members
public:

	// Broadcasting at the end of OnFindSessionsComplete method
	FOnFindSessionsResultReady OnFindSessionsResultReadyDelegate;

	FString LastLobbyMapURL;

	// Convert ESessionSettings enumeration to FName to pass it to FOnlineSessionSettings::Set 
	TArray<FName, TFixedAllocator<ESessionSettings::ESessionSettingsSize>> SessionSettingsKeys;

	// Convert EGameModes enumeration to FString to pass it to FOnlineSessionSettings::Set  
	TArray<FString, TFixedAllocator<EGameModes::EGameModesSize>> GameModesArray;


private:
	// On sessions operations complete delegates
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;


	// Creating delegate handles to be able to manage and clear delegates
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;


	IOnlineSessionPtr OnlineSessionPtr;

	// We should have this variable alive to use it in two functions.
	// Using this variable in FindSessions and then in OnFindSessionsComplete
	TSharedPtr<FOnlineSessionSearch> SessionsSearchSettingsPtr;

	FName CurrentSessionName;
	FName SubsystemName;
};

