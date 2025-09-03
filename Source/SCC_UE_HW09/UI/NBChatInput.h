// NBChatInput.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NBChatInput.generated.h"

class UEditableTextBox;
/**
 * 
 */
UCLASS()
class SCC_UE_HW09_API UNBChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
};
