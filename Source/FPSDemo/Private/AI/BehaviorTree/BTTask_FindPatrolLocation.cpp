#include "AI/BehaviorTree/BTTask_FindPatrolLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

namespace
{
const FVector NavigationProjectionExtent(100.0f, 100.0f, 300.0f);
constexpr int32 MaxPatrolLocationAttempts = 8;
}

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
    NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!AIController || !Blackboard || PatrolLocationKeyName.IsNone())
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    if (Blackboard->GetKeyID(PatrolLocationKeyName) == FBlackboard::InvalidKey)
    {
        return EBTNodeResult::Failed;
    }

    FVector Center = Pawn->GetActorLocation();
    if (!CenterLocationKeyName.IsNone() && Blackboard->GetKeyID(CenterLocationKeyName) != FBlackboard::InvalidKey)
    {
        Center = Blackboard->GetValueAsVector(CenterLocationKeyName);
    }

    const float MinRadius = FMath::Max(0.0f, PatrolRadiusMin);
    const float MaxRadius = FMath::Max(MinRadius, PatrolRadiusMax);
    FVector PatrolLocation = Center;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIController);
    if (NavSys)
    {
        for (int32 AttemptIndex = 0; AttemptIndex < MaxPatrolLocationAttempts; ++AttemptIndex)
        {
            const float RandomRadius = FMath::RandRange(MinRadius, MaxRadius);
            const float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);
            const FVector RandomOffset(
                FMath::Cos(RandomAngle) * RandomRadius,
                FMath::Sin(RandomAngle) * RandomRadius,
                0.0f);
            const FVector CandidateLocation = Center + RandomOffset;

            FNavLocation NavLocation;
            if (NavSys->ProjectPointToNavigation(CandidateLocation, NavLocation, NavigationProjectionExtent))
            {
                PatrolLocation = NavLocation.Location;
                break;
            }
        }
    }

    Blackboard->SetValueAsVector(PatrolLocationKeyName, PatrolLocation);
    return EBTNodeResult::Succeeded;
}
