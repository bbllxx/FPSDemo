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
    bool bHasReachedTarget;
    float WaitTimer;
};
