// Copyright Epic Games, Inc. All Rights Reserved.

#include "BaseDefenderGameMode.h"
#include "BaseDefenderCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABaseDefenderGameMode::ABaseDefenderGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
