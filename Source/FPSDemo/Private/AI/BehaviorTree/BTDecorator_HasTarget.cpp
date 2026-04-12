// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTDecorator_HasTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HasTarget::UBTDecorator_HasTarget()
{
    NodeName = TEXT("HasTarget");
    FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_HasTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        return false;
    }

    UObject* Target = Blackboard->GetValueAsObject(TEXT("Target"));
    return Target != nullptr;
}
