// 版权所有 Epic Games, Inc. 保留所有权利。

#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieBase.h"

namespace
{
const FName TargetBlackboardKey(TEXT("Target"));
const FName StateBlackboardKey(TEXT("State"));
const FName AttackStateName(TEXT("Attack"));
}

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

    UObject* Target = Blackboard->GetValueAsObject(TargetBlackboardKey);
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie || !Zombie->IsTargetInAttackRange())
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsName(StateBlackboardKey, AttackStateName);
    Zombie->PerformAttack();

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

    UObject* Target = Blackboard->GetValueAsObject(TargetBlackboardKey);
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

    if (!Zombie->IsTargetInAttackRange())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (Zombie->CanAttackAboutCooldown())
    {
        Zombie->PerformAttack();
    }

    AttackTimer += DeltaSeconds;
}
