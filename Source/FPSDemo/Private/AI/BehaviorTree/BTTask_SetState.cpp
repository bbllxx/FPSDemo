#include "AI/BehaviorTree/BTTask_SetState.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetState::UBTTask_SetState()
{
    NodeName = TEXT("Set State");
}

EBTNodeResult::Type UBTTask_SetState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard || StateKeyName.IsNone() || StateValue.IsNone())
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsName(StateKeyName, StateValue);
    return EBTNodeResult::Succeeded;
}
