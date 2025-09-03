// NBPlayerController.cpp
#include "NBPlayerController.h"

#include "NBPlayerState.h"
#include "SCC_UE_HW09.h"
#include "Blueprint/UserWidget.h"
#include "Game/NBGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/NBChatInput.h"

ANBPlayerController::ANBPlayerController()
{
	bReplicates = true;
}

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass))
	{
		ChatInputWidgetInstance = CreateWidget<UNBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance))
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass))
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance))
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, NotificationText);
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	if (IsLocalController())
	{
		ANBPlayerState* NBPS = GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			FString CombinedMessageString = NBPS->GetPlayerInfoString() + TEXT(" : ") + InChatMessageString;
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	SCC_UE_HW09FunctionLibrary::MyPrintString(this, InChatMessageString, 20.f);
}

void ANBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ANBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM))
	{
		ANBGameModeBase* NBGM = Cast<ANBGameModeBase>(GM);
		if (IsValid(NBGM))
		{
			NBGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
