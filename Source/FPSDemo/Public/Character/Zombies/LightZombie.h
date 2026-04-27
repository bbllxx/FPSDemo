
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
    static constexpr int32 ClassId = 2;

    ALightZombie();

    virtual void BeginPlay() override;
    virtual int32 GetClassId() const override { return ClassId; }
};
