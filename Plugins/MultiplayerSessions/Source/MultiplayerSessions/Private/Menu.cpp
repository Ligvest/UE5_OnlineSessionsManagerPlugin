// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

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

void UMenu::CreateGameSession()
{
	UMultiplayerSessionsSubsystem* SessionsSubsystem = GetSessionsSubsystem();
	if (SessionsSubsystem)
	{
		// Debug
		DEBUG_MESSAGE(FString(TEXT("SessionsSubsystem OK.")), FColor::Green);

		SessionsSubsystem->HostLobby(4, EGameModes::EGM_Default, FString(TEXT("/Game/Maps/Lobby?listen")));
	}
}


void UMenu::FindSessions(int MaxEntriesNumber, const FSearchFilter& Filter)
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



