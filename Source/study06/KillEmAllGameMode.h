// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleShooterGameModeBase.h"
#include "KillEmAllGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STUDY06_API AKillEmAllGameMode : public AGameModeBase /*ASimpleShooterGameModeBase*/
{
	GENERATED_BODY()

public:
	//virtual void PawnKilled(APawn* PawnKilled) override;
	void PawnKilled(APawn* PawnKilled);

private:
	void EndGame(bool bIsPlayerWinner);

};
