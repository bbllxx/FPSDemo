
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

// 僵尸类型枚举，区分不同僵尸种类
UENUM(BlueprintType)
enum class EZombieType : uint8
{
    Heavy     UMETA(DisplayName = "重型僵尸"),   // 高血量、高攻击、缓慢
    Light     UMETA(DisplayName = "轻型僵尸"),   // 低血量、快速、攻击频繁
    Ranged    UMETA(DisplayName = "远程僵尸"),  // 远程攻击能力
    None      UMETA(DisplayName = "无")
};

/**
 * AZombieBase - 僵尸基类
 * 所有僵尸类型的父类，继承自ACharacter
 * 提供通用的僵尸行为：感知玩家、攻击、受伤、死亡
 */
UCLASS(Abstract)
class FPSDEMO_API AZombieBase : public ACharacter
{
    GENERATED_BODY()

public:
    static constexpr int32 ClassId = 0;

    AZombieBase();

    // 游戏开始时调用，初始化组件和属性
    virtual void BeginPlay() override;
    // 每帧更新
    virtual void Tick(float DeltaTime) override;
    // 设置输入组件
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /**
     * 受到伤害时调用
     * @param DamageAmount 伤害值
     * @param DamageEvent 伤害事件信息
     * @param EventInstigator 造成伤害的控制器
     * @param DamageCauser 造成伤害的Actor
     */
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    /** 兼容旧入口，内部转调 TryStartAttack */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    virtual bool StartAttack();

    /** 尝试发起攻击；不判断攻击距离，命中距离由动画通知结算 */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    virtual bool TryStartAttack();

    /** 由攻击动画通知调用，结算本次攻击伤害 */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    virtual float CommitAttackDamage();

    /** 检查是否可以攻击（冷却是否结束） */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    bool CanAttackAboutCooldown() const;

    /** 检查目标是否在攻击范围内 */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    bool IsTargetInAttackRange() const;

    /** 获取当前生命值 */
    UFUNCTION(BlueprintPure, Category = "Zombie")
    float GetCurrentHealth() const { return CurrentHealth; }

    /** 获取当前目标玩家 */
    UFUNCTION(BlueprintPure, Category = "Zombie")
    AActor* GetTargetPlayer() const { return TargetPlayer.Get(); }

    /** 设置目标玩家 */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    void SetTargetPlayer(AActor* NewTarget) { TargetPlayer = NewTarget; }

    /** 获取僵尸类别编号 */
    UFUNCTION(BlueprintPure, Category = "Zombie")
    virtual int32 GetClassId() const { return ClassId; }

    /** 获取追击目标时允许停止的距离 */
    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    float GetChaseAcceptableRadius() const { return ChaseAcceptableRadius; }

protected:
    // 僵尸类型
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    EZombieType ZombieType;

    // 最大生命值
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float MaxHealth;

    // 当前生命值
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float CurrentHealth;

    // 攻击伤害值
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float AttackDamage;

    // 攻击范围（距离）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float AttackRange;

    // 攻击冷却时间（秒）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float AttackCooldown;

    // 当前追踪的目标玩家（弱引用，避免循环引用）
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    TWeakObjectPtr<AActor> TargetPlayer;

    // 追击目标时允许停止的距离，由不同僵尸类型自行配置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie AI")
    float ChaseAcceptableRadius;

    // 上次攻击时间（用于计算冷却）
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    float LastAttackTime;

    // 是否有一次已经发起但尚未由动画通知结算的攻击
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    bool bAttackDamagePending;

    /** 僵尸死亡时调用 */
    UFUNCTION()
    virtual void OnDeath();

    /** 对目标造成实际伤害 */
    UFUNCTION(BlueprintCallable, Category = "Zombie")
    virtual float DealDamageToTarget();

    /** 攻击事件（Blueprint实现，用于播放攻击动画等） */
    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie")
    void OnAttackAnim();

    /** 受伤事件（Blueprint实现，用于播放受伤特效等） */
    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie")
    void OnTakeDamageAnim(float DamageAmount, FVector HitLocation);

    /** 死亡事件（Blueprint实现，用于播放死亡特效等） */
    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie")
    void OnDeathAnim();
};
