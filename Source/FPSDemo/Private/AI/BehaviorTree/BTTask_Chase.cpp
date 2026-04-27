// 版权所有 Epic Games, Inc. 保留所有权利。

#include "AI/BehaviorTree/BTTask_Chase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

namespace
{
const FName ChaseTargetBlackboardKey(TEXT("Target"));
const FName ChaseStateBlackboardKey(TEXT("State"));
const FName ChaseTaskStateName(TEXT("Chase"));
}

UBTTask_Chase::UBTTask_Chase()
{
    bNotifyTick = true;
    bTickIntervals = true;
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

    UObject* Target = Blackboard->GetValueAsObject(ChaseTargetBlackboardKey);
    AActor* TargetActor = Cast<AActor>(Target);
    if (!TargetActor)
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsName(ChaseStateBlackboardKey, ChaseTaskStateName);

    AIController->MoveToActor(TargetActor, AcceptableRadius);

    // 设置Tick间隔
    SetNextTickTime(NodeMemory, TickInterval);

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

    UObject* Target = Blackboard->GetValueAsObject(ChaseTargetBlackboardKey);
    AActor* TargetActor = Cast<AActor>(Target);
    if (!TargetActor)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    AIController->MoveToActor(TargetActor, AcceptableRadius);
}
