// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Chase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Chase::UBTTask_Chase()
{
    bNotifyTick = true;
    NodeName = TEXT("Chase");
}

EBTNodeResult::Type UBTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard)
    {
        return EBTNodeResult::Failed;
    }

    UObject* Target = Blackboard->GetValueAsObject(TEXT("Target"));
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsString(TEXT("State"), TEXT("Chase"));

    AIController->MoveToActor(Cast<AActor>(Target), AcceptableRadius);

    return EBTNodeResult::InProgress;
}

void UBTTask_Chase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !AIController->GetPawn())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (!Blackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UObject* Target = Blackboard->GetValueAsObject(TEXT("Target"));
    if (!Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    AIController->MoveToActor(Cast<AActor>(Target), AcceptableRadius);
}
