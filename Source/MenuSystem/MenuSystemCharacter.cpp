// Copyright Epic Games, Inc. All Rights Reserved.

#include "MenuSystemCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"

//////////////////////////////////////////////////////////////////////////
// AMenuSystemCharacter

AMenuSystemCharacter::AMenuSystemCharacter() :
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem) {
		//OnlineSubsystem->GetOnlineServiceName();
		OnlineSessionPtr = OnlineSubsystem->GetSessionInterface();
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Found subsystem is %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}


	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMenuSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMenuSystemCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMenuSystemCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMenuSystemCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMenuSystemCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMenuSystemCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMenuSystemCharacter::TouchStopped);
}

void AMenuSystemCharacter::CreateGameSession()
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}

	//UGameInstance* GameInstance = GetGameInstance();
	//if (GameInstance) {
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		5.f,
	//		FColor::Blue,
	//		FString::Printf(TEXT("GameInstance OK."))
	//	);

	//	UMultiplayerSessionsSubsystem* SessionsSubsystemHelper = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	//	if (SessionsSubsystemHelper)
	//	{
	//		GEngine->AddOnScreenDebugMessage(
	//			-1,
	//			5.f,
	//			FColor::Blue,
	//			FString::Printf(TEXT("SessionsSubsystem OK."))
	//		);
	//		SessionsSubsystemHelper->CreateSession(4, TEXT("Ligvest"));
	//	}
	//}

	//return;




	FNamedOnlineSession* ExistingSession = OnlineSessionPtr->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {
		//OnlineSessionInterfacePtr->DestroySession(NAME_GameSession);
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Blue,
			FString::Printf(TEXT("Existing session was destroyed"))
		);
	}

	// Clear from previous delegates to prevent many copies of one delegate be executed
	//FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	//OnlineSessionInterfacePtr->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);


	// Configure the session
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettingsPtr = MakeShareable(new FOnlineSessionSettings);
	OnlineSessionSettingsPtr->bIsLANMatch = false;
	OnlineSessionSettingsPtr->NumPublicConnections = 4;
	OnlineSessionSettingsPtr->bAllowJoinInProgress = true;
	OnlineSessionSettingsPtr->bShouldAdvertise = true;
	OnlineSessionSettingsPtr->bAllowJoinViaPresence = true;
	OnlineSessionSettingsPtr->bUsesPresence = true;
	OnlineSessionSettingsPtr->bUseLobbiesIfAvailable = true;

	FName MatchTypeKey = FName(TEXT("MatchType"));
	FString MatchTypeValue = FString(TEXT("DefaultLigvest"));
	OnlineSessionSettingsPtr->Set(MatchTypeKey, MatchTypeValue, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionPtr->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *OnlineSessionSettingsPtr);
}

void AMenuSystemCharacter::JoinGameSession()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		UMultiplayerSessionsSubsystem* SessionsSubsystemHelper = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (SessionsSubsystemHelper)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Blue,
				FString::Printf(TEXT("SessionsSubsystem OK."))
			);
			SessionsSubsystemHelper->FindSessions(10000);
		}
	}
}

void AMenuSystemCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Green,
				FString::Printf(TEXT("Online Session has been successfuly created. Name = %s"), *SessionName.ToString())
			);
		}

		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				FString::Printf(TEXT("Online Session couldn't be created"))
			);
		}
	}

}

void AMenuSystemCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{

	//if (!OnlineSessionInterfacePtr.IsValid()) {
	//	return;
	// }
 //
	//	if (GEngine) {
	//		GEngine->AddOnScreenDebugMessage(
	//			-1,
	//			5.f,
	//			FColor::Orange,
	//			FString::Printf(TEXT("Search completed."))
	//		);
	//	}

	//for (FOnlineSessionSearchResult& SearchResult : SessionSearchSettingsPtr->SearchResults) {
	//	
	//	//Info about found sessions
	//	if (GEngine) {
	//		GEngine->AddOnScreenDebugMessage(
	//			-1,
	//			5.f,
	//			FColor::Orange,
	//			FString::Printf(TEXT("Found session. ID: %s, Owner: %s"), *SearchResult.GetSessionIdStr(), *SearchResult.Session.OwningUserName)
	//		);
	//	}

	//	// Match the correct match type
	//	FName MatchTypeKey = FName(TEXT("MatchType"));
	//	FString MatchTypeValue;
	//	SearchResult.Session.SessionSettings.Get(MatchTypeKey, MatchTypeValue);
	//	if (MatchTypeValue.Equals(TEXT("DefaultLigvest"))) {
	//		OnlineSessionInterfacePtr->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	//	if (GEngine) {
	//		GEngine->AddOnScreenDebugMessage(
	//			-1,
	//			5.f,
	//			FColor::Orange,
	//			FString::Printf(TEXT("Found matched session. ID: %s, Owner: %s"), *SearchResult.GetSessionIdStr(), *SearchResult.Session.OwningUserName)
	//		);
	//	}

	//		// Join session
	//		ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//		//OnlineSessionInterfacePtr->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
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
}

void AMenuSystemCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult)
{
	if (!OnlineSessionPtr.IsValid()) {
		return;
	}
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Orange,
				FString::Printf(TEXT("Joined a session. SessionName = %s"), *SessionName.ToString())
			);
		}

	FString JoinedSessionAddress;
	OnlineSessionPtr->GetResolvedConnectString(NAME_GameSession, JoinedSessionAddress);

	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (PC) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Orange,
				FString::Printf(TEXT("Trying to connect..."))
			);
		}

		PC->ClientTravel(JoinedSessionAddress, ETravelType::TRAVEL_Absolute);
	}
}

void AMenuSystemCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMenuSystemCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMenuSystemCharacter::TurnAtRate(float Rate)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *MyMessage);
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMenuSystemCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMenuSystemCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMenuSystemCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
