
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Idle.generated.h"

UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_Idle : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Idle();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
