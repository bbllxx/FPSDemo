// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Patrol.generated.h"

UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_Patrol : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Patrol();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadiusMin = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadiusMax = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolWaitTime = 2.0f;

private:
    FVector TargetLocation;
    FVector LastValidPatrolLocation;  // 最后一个有效的巡逻位置
    bool bHasReachedTargetLocation;
    float WaitTimer;
    float TickInterval = 0.3f;  // 默认Tick间隔时间

    // 动态Tick间隔的距离阈值
    float FarDistanceThreshold = 1000.0f;   // 超过此距离使用远距离间隔
    float NearDistanceThreshold = 500.0f;    // 低于此距离使用近距离间隔
    float FarTickInterval = 0.5f;            // 远距离Tick间隔
    float NearTickInterval = 0.2f;           // 近距离Tick间隔
};
