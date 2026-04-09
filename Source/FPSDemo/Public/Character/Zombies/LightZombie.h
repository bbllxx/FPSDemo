// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombies/ZombieBase.h"
#include "LightZombie.generated.h"

/**
 * ALightZombie - 轻型僵尸
 * 速度快、血量低、攻击频繁
 * 适合快速包抄玩家
 */
UCLASS()
class FPSDEMO_API ALightZombie : public AZombieBase
{
    GENERATED_BODY()

public:
    ALightZombie();

    virtual void BeginPlay() override;
};
