// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "FoundSessionData.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	// Connecting all our delegates with methods which should be executed
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{
	// Initialize session interfaces
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem) {
		OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
		// A subsystem name. Will be NULL if no subsystem detected.  
		// NULL is default UE subsystem. Should be Steam in my case
		SubsystemName = OnlineSubsystem->GetSubsystemName();
		DEBUG_MESSAGE(FString::Printf(TEXT("Subsystem name is \"%s\""), *SubsystemName.ToString()), FColor::Yellow);
	}

	CurrentSessionName = FName(TEXT("DefaultSession"));

	// We should have this variable alive to use it in two functions.
	// Using this variable in FindSessions and then in OnFindSessionsComplete
	SessionsSearchSettingsPtr = MakeShared<FOnlineSessionSearch>();

	// Initializing converter from ESessionSettings enum to FName
	SessionSettingsKeys.SetNum(ESessionSettings::ESessionSettingsSize);
	SessionSettingsKeys[ESessionSettings::ESS_GameMode] = FName(TEXT("GameMode"));

	// Initializing converter from EGameModesSize enum to FString
	GameModesArray.SetNum(EGameModes::EGameModesSize);
	GameModesArray[EGameModes::EGM_Default] = FString(TEXT("DefaultMode"));
}


void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (OnlineSessionPtr.IsValid()) {
		// Adding a delegate to be executed on CreateSession is completed
		if (!OnCreateSessionCompleteDelegateHandle.IsValid()) {
			OnCreateSessionCompleteDelegateHandle = OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
		}

		// Adding a delegate to be executed on FindSessions is completed
		if (!OnFindSessionsCompleteDelegateHandle.IsValid()) {
			OnFindSessionsCompleteDelegateHandle = OnlineSessionPtr->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
		}

		// Adding a delegate to be executed on JoinSession is completed
		if (!OnJoinSessionCompleteDelegateHandle.IsValid()) {
			OnJoinSessionCompleteDelegateHandle = OnlineSessionPtr->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		}

		// Adding a delegate to be executed on DestroySession is completed
		if (!OnDestroySessionCompleteDelegateHandle.IsValid()) {
			OnDestroySessionCompleteDelegateHandle = OnlineSessionPtr->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
		}
	}
}


/*
Creates a session
int NumPublicConnections - how much people can connect
EGameModes GameMode - game mode to create. Probably should be replaced with Filter structure. 
*/
void UMultiplayerSessionsSubsystem::CreateSession(int NumPublicConnections, EGameModes GameMode)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	DEBUG_MESSAGE(FString(TEXT("Creating a Session")), FColor::Yellow);

	// Filling session settings before creating the session
	TUniquePtr<FOnlineSessionSettings> SessionSettingsPtr = MakeUnique<FOnlineSessionSettings>();
	// set to false if we are connected to any subsystem ( e.g. Steam )
	// or to true if no subsystem is "NULL" which is UE default
	SessionSettingsPtr->bIsLANMatch = SubsystemName.IsEqual(FName(TEXT("NULL"))) ? true : false;

	// How much players can connect
	SessionSettingsPtr->NumPublicConnections = NumPublicConnections;
	
	// Allow joining after a session is started?
	SessionSettingsPtr->bAllowJoinInProgress = true;

	// Should be visible in steam
	SessionSettingsPtr->bShouldAdvertise = true;

	// Wrote about Presence below
	SessionSettingsPtr->bUsesPresence = true;

	// Presence is a system when you press on your friend in Steam and 
	// see information about a game he plays. You can press join button there to connect to the game
	SessionSettingsPtr->bAllowJoinViaPresence = true;

	// To be able to connect?
	SessionSettingsPtr->bUseLobbiesIfAvailable = true;

	// Maybe allow to invite from the game?
	SessionSettingsPtr->bAllowInvites = true;

	// We are not using dedicated server
	SessionSettingsPtr->bIsDedicated = false;

	//SessionSettingsPtr->bAllowJoinViaPresenceFriendsOnly = true; // Can't find the session when this parameter is true

	// Adding custom settings. 
	SessionSettingsPtr->Set(SessionSettingsKeys[ESessionSettings::ESS_GameMode], GameModesArray[EGameModes::EGM_Default], EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	OnlineSessionPtr->CreateSession(0, CurrentSessionName, *SessionSettingsPtr);

	// Here the lector checks for success of session creation and clean up 
	// delegate if the creation is not successful. Later will add this if it will be needed
}


// To Implement: Not implemented
void UMultiplayerSessionsSubsystem::StartSession()
{

}

// To Implement: Not implemented
void UMultiplayerSessionsSubsystem::Disconnect()
{
	// Destroy existing session
	DestroySessionIfCreated();
	// Travel a player back to main menu
}


/*
Search for sessions
int MaxSearchResultes - number of results could be found. Should be 10 000+ for some reason.
const FSearchFilter& Filter - filter structure to reduce number of results
*/
void UMultiplayerSessionsSubsystem::FindSessions(int MaxSearchResults,  const FSearchFilter& Filter)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	SessionsSearchSettingsPtr->MaxSearchResults = MaxSearchResults;

	// With this option set no sessions are found
	// To Implement: maybe later will check how it works. 
	//SessionsSearchSettingsPtr->QuerySettings.Set(SessionSettingsKeys[ESessionSettings::GameMode], FString(TEXT("Ligvest")), EOnlineComparisonOp::Equals);

	// We don't search for Lan games
	SessionsSearchSettingsPtr->bIsLanQuery = false;

	// Presence should be supported
	SessionsSearchSettingsPtr->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	DEBUG_MESSAGE(FString(TEXT("Start searching")), FColor::Yellow);

	OnlineSessionPtr->FindSessions(0, SessionsSearchSettingsPtr.ToSharedRef());
}


/*
Join a session
const FOnlineSessionSearchResult& SearchResult - a session to connect to
*/
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	DEBUG_MESSAGE(FString(TEXT("Trying to join a session")), FColor::Yellow);
	OnlineSessionPtr->JoinSession(0, CurrentSessionName, SearchResult);
}

/*
 Destroys a session if it's existing
*/
void UMultiplayerSessionsSubsystem::DestroySessionIfCreated()
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	FNamedOnlineSession* ExistingSession = OnlineSessionPtr->GetNamedSession(CurrentSessionName);
	if (ExistingSession) {
		DEBUG_MESSAGE(FString(TEXT("Destroying a session")), FColor::Yellow);
		OnlineSessionPtr->DestroySession(CurrentSessionName);
	}
}

/*
Combine CreateSession and ServerTravel to travel to a lobby. Traveling to a lobby is done in 
OnCreateSessionComplete method. But if you want not to travel then just pass empty string in LobbyMapURL parameter
int NumPublicConnections - how much people can connect
EGameModes GameMode - game mode to create. Probably should be replaced with Filter structure. 
const FString& LobbyMapURL - Path to the map to be used as a lobby to travel to
*/
void UMultiplayerSessionsSubsystem::HostLobby(int NumPublicConnections, EGameModes GameMode, const FString& LobbyMapURL)
{
	CreateSession(4, EGameModes::EGM_Default);
	LastLobbyMapURL = LobbyMapURL;
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		DEBUG_MESSAGE(FString(TEXT("Session was created")), FColor::Green);
		if (LastLobbyMapURL == TEXT("")) {
			DEBUG_MESSAGE(FString(TEXT("Lobby map url isn't specified")), FColor::Red);
		}
		else {
			UWorld* World = GetWorld();
			if (World) {
				World->ServerTravel(LastLobbyMapURL);
			}
		}
	}
	else {
		DEBUG_MESSAGE(FString(TEXT("Session couldn't be created")), FColor::Red);
	}
}
		
// To Implement: Not implemented
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{

}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionsSearchSettingsPtr->SearchState == EOnlineAsyncTaskState::Failed) {
		DEBUG_MESSAGE(FString(TEXT("Session search failed. Return")), FColor::Red);
		return;
	}

	DEBUG_MESSAGE(FString(TEXT("Session search finished. Found results:")), FColor::Green);

	OnFindSessionsResultReadyDelegate.Broadcast(SessionsSearchSettingsPtr->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult)
{
	if (JoinSessionResult != EOnJoinSessionCompleteResult::Type::Success) {
		DEBUG_MESSAGE(FString::Printf(TEXT("Couldn't join. The reason: %s"), LexToString(JoinSessionResult)), FColor::Red);
		return;
	}

	if (!OnlineSessionPtr.IsValid()) {
		return;
	}


	FString ServerAddress{};
	OnlineSessionPtr->GetResolvedConnectString(CurrentSessionName, ServerAddress);

	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (PC) {
		PC->ClientTravel(ServerAddress, ETravelType::TRAVEL_Absolute);
	}

	DEBUG_MESSAGE(FString::Printf(TEXT("Successfuly joined a session")), FColor::Green);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	DEBUG_MESSAGE(FString::Printf(TEXT("Successfuly destroyed a session")), FColor::Green);
}
