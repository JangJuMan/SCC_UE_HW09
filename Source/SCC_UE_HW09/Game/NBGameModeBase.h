// NBGameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;
/**
 * 
 */
UCLASS()
class SCC_UE_HW09_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void BeginPlay() override;

	FString GenerateSecretNumber();
	bool IsGuessNumberString(const FString& InNumberString);
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	void PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	void IncreaseGuessCount(ANBPlayerController* InChattingPlayerController);
	void ResetGame();
	void JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount);

	// ADD : 턴제 게임
	void BeginTurn(ANBPlayerController* NewTurnPlayer);
	void EndTurn();
	void TickTurnTimer();

protected:
	FString SecretNumberString;
	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;

	// ADD : 턴 제어, 시간 관리
	TObjectPtr<ANBPlayerController> CurrentTurnPlayer;
	FTimerHandle TurnTimerHandle;
	int32 TurnTimeLimit = 15;

	bool bIsGameOver = false;
	bool bIsGamePlaying = true;
};
