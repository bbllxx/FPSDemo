#include "Character/Zombies/RangedZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARangedZombie::ARangedZombie()
{
    // 设置为远程僵尸类型
    ZombieType = EZombieType::Ranged;

    // 远程僵尸属性：中等血量、中等速度、远程攻击
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 15.0f;        // 中等伤害（投射物命中时）
    AttackRange = 2000.0f;        // 极大攻击范围（可远程射击）
    AttackCooldown = 2.0f;        // 中等攻击间隔

    // 中等移动速度
    GetCharacterMovement()->MaxWalkSpeed = 350.0f;

    // 创建并配置发射位置组件（挂载在Mesh上）
    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(GetMesh());
    // 相对于Mesh的偏移（向前50单位）
    MuzzleLocation->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

    // 理想攻击距离：1500（太远会靠近，太近会后退）
    IdealAttackRange = 1500.0f;
    ChaseAcceptableRadius = IdealAttackRange;
    // 投射物速度
    ProjectileSpeed = 2000.0f;
}

void ARangedZombie::BeginPlay()
{
    Super::BeginPlay();
}

/**
 * 重写攻击行为
 * 冷却检查后发射投射物而非近身攻击
 */
void ARangedZombie::PerformAttack()
{
    if (!CanAttackAboutCooldown())
    {
        return;
    }

    LastAttackTime = GetWorld()->GetTimeSeconds();
    OnAttackAnim();              // 触发攻击事件（播放动画等）
    FireProjectile();        // 发射投射物
}

/**
 * 发射投射物
 * 在发射位置创建投射物Actor，并设置其朝向和速度
 */
void ARangedZombie::FireProjectile()
{
    // 检查是否设置了投射物类
    if (!ProjectileClass)
    {
        return;
    }

    // 检查目标是否有效
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    // 计算发射位置（优先使用MuzzleLocation，否则使用身体前方）
    FVector SpawnLocation = MuzzleLocation ? MuzzleLocation->GetComponentLocation() : GetActorLocation() + GetActorForwardVector() * 50.0f;

    // 计算目标位置和发射方向
    FVector TargetLocation = TargetPlayer.Get()->GetActorLocation();
    FVector Direction = TargetLocation - SpawnLocation;
    Direction.Normalize();
    FRotator SpawnRotation = Direction.Rotation();

    // 设置生成参数
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;                                          // 发射者是僵尸自身
    SpawnParams.Instigator = GetInstigator();                           // 肇事者
    // 忽略碰撞检测，始终生成
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 生成投射物
    AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    // 配置投射物移动组件
    if (Projectile)
    {
        UProjectileMovementComponent* ProjMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>();
        if (ProjMovement)
        {
            ProjMovement->InitialSpeed = ProjectileSpeed;
            ProjMovement->MaxSpeed = ProjectileSpeed;
            ProjMovement->Velocity = Direction * ProjectileSpeed;
        }
    }
}

/**
 * 重写伤害处理
 * 远程僵尸的伤害由投射物自己造成，这里为空
 */
float ARangedZombie::DealDamageToTarget()
{
    // 远程僵尸不通过此方法造成伤害
    // 伤害由投射物碰撞时自己应用
    return 0.0f;
}

/**
 * 检查是否需要靠近目标
 * 当距离大于IdealAttackRange+200时返回true
 */
bool ARangedZombie::ShouldMoveCloser() const
{
    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer.Get()->GetActorLocation());
    // 距离超过 IdealAttackRange + 200 时需要靠近
    return Distance > IdealAttackRange + 200.0f;
}

/**
 * 检查是否需要远离目标
 * 当距离小于IdealAttackRange-200时返回true
 */
bool ARangedZombie::ShouldMoveAway() const
{
    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer.Get()->GetActorLocation());
    // 距离小于 IdealAttackRange - 200 时需要后退
    return Distance < IdealAttackRange - 200.0f;
}
