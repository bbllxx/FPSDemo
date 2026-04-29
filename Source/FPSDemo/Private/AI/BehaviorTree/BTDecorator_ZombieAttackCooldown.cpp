#include "AI/BehaviorTree/BTDecorator_ZombieAttackCooldown.h"
#include "AIController.h"
#include "Character/Zombies/ZombieBase.h"

UBTDecorator_ZombieAttackCooldown::UBTDecorator_ZombieAttackCooldown()
{
    NodeName = TEXT("ZombieAttackCooldown");
    FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_ZombieAttackCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

    return Zombie->CanAttackAboutCooldown();
}
