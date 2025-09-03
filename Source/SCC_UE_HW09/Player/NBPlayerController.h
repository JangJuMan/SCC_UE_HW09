﻿// NBPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBChatInput;
/**
 * 
 */
UCLASS()
class SCC_UE_HW09_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANBPlayerController();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNBChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UNBChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};
