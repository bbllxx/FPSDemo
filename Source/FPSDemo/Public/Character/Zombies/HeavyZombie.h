// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombies/ZombieBase.h"
#include "HeavyZombie.generated.h"

/**
 * AHeavyZombie - 重型僵尸
 * 血量高、防御强、攻击伤害大但速度慢
 * 适合正面对抗玩家
 */
UCLASS()
class FPSDEMO_API AHeavyZombie : public AZombieBase
{
    GENERATED_BODY()

public:
    AHeavyZombie();

    virtual void BeginPlay() override;

protected:
    // 伤害减免比例（受到伤害时按此比例减少）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy Zombie")
    float DamageReduction;

    /** 重写受伤处理，实现伤害减免 */
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
};
