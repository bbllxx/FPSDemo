// 版权所有 Epic Games, Inc. 保留所有权利。

#include "AI/BehaviorTree/BTTask_ZombieMoveTo.h"
#include "AIController.h"
#include "Character/Zombies/ZombieBase.h"
#include "Tasks/AITask_MoveTo.h"

UBTTask_ZombieMoveTo::UBTTask_ZombieMoveTo(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("Zombie Move To");
    BlackboardKey.SelectedKeyName = TEXT("Target");
    bTrackMovingGoal = true;
    bObserveBlackboardValue = true;
}

UAITask_MoveTo* UBTTask_ZombieMoveTo::PrepareMoveTask(
    UBehaviorTreeComponent& OwnerComp,
    UAITask_MoveTo* ExistingTask,
    FAIMoveRequest& MoveRequest)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return nullptr;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        return nullptr;
    }

    MoveRequest.SetAcceptanceRadius(Zombie->GetChaseAcceptableRadius());

    return Super::PrepareMoveTask(OwnerComp, ExistingTask, MoveRequest);
}
