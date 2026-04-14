// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Zombies/ZombieBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AZombieBase::AZombieBase()
{
    // 允许Tick更新
    PrimaryActorTick.bCanEverTick = true;

    // 默认属性值（子类会覆盖）
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 150.0f;
    AttackCooldown = 1.5f;
    ZombieType = EZombieType::None;
    TargetPlayer = nullptr;
    // 初始化为负的冷却时间，确保第一次可以立即攻击
    LastAttackTime = -AttackCooldown;

    // 创建并配置感知组件 - 用于检测玩家
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SetPeripheralVisionAngle(120.0f);         // 视野角度120度
    PawnSensingComp->SightRadius = 2000.0f;                   // 视野半径2000
    PawnSensingComp->HearingThreshold = 1000.0f;              // 听觉阈值
    PawnSensingComp->LOSHearingThreshold = 1500.0f;           // 视线外听觉阈值
    PawnSensingComp->bSeePawns = true;                        // 启用视觉检测
    PawnSensingComp->bHearNoises = true;                      // 启用听觉检测

    // 配置角色移动组件
    GetCharacterMovement()->bOrientRotationToMovement = true;  // 朝向移动方向旋转
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);  // 旋转速度
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;            // 默认行走速度

    // 设置胶囊体碰撞响应 - 阻挡武器
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AZombieBase::BeginPlay()
{
    Super::BeginPlay();

    // 确保CurrentHealth与MaxHealth同步
    CurrentHealth = MaxHealth;

    // 绑定感知组件的回调函数
    if (PawnSensingComp)
    {
        // 当看到Pawn时触发（看到玩家）
        PawnSensingComp->OnSeePawn.AddDynamic(this, &AZombieBase::OnSeePawn);
        // 当听到声音时触发
        PawnSensingComp->OnHearNoise.AddDynamic(this, &AZombieBase::OnHearNoise);
    }
}

void AZombieBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AZombieBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/**
 * 受到伤害处理
 * 实现伤害的接收、扣除生命值、判断死亡
 */
float AZombieBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    // 调用父类处理获取实际伤害值
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        // 扣除生命值
        CurrentHealth -= ActualDamage;

        // 获取受击位置，默认为僵尸当前位置
        FVector HitLocation = GetActorLocation();
        // 如果是点伤害事件，使用精确的受击点
        if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
        {
            FPointDamageEvent* PointDamage = (FPointDamageEvent*)&DamageEvent;
            HitLocation = PointDamage->HitInfo.ImpactPoint;
        }

        // 触发Blueprint中的受伤事件（播放受击特效等）
        OnTakeDamage(ActualDamage, HitLocation);

        // 检查是否死亡
        if (CurrentHealth <= 0.0f)
        {
            OnDeath();
        }
    }

    return ActualDamage;
}

/**
 * 僵尸死亡处理
 * 禁用碰撞、停止移动、延迟销毁
 */
void AZombieBase::OnDeath()
{
    // 触发Blueprint中的死亡事件（播放死亡特效等）
    OnDeathEvent();

    // 禁用胶囊体碰撞
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 禁用角色移动
    GetCharacterMovement()->DisableMovement();
    // 3秒后自动销毁Actor
    SetLifeSpan(3.0f);
}

/**
 * 感知组件回调 - 当看到Pawn时调用
 * 仅响应玩家控制的Pawn
 */
void AZombieBase::OnSeePawn(APawn* Pawn)
{
    if (Pawn && Pawn->IsPlayerControlled())
    {
        TargetPlayer = Pawn;
    }
}

/**
 * 感知组件回调 - 当听到声音时调用
 * 仅在还没有目标时响应玩家声音
 */
void AZombieBase::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
    // 只有在没有目标时才响应声音（声音会让僵尸发现玩家）
    if (!TargetPlayer.IsValid() && PawnInstigator && PawnInstigator->IsPlayerControlled())
    {
        TargetPlayer = PawnInstigator;
    }
}

/**
 * 检查是否可以攻击
 * 基于冷却时间判断
 */
bool AZombieBase::CanAttackAboutCooldown() const
{
    // 当前时间 - 上次攻击时间 >= 冷却时间
    return (GetWorld()->GetTimeSeconds() - LastAttackTime) >= AttackCooldown;
}

/**
 * 检查目标是否在攻击范围内
 */
bool AZombieBase::IsTargetInAttackRange() const
{
    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    // 计算与目标的距离
    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer.Get()->GetActorLocation());
    return Distance <= AttackRange;
}

/**
 * 执行攻击
 * 检查冷却和攻击范围，然后造成伤害
 */
void AZombieBase::PerformAttack()
{
    // 检查冷却
    if (!CanAttackAboutCooldown())
    {
        return;
    }

    // 更新攻击时间
    LastAttackTime = GetWorld()->GetTimeSeconds();
    // 触发攻击事件（播放动画等）
    OnAttack();
    // 对目标造成伤害
    DealDamageToTarget();
}

/**
 * 对目标造成实际伤害
 * 使用GameplayStatics的ApplyDamage进行伤害传递
 */
void AZombieBase::DealDamageToTarget()
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    if (!IsTargetInAttackRange())
    {
        return;
    }

    // 应用伤害，伤害来源为僵尸自身
    UGameplayStatics::ApplyDamage(TargetPlayer.Get(), AttackDamage, GetController(), this, nullptr);
}
