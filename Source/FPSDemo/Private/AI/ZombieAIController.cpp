// 版权所有 Epic Games, Inc. 保留所有权利。

#include "AI/ZombieAIController.h"
#include "Character/Zombies/ZombieBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

namespace
{
const FName ZombieAITargetBlackboardKey(TEXT("Target"));
const FName ZombieAIStateBlackboardKey(TEXT("State"));
}

AZombieAIController::AZombieAIController()
{
    // 允许Tick更新
    PrimaryActorTick.bCanEverTick = true;

    // 创建AI感知组件
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

    // 创建并配置视觉感知配置
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.0f;              // 可视范围
    SightConfig->LoseSightRadius = 2500.0f;         // 丢失目标范围
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;  // 周边视野角度
    // 感知 affiliation 设置（可感知敌对、中立单位）
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    // 刺激在感知系统中保留的时长（2秒后自动移除）
    SightConfig->SetMaxAge(2.0f);

    // 应用感知配置
    AIPerceptionComp->ConfigureSense(*SightConfig);
    // 设置主导感知为视觉
    AIPerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());

    // 初始化状态
    TargetPlayer = nullptr;
    bCanPerceiveTarget = false;
    CurrentState = FName("Idle");
    ChaseRange = 2000.0f;
    LoseTargetRange = 2500.0f;
}

void AZombieAIController::BeginPlay()
{
    Super::BeginPlay();

    // 绑定感知更新回调
    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnPerceptionUpdated);
        AIPerceptionComp->OnTargetPerceptionForgotten.AddDynamic(this, &AZombieAIController::OnTargetPerceptionForgotten);
    }
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 初始化出生点位置
    if (InPawn)
    {
        HomeLocation = InPawn->GetActorLocation();
    }

    // 启动行为树
    StartBehaviorTree();

    // 将HomeLocation写入Blackboard
    if (Blackboard)
    {
        Blackboard->SetValueAsVector(FName("HomeLocation"), HomeLocation);
    }

    // 启动定时器：每0.5秒检查一次目标可见性
    GetWorldTimerManager().SetTimer(VisibilityCheckTimer, this, &AZombieAIController::CheckTargetVisibility, 0.5f, true);
}

void AZombieAIController::OnUnPossess()
{
    Super::OnUnPossess();

    // 停止行为树
    StopBehaviorTree();
    // 清理定时器
    GetWorldTimerManager().ClearTimer(VisibilityCheckTimer);
}

/**
 * 启动BehaviorTree
 * 初始化Blackboard并运行行为树
 */
void AZombieAIController::StartBehaviorTree()
{
    if (BlackboardAsset)
    {
        UBlackboardComponent* BlackboardComp = nullptr;
        UseBlackboard(BlackboardAsset, BlackboardComp);
        Blackboard = BlackboardComp;
    }

    if (BehaviorTreeAsset && Blackboard)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

/**
 * 停止BehaviorTree
 * 停止AI逻辑
 */
void AZombieAIController::StopBehaviorTree()
{
    if (BrainComponent)
    {
        BrainComponent->StopLogic(FString("Unpossessed"));
    }
}

/**
 * AI感知更新回调
 * 当AI感知到新的Actor时被调用
 * 从多个感知目标中选择最近的玩家
 */
void AZombieAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    if (bCanPerceiveTarget||TargetPlayer.IsValid())
    {
        return;
    }
    AActor* ClosestActor = nullptr;
    float ClosestDistance = FLT_MAX;

    for (AActor* Actor : UpdatedActors)
    {
        ACharacter* SensedCharacter = Cast<ACharacter>(Actor);
        if (SensedCharacter && SensedCharacter->IsPlayerControlled())
        {
            FActorPerceptionBlueprintInfo PerceptionInfo;
            AIPerceptionComp->GetActorsPerception(Actor, PerceptionInfo);

            bool bIsSensed = false;
            for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
            {
                if (Stimulus.WasSuccessfullySensed())
                {
                    bIsSensed = true;
                    break;
                }
            }

            if (bIsSensed)
            {
                float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
                if (Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestActor = Actor;
                }
            }
        }
    }

    if (ClosestActor)
    {
        SetTargetPlayer(ClosestActor);
        bCanPerceiveTarget = true;

        AZombieBase* Zombie = GetControlledZombie();
        if (Zombie)
        {
            Zombie->SetTargetPlayer(ClosestActor);
        }
    }
}

/**
 * 目标从感知中消失时调用
 * 当感知系统清除对某个目标的追踪时触发
 */
void AZombieAIController::OnTargetPerceptionForgotten(AActor* ForgottenActor)
{
    if (ForgottenActor == TargetPlayer)
    {
        bCanPerceiveTarget = false;
    }
}

/**
 * 定期检查目标是否仍可见
 * 由定时器每0.5秒调用
 */
void AZombieAIController::CheckTargetVisibility()
{
    if (!TargetPlayer.IsValid())
    {
        bCanPerceiveTarget = false;
        return;
    }

    // 计算与目标的距离
    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer.Get()->GetActorLocation());

    // 超过丢失范围则完全丢失目标
    if (Distance > LoseTargetRange)
    {
        OnTargetLost();
        return;
    }

    // 如果没被感知到且距离超过追踪范围，则丢失目标
    if (!bCanPerceiveTarget && Distance > ChaseRange)
    {
        OnTargetLost();
    }
}

/**
 * 目标丢失处理
 * 清除目标引用和可见性状态
 */
void AZombieAIController::OnTargetLost()
{
    SetTargetPlayer(nullptr);
    bCanPerceiveTarget = false;
}

/**
 * 设置目标玩家
 * 同时同步到黑板和僵尸自身
 */
void AZombieAIController::SetTargetPlayer(AActor* NewTarget)
{
    TargetPlayer = NewTarget;

    if (Blackboard)
    {
        if (NewTarget)
        {
            Blackboard->SetValueAsObject(ZombieAITargetBlackboardKey, NewTarget);
        }
        else
        {
            Blackboard->ClearValue(ZombieAITargetBlackboardKey);
        }
    }

    AZombieBase* Zombie = GetControlledZombie();
    if (Zombie)
    {
        Zombie->SetTargetPlayer(NewTarget);
    }
}

/**
 * 设置当前AI状态
 * 同时更新Blackboard
 */
void AZombieAIController::SetCurrentState(FName NewState)
{
    CurrentState = NewState;

    if (Blackboard)
    {
        Blackboard->SetValueAsName(ZombieAIStateBlackboardKey, NewState);
    }
}

/**
 * 追踪目标
 * 使用AI的MoveToActor功能向目标移动
 */
void AZombieAIController::ChaseTarget()
{
    if (!TargetPlayer.IsValid() || !GetPawn())
    {
        return;
    }

    // 向目标移动，保持100单位距离
    MoveToActor(TargetPlayer.Get(), 100.0f, true, true, false, nullptr, true);
}

/**
 * 攻击目标
 * 调用僵尸的PerformAttack
 */
void AZombieAIController::AttackTarget()
{
    AZombieBase* Zombie = GetControlledZombie();
    if (Zombie)
    {
        Zombie->PerformAttack();
    }
}

/**
 * 停止AI移动
 */
void AZombieAIController::StopAIMovement()
{
    StopMovement();
}

/**
 * 获取被控制的僵尸引用
 */
AZombieBase* AZombieAIController::GetControlledZombie() const
{
    return Cast<AZombieBase>(GetPawn());
}
