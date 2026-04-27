
#pragma once

#include "CoreMinimal.h"
#include "Character/Zombies/ZombieBase.h"
#include "RangedZombie.generated.h"

/**
 * ARangedZombie - 远程僵尸
 * 可以在远距离发射投射物攻击玩家
 * 会保持一定距离，太近会后退
 */
UCLASS()
class FPSDEMO_API ARangedZombie : public AZombieBase
{
    GENERATED_BODY()

public:
    static constexpr int32 ClassId = 3;

    ARangedZombie();

    virtual void BeginPlay() override;
    virtual int32 GetClassId() const override { return ClassId; }

    /** 重写攻击行为：发射投射物而非近身攻击 */
    virtual void PerformAttack() override;

    /** 发射投射物 */
    UFUNCTION(BlueprintCallable, Category = "Ranged Zombie")
    void FireProjectile();

    /** 获取理想的攻击距离 */
    UFUNCTION(BlueprintPure, Category = "Ranged Zombie")
    float GetIdealAttackRange() const { return IdealAttackRange; }

protected:
    // 投射物类（需要在Blueprint或此处设置具体的Actor类）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Zombie")
    TSubclassOf<class AActor> ProjectileClass;

    // 发射位置组件（挂载在骨骼上）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* MuzzleLocation;

    // 理想攻击距离（保持在这个距离外攻击）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Zombie")
    float IdealAttackRange;

    // 投射物速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Zombie")
    float ProjectileSpeed;

    /** 重写：远程僵尸不通过近身造成伤害（伤害由投射物自己造成） */
    virtual void DealDamageToTarget() override;

    /** 检查是否需要靠近目标（距离太远时） */
    UFUNCTION(BlueprintPure, Category = "Ranged Zombie")
    bool ShouldMoveCloser() const;

    /** 检查是否需要远离目标（距离太近时） */
    UFUNCTION(BlueprintPure, Category = "Ranged Zombie")
    bool ShouldMoveAway() const;
};
