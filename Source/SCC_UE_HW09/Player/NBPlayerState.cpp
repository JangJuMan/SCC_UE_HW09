// NBPlayerState.cpp
#include "NBPlayerState.h"

#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState()
	: PlayerNameString(TEXT("NONE"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
	DOREPLIFETIME(ThisClass, TimeRemainingForTurn);	// ADD : 턴당 시간관리
}

FString ANBPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT(" / ") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
