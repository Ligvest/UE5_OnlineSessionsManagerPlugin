// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/ListView.h"
#include "FoundSessionData.h"


// Set the menu visible, change input mode etc.
// Call whenever you want to begin to interact with the menu
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

// Before you destroy the menu you should call this 
// To disable visibility, change input mode back etc.
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

// Override function which is called when current level is destroyed
void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	BeforeRemoval();

	// This function will call RemoveFromParent so we don't have to
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}


/* Create session and move to the lobby
 * which is located here FString(TEXT("/Game/Maps/Lobby?listen"))
 * To Implement: later could be added as parameter of the method
 */ 
void UMenu::HostSession()
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem)
	{
		DEBUG_MESSAGE(FString(TEXT("SessionsSubsystem OK.")), FColor::Green);

		SessionsSubsystem->HostLobby(4, EGameModes::EGM_Default, FString(TEXT("/Game/Maps/Lobby?listen")));
	}
}


/* SearchSessions calls OnSearchSessionsComplete after the search completed
 * MaxEntriesNumber - Amount of results which could be found. Should be 10 000+
 * Filter - Some rules to specify a game we are looking for. To Implement
 */
void UMenu::SearchSessions(int MaxEntriesNumber, const FSearchFilter& Filter)
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem)
	{
		DEBUG_MESSAGE(FString(TEXT("UMenu::SearchSessions")), FColor::Green);

		//FSearchFilter Filter;
		//Filter.GameMode = EGameModes::EGM_Default;

		SessionsSubsystem->FindSessions(MaxEntriesNumber, Filter);
	}
}

void UMenu::JoinSession(int32 ID)
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem) {
		DEBUG_MESSAGE(FString(TEXT("UMenu::JoinSession")), FColor::Green);
		SessionsSubsystem->JoinSession(RecentSearchResults[ID]);
	}
}

void UMenu::Disconnect()
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem) {
		DEBUG_MESSAGE(FString(TEXT("UMenu::Disconnect")), FColor::Green);
		SessionsSubsystem->Disconnect();
	}
}

// Function to work with search results after SearchSessions completed
void UMenu::OnSearchSessionsComplete(TArray<FOnlineSessionSearchResult> SearchResults, bool bWasSuccessful)
{
	DEBUG_MESSAGE(FString(TEXT("UMenu::OnSearchSessionsComplete")), FColor::Green);

	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem) {
		RecentSearchResults = SearchResults;

		for (int32 Index = 0; Index < RecentSearchResults.Num(); ++Index) {

			// Getting info from SearchResult
			FOnlineSessionSearchResult SearchResult = RecentSearchResults[Index];
			FString OwnerName = SearchResult.Session.OwningUserName;
			FString GameMode{};

			const auto& SessionSettingsKeys = SessionsSubsystem->SessionSettingsKeys;
			const auto& GameModesArray = SessionsSubsystem->GameModesArray;

			SearchResult.Session.SessionSettings.Get(SessionSettingsKeys[ESessionSettings::ESS_GameMode], GameMode);

			// Filling UObject data structure to send it to newly created 
			// ListViewItem which will use this info to set its variables
			// And create a new ListViewItem
			UFoundSessionData* SessionData = NewObject<UFoundSessionData>(this, UFoundSessionData::StaticClass());
			SessionData->ShortDescription = FString::Printf(TEXT("Owner name: %s, Game mode: %s"), *OwnerName, *GameMode);
			SessionData->Index = Index;
			SessionData->MenuReference = this;
			if (ListView_Sessions) {
				ListView_Sessions->AddItem(SessionData);
			}
		}
	}
}

// Returns UMultiplayerSessionSubsystem* if success or nullptr otherwise
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
