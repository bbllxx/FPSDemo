// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Zombies/HeavyZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AHeavyZombie::AHeavyZombie()
{
    // 设置为重型僵尸类型
    ZombieType = EZombieType::Heavy;

    // 重型僵尸属性：高血量、低速度、高防御
    MaxHealth = 200.0f;         // 高血量（轻型的4倍）
    CurrentHealth = MaxHealth;
    AttackDamage = 25.0f;       // 高攻击伤害
    AttackRange = 150.0f;       // 中等攻击范围
    AttackCooldown = 2.0f;      // 慢攻击节奏
    DamageReduction = 0.5f;      // 50%伤害减免（实际受到伤害减半）

    // 低速移动
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;  // 比轻型僵尸慢3倍
    // 大型胶囊体
    GetCapsuleComponent()->InitCapsuleSize(50.0f, 120.0f);
}

void AHeavyZombie::BeginPlay()
{
    Super::BeginPlay();
}

/**
 * 重写受伤处理
 * 应用伤害减免公式：实际伤害 = 原始伤害 × (1 - 减免比例)
 * 例如：受到100伤害，实际扣除50点生命值
 */
float AHeavyZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    // 计算减免后的伤害
    float ReducedDamage = DamageAmount * (1.0f - DamageReduction);
    // 调用父类处理（父类会处理扣除生命值等）
    return Super::TakeDamage(ReducedDamage, DamageEvent, EventInstigator, DamageCauser);
}
