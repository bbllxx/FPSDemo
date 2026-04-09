// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

/**
 * AZombieAIController - 僵尸AI控制器
 * 负责僵尸的感知、追踪和攻击行为
 * 使用BehaviorTree和Blackboard进行AI决策
 */
UCLASS()
class FPSDEMO_API AZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    AZombieAIController();

    // 游戏开始时调用
    virtual void BeginPlay() override;
    // 控制Pawn时调用（Possess）
    virtual void OnPossess(APawn* InPawn) override;
    // 解除控制时调用（UnPossess）
    virtual void OnUnPossess() override;

    /** 获取当前目标玩家 */
    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    AActor* GetTargetPlayer() const { return TargetPlayer.Get(); }

    /** 设置目标玩家 */
    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void SetTargetPlayer(AActor* NewTarget);

    /** 是否能看见目标 */
    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    bool CanSeeTarget() const { return bCanSeeTarget; }

    /** 获取当前AI状态 */
    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    FName GetCurrentState() const { return CurrentState; }

    /** 设置当前AI状态 */
    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void SetCurrentState(FName NewState);

    /** 追踪目标（向目标移动） */
    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void ChaseTarget();

    /** 攻击目标 */
    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void AttackTarget();

    /** 停止AI移动 */
    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void StopAIMovement();

protected:
    // BehaviorTree资源（用于AI决策）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

    // Blackboard资源（用于存储AI状态数据）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UBlackboardData> BlackboardAsset;

    // AI感知组件（检测玩家）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UAIPerceptionComponent> AIPerceptionComp;

    // 当前追踪的目标玩家
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    TWeakObjectPtr<class AActor> TargetPlayer;

    // 是否能看见目标
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    bool bCanSeeTarget;

    // 当前AI状态（用于Blackboard同步）
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    FName CurrentState;

    // 追踪范围（超过此距离可能丢失目标）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie AI")
    float ChaseRange;

    // 丢失目标范围（超过此距离完全丢失目标）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie AI")
    float LoseTargetRange;

    // 定时器句柄：用于定期检查目标可见性
    FTimerHandle VisibilityCheckTimer;

    /** AI感知更新回调（当感知到新Actor时调用） */
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    /** 定期检查目标是否仍可见 */
    UFUNCTION()
    void CheckTargetVisibility();

    /** 目标丢失时调用 */
    UFUNCTION()
    void OnTargetLost();

    /** 获取被控制的僵尸引用 */
    UFUNCTION()
    class AZombieBase* GetControlledZombie() const;

    /** 启动BehaviorTree */
    UFUNCTION()
    void StartBehaviorTree();

    /** 停止BehaviorTree */
    UFUNCTION()
    void StopBehaviorTree();
};
