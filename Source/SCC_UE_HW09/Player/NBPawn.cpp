// NBPawn.cpp
#include "NBPawn.h"

#include "SCC_UE_HW09.h"

void ANBPawn::BeginPlay()
{
	Super::BeginPlay();
	FString NetRoleString = SCC_UE_HW09FunctionLibrary::GetRoleString(this);
	FString NetModeString = SCC_UE_HW09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBPawn::BeginPlay() %s [%s]"), *NetModeString, *NetRoleString);
	SCC_UE_HW09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ANBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = SCC_UE_HW09FunctionLibrary::GetRoleString(this);
	FString NetModeString = SCC_UE_HW09FunctionLibrary::GetNetModeString(this);
	FString CombinedString = FString::Printf(TEXT("NBPawn::PossessedBy() %s [%s]"), *NetModeString, *NetRoleString);
	SCC_UE_HW09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}