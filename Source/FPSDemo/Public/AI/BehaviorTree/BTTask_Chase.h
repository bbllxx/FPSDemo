
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Chase.generated.h"

UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_Chase : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Chase();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase")
    float TickInterval = 0.1f;
};
