// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Patrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Patrol::UBTTask_Patrol()
{
    NodeName = TEXT("Patrol");
    bNotifyTick = true;
    bTickIntervals = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    // 设置黑板 "State" 为 "Patrol"
    if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
    {
        BlackboardComp->SetValueAsName(TEXT("State"), TEXT("Patrol"));
    }

    // 以生成点为中心
    FVector Center = Pawn->GetActorLocation();

    // 在 PatrolRadiusMin 和 PatrolRadiusMax 之间生成随机半径
    float RandomRadius = FMath::RandRange(PatrolRadiusMin, PatrolRadiusMax);

    // 生成随机方向
    float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);
    FVector RandomOffset = FVector(FMath::Cos(RandomAngle) * RandomRadius, FMath::Sin(RandomAngle) * RandomRadius, 0.0f);
    FVector RandomPoint = Center + RandomOffset;

    // 查找可达的导航点
    FNavLocation NavLocation;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(AIController);
    if (NavSys && NavSys->GetRandomReachablePointInRadius(RandomPoint, 100.0f, NavLocation))
    {
        TargetLocation = NavLocation.Location;
        LastValidPatrolLocation = TargetLocation;  // 成功时更新有效位置
    }
    else
    {
        // 如果导航失败，使用上次的有效位置
        if (!LastValidPatrolLocation.IsZero())
        {
            TargetLocation = LastValidPatrolLocation;
            LastValidPatrolLocation = Pawn->GetActorLocation();  // 同时用当前位置更新
        }
        else
        {
            // 没有历史位置且导航失败，直接失败
            TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(FName("HomeLocation"));
        }
    }

    // 向目标位置移动
    AIController->MoveToLocation(TargetLocation);

    // 初始化状态
    bHasReachedTargetLocation = false;
    WaitTimer = 0.0f;

    // 设置Tick间隔
    SetNextTickTime(NodeMemory, TickInterval);

    return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 检查是否检测到目标（黑板 Target 不为 None）
    if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
    {
        UObject* Target = BlackboardComp->GetValueAsObject(TEXT("Target"));
        if (Target != nullptr)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }


    // 获取与玩家的距离，用于动态Tick间隔
    float DistanceToPlayer = FarDistanceThreshold;
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        DistanceToPlayer = FVector::Dist(Pawn->GetActorLocation(), PlayerPawn->GetActorLocation());
    }

    // 根据与玩家的距离动态调整下次Tick间隔
    float CurrentInterval = TickInterval;
    if (DistanceToPlayer > FarDistanceThreshold)
    {
        CurrentInterval = FarTickInterval;
    }
    else if (DistanceToPlayer < NearDistanceThreshold)
    {
        CurrentInterval = NearTickInterval;
    }
    SetNextTickTime(NodeMemory, CurrentInterval);


    // 检查是否到达目标位置,并处理等待时间
    float DistanceToTarget = FVector::Dist(Pawn->GetActorLocation(), TargetLocation);

    if (DistanceToTarget < 100.0f)
    {
        if (!bHasReachedTargetLocation)
        {
            bHasReachedTargetLocation = true;
            WaitTimer = 0.0f;
        }

        WaitTimer += DeltaSeconds;
        if (WaitTimer >= PatrolWaitTime)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
