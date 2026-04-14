// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/FPSDemoGameMode.h"
#include "Character/FPSDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSDemoGameMode::AFPSDemoGameMode()
	: Super()
{
	// 设置默认Pawn类为蓝图中的人物
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
