#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Zombies/ZombieBase.h"

namespace
{
const FName AttackTargetBlackboardKey(TEXT("Target"));
}

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
    bCreateNodeInstance = true;
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

    ClearAttackBinding();

    CachedOwnerComp = &OwnerComp;
    CachedZombie = Zombie;
    AttackFinishedDelegateHandle = Zombie->OnAttackFinished.AddUObject(this, &UBTTask_Attack::HandleAttackFinished);

    if (!Zombie->TryStartAttack())
    {
        ClearAttackBinding();
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ClearAttackBinding();
    return EBTNodeResult::Aborted;
}

void UBTTask_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    ClearAttackBinding();
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_Attack::HandleAttackFinished(AZombieBase* FinishedZombie)
{
    if (!CachedZombie.IsValid() || CachedZombie.Get() != FinishedZombie || !CachedOwnerComp.IsValid())
    {
        return;
    }

    UBehaviorTreeComponent* OwnerComp = CachedOwnerComp.Get();
    ClearAttackBinding();
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_Attack::ClearAttackBinding()
{
    if (CachedZombie.IsValid() && AttackFinishedDelegateHandle.IsValid())
    {
        CachedZombie->OnAttackFinished.Remove(AttackFinishedDelegateHandle);
    }

    AttackFinishedDelegateHandle.Reset();
    CachedZombie.Reset();
    CachedOwnerComp.Reset();
}
