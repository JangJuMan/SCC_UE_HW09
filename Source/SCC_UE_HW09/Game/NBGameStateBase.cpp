// NBGameStateBase.cpp
#include "NBGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/NBPlayerController.h"

void ANBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (!HasAuthority())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			ANBPlayerController* NBPC = Cast<ANBPlayerController>(PC);
			if (IsValid(NBPC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the Game.");
				NBPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
