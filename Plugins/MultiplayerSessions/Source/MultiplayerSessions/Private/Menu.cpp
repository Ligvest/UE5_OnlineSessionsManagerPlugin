// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/ListView.h"
#include "FoundSessionData.h"

void UMenu::SetupMenu()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC) {
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(true);
		}
	}

	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem) {
		SessionsSubsystem->OnFindSessionsResultReadyDelegate.AddUObject(this, &ThisClass::OnSearchSessionsComplete);
	}
}

void UMenu::BeforeRemoval()
{
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC) {
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(false);
		}
	}
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	BeforeRemoval();

	// This function will call RemoveFromParent so we don't have to
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}


UMultiplayerSessionsSubsystem* UMenu::GetSessionsSubsystem()
{
	// Check GetWorld() as GetGameInstance() function use this call too but doesn't check if GetWorld() returns nullptr
	if (GetWorld()) {
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance) {
			return GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		}
	}
	return nullptr;

}

void UMenu::HostSession()
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem)
	{
		// Debug
		DEBUG_MESSAGE(FString(TEXT("SessionsSubsystem OK.")), FColor::Green);

		SessionsSubsystem->HostLobby(4, EGameModes::EGM_Default, FString(TEXT("/Game/Maps/Lobby?listen")));
	}
}


void UMenu::SearchSessions(int MaxEntriesNumber, const FSearchFilter& Filter)
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem)
	{
		// Debug
		DEBUG_MESSAGE(FString(TEXT("SessionsSubsystem OK.")), FColor::Green);

		//FSearchFilter Filter;
		//Filter.GameMode = EGameModes::EGM_Default;

		SessionsSubsystem->FindSessions(MaxEntriesNumber, Filter);
	}


}

void UMenu::JoinSession(int32 ID)
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem) {
		// Debug
		DEBUG_MESSAGE(FString(TEXT("UMenu::JoinSession")), FColor::Green);
		SessionsSubsystem->JoinSession(RecentSearchResults[ID]);
	}
}

void UMenu::OnSearchSessionsComplete(TArray<FOnlineSessionSearchResult> SearchResults, bool bWasSuccessful)
{
	// Debug
	DEBUG_MESSAGE(FString(TEXT("UMenu::OnSearchSessionsComplete")), FColor::Green);

	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem) {
		RecentSearchResults = SearchResults;
		//for (const FOnlineSessionSearchResult& SearchResult : SearchResults) {
		for (int32 Index = 0; Index < RecentSearchResults.Num(); ++Index) {

			// Getting info from SearchResult
			FOnlineSessionSearchResult SearchResult = RecentSearchResults[Index];
			FString OwnerName = SearchResult.Session.OwningUserName;
			FString GameMode{};

			const auto& SessionSettingsKeys = SessionsSubsystem->SessionSettingsKeys;
			const auto& GameModesArray = SessionsSubsystem->GameModesArray;

			SearchResult.Session.SessionSettings.Get(SessionSettingsKeys[ESessionSettings::ESS_GameMode], GameMode);

			// Filling Listview Item
			UFoundSessionData* SessionData = NewObject<UFoundSessionData>(this, UFoundSessionData::StaticClass());
			SessionData->ShortDescription = FString::Printf(TEXT("Owner name: %s, Game mode: %s"), *OwnerName, *GameMode);
			SessionData->Index = Index;
			SessionData->MenuReference = this;
			if (ListView_Sessions) {
				ListView_Sessions->AddItem(SessionData);
			}

			if (GameMode.Equals(GameModesArray[EGameModes::EGM_Default])) {
			}
		}
	}
}

