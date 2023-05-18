# UE5_OnlineSessionsManagerPlugin

How to use:

Inherit a UserWidget from UMenu.
Add in the widget:
ListView and name it "ListView_Sessions".

For entries in the ListView_Sessions List:
Inherit a UserWidget from UFoundSessionListViewEntry.
Add in the widget:
TextBlock and name it Text_SessionShortDescription
TextBlock and name it Text_SessionIndex

After instantiation of the menu use:
/*
 * Set the menu visible, change input mode etc.
 * Call whenever you want to begin to interact with the menu
 */
UFUNCTION(BlueprintCallable)
void SetupMenu();

Before removing the menu use:
/* 
 * Before you destroy the menu you should call this 
 * To disable visibility, change input mode back etc.
 */
UFUNCTION(BlueprintCallable)
void BeforeRemoval();

To host a session from the menu use:
/* 
 * Create session and move to the lobby
 * which is located here FString(TEXT("/Game/Maps/Lobby?listen"))
 * To Implement: later could be added as parameter of the method
 */ 
UFUNCTION(BlueprintCallable)
void HostSession();

To search existing sessions ( results should show up in the ListView_Sessions ) from the menu use :
/* 
 * SearchSessions calls OnSearchSessionsComplete after the search completed
 * MaxEntriesNumber - Amount of results which could be found. Should be 10 000+
 * Filter - Some rules to specify a game we are looking for. To Implement
 */
UFUNCTION(BlueprintCallable)
void SearchSessions(int MaxEntriesNumber, const FSearchFilter& Filter);

To join a specific game from the menu use it's index from Text_SessionIndex and pass it as a number to the function:
/*
 * Join a session.
 * ID - this is the ID from UFoundSessionListViewEntry::Text_SessionIndex. 
 * This is the index of the session in RecentSearchResults array
 */
UFUNCTION(BlueprintCallable)
void JoinSession(int32 ID);

To disconnect from a game ( your or another ) from the menu use:
/* 
 * For now it only destroys a session  
 * But you should use it in order to create a new session or be able to join another game. 
 * Or even to search for games if you are already in any session
 */
UFUNCTION(BlueprintCallable)
void Disconnect();
