#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_Attack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Attack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
    void HandleAttackFinished(class AZombieBase* FinishedZombie);
    void ClearAttackBinding();

    TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
    TWeakObjectPtr<class AZombieBase> CachedZombie;
    FDelegateHandle AttackFinishedDelegateHandle;
};
