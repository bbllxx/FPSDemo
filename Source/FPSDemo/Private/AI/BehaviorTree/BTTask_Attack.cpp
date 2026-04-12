// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieBase.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true;
    NodeName = TEXT("Attack");
    AttackTimer = 0.0f;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    Blackboard->SetValueAsString(TEXT("State"), TEXT("Attack"));

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (Zombie && Zombie->CanAttack())
    {
        Zombie->PerformAttack();
    }

    AttackTimer = 0.0f;

    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard)
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

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (Zombie->IsTargetInAttackRange() && Zombie->CanAttack())
    {
        Zombie->PerformAttack();
    }

    AttackTimer += DeltaSeconds;
}
