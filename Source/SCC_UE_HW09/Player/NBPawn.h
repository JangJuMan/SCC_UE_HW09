// NBPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NBPawn.generated.h"

UCLASS()
class SCC_UE_HW09_API ANBPawn : public APawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
};
