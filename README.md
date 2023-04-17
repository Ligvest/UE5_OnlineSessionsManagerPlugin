# UE5_OnlineSessionsManagerPlugin
// A Plugin to wrap work with online sessions and to expose only high-level interfaces

// Derived from UGameInstanceSubsystem

// Interface
public:
	void CreateSession(int NumPublicConnections, FString GameMode);
	void StartSession();
	void FindSessions(int MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SearchResults);
	void DestroySession();
	
// To get access to the interface:
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		UMultiplayerSessionsSubsystem* SessionsSubsystemHelper = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	SessionsSubsystemHelper->CreateSession(4, FString(TEXT("MyGameMode")));
