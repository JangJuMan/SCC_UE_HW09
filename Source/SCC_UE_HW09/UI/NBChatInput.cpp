// NBChatInput.cpp
#include "NBChatInput.h"

#include "GameplayTagContainer.h"
#include "Components/EditableTextBox.h"
#include "Player/NBPlayerController.h"

void UNBChatInput::NativeConstruct()
{
	Super::NativeConstruct();
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::NativeDestruct()
{
	Super::NativeDestruct();
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController))
		{
			ANBPlayerController* OwningNBPlayerController = Cast<ANBPlayerController>(OwningPlayerController);
			if (IsValid(OwningNBPlayerController))
			{
				OwningNBPlayerController->SetChatMessageString(Text.ToString());
				EditableTextBox_ChatInput->SetText(FText());

				// ADD: 채팅창 자동 포커스
				GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
				{
					FSlateApplication::Get().SetKeyboardFocus(EditableTextBox_ChatInput->TakeWidget());
				});
			}
		}
	}
}
