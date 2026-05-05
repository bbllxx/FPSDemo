#include "AI/BehaviorTree/BTTask_SetZombieMoveSpeed.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Zombies/ZombieBase.h"

UBTTask_SetZombieMoveSpeed::UBTTask_SetZombieMoveSpeed()
{
    NodeName = TEXT("Set Zombie Move Speed");
}

EBTNodeResult::Type UBTTask_SetZombieMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        return EBTNodeResult::Failed;
    }

    Zombie->ApplyMoveSpeed(MoveSpeedMode);
    return EBTNodeResult::Succeeded;
}
