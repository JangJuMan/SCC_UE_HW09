// NBGameModeBase.cpp
#include "NBGameModeBase.h"

#include "EngineUtils.h"
#include "NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPlayerController))
	{
		NBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the Game Server."));
		AllPlayerControllers.Add(NBPlayerController);

		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			NBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase))
		{
			NBGameStateBase->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}

	// ADD : 첫 번째 플레이어 접속시 최초 턴 시작
	if (AllPlayerControllers.Num() == 1)
	{
		BeginTurn(NBPlayerController);
	}
}

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString = GenerateSecretNumber();
}

FString ANBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });	// TODO: 의미가 있나 이 코드가?

	FString Result;
	for (int32 i = 1; i <= 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	UE_LOG(LogTemp, Warning, TEXT("Secret Number : %s"), *Result);
	return Result;
}

bool ANBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do
	{
		if (InNumberString.Len() != 3)
		{
			break;
		}
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (!FChar::IsDigit(C) || C == '0')
			{
				bIsUnique = false;
				break;
			}
			UniqueDigits.Add(C);
		}

		if (!bIsUnique)
		{
			break;
		}
		bCanPlay = true;
	}while (false);	// early return 대신 scope를 break로 탈출하기 위함
	
	return bCanPlay;
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			++StrikeCount;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				++BallCount;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}
	return FString::Printf(TEXT("%dS %dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::PrintChatMessageString(ANBPlayerController* InChattingPlayerController,
	const FString& InChatMessageString)
{
	FString ChatMessageString = InChatMessageString;
	int32 Index = InChatMessageString.Len() - 3;

	FString GuessNumberString = InChatMessageString.RightChop(Index);	// 오른쪽부터 Index 개수만큼 제거해서 새로운 문자열 반환(왼쪽 3개만 남도록)
	if (IsGuessNumberString(GuessNumberString))
	{
		// 새 게임 대기시간에는 정답 외치기 불가
		if (!bIsGamePlaying)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("새 게임 시작을 기다리는 중입니다."));
			return;
		}
		
		// 자신의 턴이 아니면 정답 외치기 불가
		if (InChattingPlayerController != CurrentTurnPlayer)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("지금은 당신의 턴이 아닙니다."));
			return;
		}

		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) && NBPS->TimeRemainingForTurn <= 0)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("턴 시간이 모두 소진되었습니다."));
			return;
		}

		
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);	// OUT, 1S2B ...
		// EndTurn();	// 정답 1회 외치면 턴 종료, 여기서 GuessCount 증가시키기
		IncreaseGuessCount(InChattingPlayerController);

		// ADD : 업데이트된 PlayerInfoString으로 변경(GuessCount)
		// ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		FString PlayerInfoString = TEXT("");
		if (IsValid(NBPS))
		{
			PlayerInfoString = NBPS->GetPlayerInfoString();
			PlayerInfoString = PlayerInfoString + InChatMessageString.RightChop(PlayerInfoString.Len());
		}

		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPlayerController = *It;
			if (IsValid(NBPlayerController))
			{
				// FString CombinedMessageString = InChatMessageString + TEXT(" → ") + JudgeResultString;
				FString CombinedMessageString = PlayerInfoString + TEXT(" → ") + JudgeResultString;	// ADD : 업데이트된 내용 출력
				NBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(NBPlayerController, StrikeCount);
			}
		}
		EndTurn();
	}
	else
	{
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPlayerController = *It;
			if (IsValid(NBPlayerController))
			{
				NBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChattingPlayerController)
{
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS))
	{
		NBPS->CurrentGuessCount++;
	}
}

void ANBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	bIsGameOver = false;
	bIsGamePlaying = true;

	for (const auto& NBPlayerController : AllPlayerControllers)
	{
		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			NBPS->CurrentGuessCount = 0;
		}
	}
}

void ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			if (IsValid(NBPS))
			{
				FString CombinedMessageString = NBPS->PlayerNameString + TEXT(" has won the Game.\n잠시 후 새 게임이 시작됩니다.");
				NBPlayerController->NotificationText = FText::FromString(CombinedMessageString);
				bIsGameOver = true;
				bIsGamePlaying = false;
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS))
			{
				if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (bIsDraw)
		{
			for (const auto& NBPlayerController : AllPlayerControllers)
			{
				NBPlayerController->NotificationText = FText::FromString(TEXT("Draw...\n잠시 후 새 게임이 시작됩니다."));
				bIsGameOver = true;
				bIsGamePlaying = false;
			}
		}
	}
}

void ANBGameModeBase::BeginTurn(ANBPlayerController* NewTurnPlayer)
{
	if (IsValid(NewTurnPlayer))
	{
		CurrentTurnPlayer = NewTurnPlayer;
		ANBPlayerState* NBPS = NewTurnPlayer->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			NBPS->TimeRemainingForTurn = TurnTimeLimit;
		}
		
		GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ThisClass::TickTurnTimer, 1, true);

		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPlayerController = *It;
			if (IsValid(NBPlayerController))
			{
				FString TurnMessage = FString::Printf(TEXT("%s의 턴입니다.\n%d"), *NBPS->PlayerNameString, NBPS->TimeRemainingForTurn);
				NBPlayerController->NotificationText = FText::FromString(TurnMessage);
			}
		}
	}
}

void ANBGameModeBase::EndTurn()
{
	// IncreaseGuessCount(CurrentTurnPlayer);
	
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);

	int32 CurrentIndex = AllPlayerControllers.Find(CurrentTurnPlayer);
	int32 NextIndex = (CurrentIndex + 1) % AllPlayerControllers.Num();

	UE_LOG(LogTemp, Warning, TEXT("bIsGameOver : %d"), bIsGameOver);
	if (bIsGameOver)
	{
		GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, FTimerDelegate::CreateLambda([this, NextIndex]()
	   {
			ResetGame();
		   if (AllPlayerControllers.IsValidIndex(NextIndex))
		   {
			   BeginTurn(AllPlayerControllers[NextIndex]);
		   }
	   }), 5.0f, false);
	}
	else
	{
		BeginTurn(AllPlayerControllers[NextIndex]);
	}
}

void ANBGameModeBase::TickTurnTimer()
{
	if (IsValid(CurrentTurnPlayer))
	{
		ANBPlayerState* NBPS = CurrentTurnPlayer->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS))
		{
			NBPS->TimeRemainingForTurn--;

			for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
			{
				ANBPlayerController* NBPlayerController = *It;
				if (IsValid(NBPlayerController))
				{
					FString TurnMessage = FString::Printf(TEXT("%s의 턴입니다.\n%d"), *NBPS->PlayerNameString, NBPS->TimeRemainingForTurn);
					NBPlayerController->NotificationText = FText::FromString(TurnMessage);
				}
			}
			
			if (NBPS->TimeRemainingForTurn <= 0)
			{
				JudgeGame(CurrentTurnPlayer, 0);
				IncreaseGuessCount(CurrentTurnPlayer);
				EndTurn();
			}
		}
	}
}
