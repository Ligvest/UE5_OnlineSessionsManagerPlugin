// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem) {
		OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
		SubsystemName = OnlineSubsystem->GetSubsystemName();
	}

	CurrentSessionName = FName(TEXT("DefaultSession"));
	SessionsSearchSettingsPtr = MakeShared<FOnlineSessionSearch>();

	// Initializing custom Session settings
	SessionSettingsKeys.SetNum(ESessionSettings::ESessionSettingsSize);
	SessionSettingsKeys[ESessionSettings::ESS_GameMode] = FName(TEXT("GameMode"));

	// Initializing list of game modes
	GameModesArray.SetNum(EGameModes::EGameModesSize);
	GameModesArray[EGameModes::EGM_Default] = FString(TEXT("DefaultMode"));
}

void UMultiplayerSessionsSubsystem::CreateSession(int NumPublicConnections, EGameModes GameMode)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	if (!OnCreateSessionCompleteDelegateHandle.IsValid()) {
		OnCreateSessionCompleteDelegateHandle = OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	}

	// Should be tested.
	//FNamedOnlineSession* ExistingSession = OnlineSessionPtr->GetNamedSession(CurrentSessionName);
	//if (ExistingSession) {
	//	OnlineSessionPtr->DestroySession(CurrentSessionName);
	//}

	// Debug
	DEBUG_MESSAGE(FString(TEXT("Creating a Session")), FColor::Yellow);

	TUniquePtr<FOnlineSessionSettings> SessionSettingsPtr = MakeUnique<FOnlineSessionSettings>();
	SessionSettingsPtr->bIsLANMatch = SubsystemName.IsEqual(FName(TEXT("NULL"))) ? true : false;
	SessionSettingsPtr->NumPublicConnections = NumPublicConnections;
	SessionSettingsPtr->bAllowJoinInProgress = true;
	SessionSettingsPtr->bShouldAdvertise = true;
	SessionSettingsPtr->bAllowJoinViaPresence = true;
	SessionSettingsPtr->bUsesPresence = true;
	SessionSettingsPtr->bUseLobbiesIfAvailable = true;

	SessionSettingsPtr->bAllowInvites = true;
	SessionSettingsPtr->bIsDedicated = false;
	//SessionSettingsPtr->bAllowJoinViaPresenceFriendsOnly = true; // Can't find the session when this parameter is true
	SessionSettingsPtr->Set(SessionSettingsKeys[ESessionSettings::ESS_GameMode], GameModesArray[EGameModes::EGM_Default], EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	OnlineSessionPtr->CreateSession(0, CurrentSessionName, *SessionSettingsPtr);
	// Here the lector checks for success of session creation and clean up 
	// delegate if the creation is not successful. Later will add this if it will be needed
}

void UMultiplayerSessionsSubsystem::StartSession()
{

}

void UMultiplayerSessionsSubsystem::FindSessions(int MaxSearchResults,  const FSearchFilter& Filter)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	if (!OnFindSessionsCompleteDelegateHandle.IsValid()) {
		OnFindSessionsCompleteDelegateHandle = OnlineSessionPtr->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	}

	SessionsSearchSettingsPtr->MaxSearchResults = MaxSearchResults;
	//SessionsSearchSettingsPtr->QuerySettings.Set(SessionSettingsKeys[ESessionSettings::GameMode], FString(TEXT("Ligvest")), EOnlineComparisonOp::Equals);
	SessionsSearchSettingsPtr->bIsLanQuery = false;
	SessionsSearchSettingsPtr->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Debug
	DEBUG_MESSAGE(FString(TEXT("Start searching...")), FColor::Orange);

	OnlineSessionPtr->FindSessions(0, SessionsSearchSettingsPtr.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	if (!OnJoinSessionCompleteDelegateHandle.IsValid()) {
		OnJoinSessionCompleteDelegateHandle = OnlineSessionPtr->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	}

	// Debug
	//DEBUG_MESSAGE(FString(TEXT("Trying to join a session...")), FColor::Yellow);
	OnlineSessionPtr->JoinSession(0, CurrentSessionName, SearchResult);
}

void UMultiplayerSessionsSubsystem::DestroySession()
{

}

void UMultiplayerSessionsSubsystem::HostLobby(int NumPublicConnections, EGameModes GameMode, const FString& LobbyMapURL)
{
	CreateSession(4, EGameModes::EGM_Default);
	UWorld* World = GetWorld();
	if (World) {
		World->ServerTravel(LobbyMapURL);
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Debug
	if (bWasSuccessful) {
		DEBUG_MESSAGE(FString(TEXT("Session was created")), FColor::Green);
	}
	else {
		DEBUG_MESSAGE(FString(TEXT("Session couldn't be created")), FColor::Red);
	}
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{

}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionsSearchSettingsPtr->SearchState == EOnlineAsyncTaskState::Failed) {
		// Debug
		DEBUG_MESSAGE(FString(TEXT("Session search failed. Return")), FColor::Red);
		return;
	}

	// Debug
	DEBUG_MESSAGE(FString(TEXT("Session search finished. Found results:")), FColor::Green);

	//for (const FOnlineSessionSearchResult& SearchResult : SessionsSearchSettingsPtr->SearchResults) {
	//	FString OwnerName = SearchResult.Session.OwningUserName;
	//	FString GameMode{};
	//	SearchResult.Session.SessionSettings.Get(SessionSettingsKeys[ESessionSettings::GameMode], GameMode);

	//	// Debug
	//	DEBUG_MESSAGE(FString::Printf(TEXT("Owner name: %s, Game mode: %s"), *OwnerName, *GameMode), FColor::Yellow);

	//	if (GameMode.Equals(GameModesArray[EGameModes::Default])) {
	//		// Debug
	//		//DEBUG_MESSAGE(FString::Printf(TEXT("Trying to join the first suitable game...")), FColor::Yellow);
	//		//JoinSession(SearchResult);
	//	}
	//}

	OnFindSessionsResultReadyDelegate.Broadcast(SessionsSearchSettingsPtr->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult)
{
	if (JoinSessionResult != EOnJoinSessionCompleteResult::Type::Success) {
		// Debug
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


	// Debug
	DEBUG_MESSAGE(FString::Printf(TEXT("Successfuly joined a session")), FColor::Green);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

}
