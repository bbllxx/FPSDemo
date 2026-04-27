#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolLocation.generated.h"

/**
 * 生成巡逻位置并写入黑板的任务。
 * 该节点不负责移动、等待或目标检测，这些职责交给行为树其他节点。
 */
UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_FindPatrolLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FindPatrolLocation();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    FName PatrolLocationKeyName = TEXT("PatrolLocation");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    FName CenterLocationKeyName = TEXT("HomeLocation");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadiusMin = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadiusMax = 1000.0f;
};
