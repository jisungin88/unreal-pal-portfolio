// Copyright Epic Games, Inc. All Rights Reserved.

#include "Pal_ProjectGameMode.h"
#include "Pal_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

APal_ProjectGameMode::APal_ProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
