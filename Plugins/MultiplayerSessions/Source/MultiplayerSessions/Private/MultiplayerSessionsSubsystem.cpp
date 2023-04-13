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
	SessionSettingsKeys.SetNum(ESessionSettings::EnumSize);
	SessionSettingsKeys[ESessionSettings::GameMode] = "GameMode";
}

void UMultiplayerSessionsSubsystem::CreateSession(int NumPublicConnections, FString GameMode)
{
	if (!OnCreateSessionCompleteDelegateHandle.IsValid()) {
		OnCreateSessionCompleteDelegateHandle =	OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	}

	// Debug
	DEBUG_MESSAGE(FString(TEXT("Creating a Session")), FColor::Yellow);

	TUniquePtr<FOnlineSessionSettings> SessionSettingsPtr = MakeUnique<FOnlineSessionSettings>();
	SessionSettingsPtr->bShouldAdvertise = true;
	SessionSettingsPtr->bAllowInvites = true;
	SessionSettingsPtr->bAllowJoinInProgress = true;
	SessionSettingsPtr->bUsesPresence = true;
	SessionSettingsPtr->bAllowJoinViaPresence = true;
	SessionSettingsPtr->bAllowJoinViaPresenceFriendsOnly = true;
	SessionSettingsPtr->bIsLANMatch = SubsystemName.IsEqual(FName(TEXT("NULL"))) ? true : false;
	SessionSettingsPtr->bUseLobbiesIfAvailable = true;
	SessionSettingsPtr->bIsDedicated = false;
	SessionSettingsPtr->NumPublicConnections = NumPublicConnections;
	SessionSettingsPtr->Settings.Add(SessionSettingsKeys[ESessionSettings::GameMode], GameMode);

	OnlineSessionPtr->CreateSession(0, CurrentSessionName, *SessionSettingsPtr);
}

void UMultiplayerSessionsSubsystem::StartSession()
{

}

void UMultiplayerSessionsSubsystem::FindSessions(int MaxSearchResults)
{
	// Debug
	DEBUG_MESSAGE(FString(TEXT("Searching sessions from the plugin")), FColor::Yellow);

	if (!OnFindSessionsCompleteDelegateHandle.IsValid()) {
		OnFindSessionsCompleteDelegateHandle = OnlineSessionPtr->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	}

	SessionsSearchSettingsPtr->MaxSearchResults = MaxSearchResults;
	//SessionsSearchSettingsPtr->QuerySettings.Set(SessionSettingsKeys[ESessionSettings::GameMode], FString(TEXT("Ligvest")), EOnlineComparisonOp::Equals);
	SessionsSearchSettingsPtr->bIsLanQuery = false;
	SessionsSearchSettingsPtr->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	OnlineSessionPtr->FindSessions(0, SessionsSearchSettingsPtr.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResults)
{

}

void UMultiplayerSessionsSubsystem::DestroySession()
{

}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Debug
	DEBUG_MESSAGE(FString(TEXT("Session was created")), FColor::Green);
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
	for (const FOnlineSessionSearchResult& SearchResult : SessionsSearchSettingsPtr->SearchResults) {
		FString OwnerName = SearchResult.Session.OwningUserName;
		FString GameMode{};
		//SearchResult.Session.SessionSettings.Get(SessionSettingsKeys[ESessionSettings::GameMode], GameMode);
		// Debug
		DEBUG_MESSAGE(FString::Printf(TEXT("Owner name: %s, Game mode: %s"), *OwnerName, *GameMode), FColor::Yellow);
	}

	//if (!OnlineSessionPtr.IsValid()) {
	//	return;
	//}

	//DEBUG_MESSAGE(FString(TEXT("Session search finished. Found results:")), FColor::Green);
	//for (FOnlineSessionSearchResult& SearchResult : SessionsSearchSettingsPtr->SearchResults) {

	//	if (GEngine) {
	//		GEngine->AddOnScreenDebugMessage(
	//			-1,
	//			5.f,
	//			FColor::Orange,
	//			FString::Printf(TEXT("Found matched session. ID: %s, Owner: %s"), *SearchResult.GetSessionIdStr(), *SearchResult.Session.OwningUserName)
	//		);
	//	}
	//	// Match the correct match type
	//	FName MatchTypeKey = FName(TEXT("GameMode"));
	//	FString MatchTypeValue;
	//	SearchResult.Session.SessionSettings.Get(MatchTypeKey, MatchTypeValue);
	//	if (MatchTypeValue.Equals(TEXT("Ligvest"))) {
	//		OnlineSessionPtr->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	//		if (GEngine) {
	//			GEngine->AddOnScreenDebugMessage(
	//				-1,
	//				5.f,
	//				FColor::Orange,
	//				FString::Printf(TEXT("Found matched session. ID: %s, Owner: %s"), *SearchResult.GetSessionIdStr(), *SearchResult.Session.OwningUserName)
	//			);
	//		}

	//		// Join session
	//		ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//		OnlineSessionPtr->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
	//		if (GEngine) {
	//			GEngine->AddOnScreenDebugMessage(
	//				-1,
	//				5.f,
	//				FColor::Orange,
	//				FString::Printf(TEXT("Joining..."))
	//			);
	//		}
	//	}
	//}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult)
{

}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

}
