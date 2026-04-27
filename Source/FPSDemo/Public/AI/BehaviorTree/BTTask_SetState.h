#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetState.generated.h"

/**
 * 写入黑板状态的通用任务。
 * 该节点只负责把指定 Name 写入指定黑板键，然后立即成功。
 */
UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_SetState : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_SetState();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FName StateKeyName = TEXT("State");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FName StateValue = NAME_None;
};
