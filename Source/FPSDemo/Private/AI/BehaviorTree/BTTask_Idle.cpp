// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Idle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Idle::UBTTask_Idle()
{
    NodeName = TEXT("Idle");
}

EBTNodeResult::Type UBTTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (Blackboard)
    {
        Blackboard->SetValueAsName(FName("State"), FName("Idle"));
    }

    return EBTNodeResult::Succeeded;
}
