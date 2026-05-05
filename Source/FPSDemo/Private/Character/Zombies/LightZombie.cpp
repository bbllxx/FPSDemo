#include "Character/Zombies/LightZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ALightZombie::ALightZombie()
{
    // 设置为轻型僵尸类型
    ZombieType = EZombieType::Light;

    // 轻型僵尸属性：低血量、高速度、高攻速
    MaxHealth = 50.0f;          // 低血量（重型的一半）
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;        // 低攻击伤害
    AttackRange = 100.0f;        // 短攻击范围（近身）
    ChaseAcceptableRadius = AttackRange;
    AttackCooldown = 1.0f;       // 快攻击节奏
    PatrolSpeed = 250.0f;
    ChaseSpeed = 500.0f;

    // 默认使用巡逻速度
    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
    // 较小的胶囊体（更灵活）
    GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);
}

void ALightZombie::BeginPlay()
{
    Super::BeginPlay();
}
