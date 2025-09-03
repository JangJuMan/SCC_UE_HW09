// SCC_UE_HW09.h

#pragma once

#include "CoreMinimal.h"

class SCC_UE_HW09FunctionLibrary
{
public:
	static void MyPrintString(const AActor* InWorldContextActor, const FString& InString, float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
	{
		if (IsValid(GEngine) && IsValid(InWorldContextActor))
		{
			if (InWorldContextActor->GetNetMode() == NM_Client || InWorldContextActor->GetNetMode() == NM_ListenServer)	// 클라 혹은 리슨 서버
			{
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
			}
			else  // 데디케이티드 서버
			{
				UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
			}
		}
	}

	static FString GetNetModeString(const AActor* InWorldContextActor)
	{
		FString NetModeString = TEXT("None");

		if (IsValid(InWorldContextActor))
		{
			ENetMode NetMode = InWorldContextActor->GetNetMode();
			if (NetMode == NM_Client)
			{
				NetModeString = TEXT("Client");
			}
			else if (NetMode == NM_Standalone)
			{
				NetModeString = TEXT("Standalone");
			}
			else
			{
				NetModeString = TEXT("Server");
			}
		}
		return NetModeString;
	}

	static FString GetRoleString(const AActor* InWorldContextActor)
	{
		FString RoleString = TEXT("None");
		if (IsValid(InWorldContextActor))
		{
			FString LocalRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InWorldContextActor->GetLocalRole());
			FString RemoteRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InWorldContextActor->GetRemoteRole());
			RoleString = FString::Printf(TEXT("%s / %s"), *LocalRoleString, *RemoteRoleString);
		}
		return RoleString;
	}
};