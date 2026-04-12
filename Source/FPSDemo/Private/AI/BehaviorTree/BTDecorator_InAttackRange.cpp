// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTDecorator_InAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieBase.h"

UBTDecorator_InAttackRange::UBTDecorator_InAttackRange()
{
    NodeName = TEXT("InAttackRange");
    FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_InAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return false;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        return false;
    }

    return Zombie->IsTargetInAttackRange();
}
