#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieBase.h"

namespace
{
const FName AttackTargetBlackboardKey(TEXT("Target"));
const FName AttackStateBlackboardKey(TEXT("State"));
const FName AttackTaskStateName(TEXT("Attack"));
}

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard)
    {
        return EBTNodeResult::Failed;
    }

    UObject* Target = Blackboard->GetValueAsObject(AttackTargetBlackboardKey);
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsName(AttackStateBlackboardKey, AttackTaskStateName);
    return Zombie->TryStartAttack() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
