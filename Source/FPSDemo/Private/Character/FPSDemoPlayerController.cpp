#include "Character/FPSDemoPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

void AFPSDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}

		if (WeaponInputMappingContext)
		{
			// 武器映射使用更高优先级，便于覆盖默认输入中冲突的按键。
			Subsystem->AddMappingContext(WeaponInputMappingContext, 1);
		}
	}
}
