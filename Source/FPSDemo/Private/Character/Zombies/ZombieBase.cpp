#include "Character/Zombies/ZombieBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
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
    ChaseAcceptableRadius = 100.0f;
    PatrolSpeed = 150.0f;
    ChaseSpeed = 300.0f;
    // 初始化为负的冷却时间，确保第一次可以立即攻击
    LastAttackTime = -AttackCooldown;
    bAttackDamagePending = false;

    // 配置角色移动组件
    GetCharacterMovement()->bOrientRotationToMovement = true;  // 朝向移动方向旋转
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);  // 旋转速度
    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;            // 默认使用巡逻速度

    // 设置胶囊体碰撞响应 - 阻挡武器
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AZombieBase::BeginPlay()
{
    Super::BeginPlay();

    // 确保CurrentHealth与MaxHealth同步
    CurrentHealth = MaxHealth;
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
        OnTakeDamageAnim(ActualDamage, HitLocation);

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
    OnDeathAnim();

    // 禁用胶囊体碰撞
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 禁用角色移动
    GetCharacterMovement()->DisableMovement();
    // 3秒后自动销毁Actor
    SetLifeSpan(3.0f);
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

float AZombieBase::GetMoveSpeed(EZombieMoveSpeedMode MoveSpeedMode) const
{
    switch (MoveSpeedMode)
    {
    case EZombieMoveSpeedMode::Chase:
        return ChaseSpeed;
    case EZombieMoveSpeedMode::Patrol:
    default:
        return PatrolSpeed;
    }
}

void AZombieBase::ApplyMoveSpeed(EZombieMoveSpeedMode MoveSpeedMode)
{
    if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
    {
        MovementComponent->MaxWalkSpeed = GetMoveSpeed(MoveSpeedMode);
    }
}

bool AZombieBase::StartAttack()
{
    return TryStartAttack();
}

bool AZombieBase::TryStartAttack()
{
    if (!CanAttackAboutCooldown())
    {
        return false;
    }

    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    if (bAttackDamagePending)
    {
        return false;
    }

    // 更新攻击时间
    LastAttackTime = GetWorld()->GetTimeSeconds();
    bAttackDamagePending = true;
    // 触发攻击事件（播放动画等）
    OnAttackAnim();
    return true;
}

float AZombieBase::CommitAttackDamage()
{
    if (!bAttackDamagePending)
    {
        return 0.0f;
    }

    bAttackDamagePending = false;
    return DealDamageToTarget();
}

/**
 * 对目标造成实际伤害
 * 使用GameplayStatics的ApplyDamage进行伤害传递
 */
float AZombieBase::DealDamageToTarget()
{
    if (!TargetPlayer.IsValid())
    {
        return 0.0f;
    }

    if (!IsTargetInAttackRange())
    {
        return 0.0f;
    }

    // 应用伤害，伤害来源为僵尸自身
    return UGameplayStatics::ApplyDamage(TargetPlayer.Get(), AttackDamage, GetController(), this, nullptr);
}
