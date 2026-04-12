// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Patrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Patrol::UBTTask_Patrol()
{
    NodeName = TEXT("Patrol");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    // Set Blackboard "State" to "Patrol"
    if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
    {
        BlackboardComp->SetValueAsName(TEXT("State"), TEXT("Patrol"));
    }

    // Get spawn point as center
    FVector Center = Pawn->GetActorLocation();

    // Generate random radius between PatrolRadiusMin and PatrolRadiusMax
    float RandomRadius = FMath::RandRange(PatrolRadiusMin, PatrolRadiusMax);

    // Generate random direction
    float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);
    FVector RandomOffset = FVector(FMath::Cos(RandomAngle) * RandomRadius, FMath::Sin(RandomAngle) * RandomRadius, 0.0f);
    FVector RandomPoint = Center + RandomOffset;

    // Find reachable navigation point
    FNavLocation NavLocation;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(AIController);
    if (NavSys && NavSys->GetRandomReachablePointInRadius(RandomPoint, 100.0f, NavLocation))
    {
        TargetLocation = NavLocation.Location;
    }
    else
    {
        // Fallback to random point if navigation fails
        TargetLocation = RandomPoint;
    }

    // Move to target location
    AIController->MoveToLocation(TargetLocation);

    // Initialize state
    bHasReachedTarget = false;
    WaitTimer = 0.0f;

    return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // Check if target is detected (Blackboard Target != None)
    if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
    {
        UObject* Target = BlackboardComp->GetValueAsObject(TEXT("Target"));
        if (Target != nullptr)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }

    // Check if reached target location
    float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetLocation);
    if (Distance < 100.0f)
    {
        if (!bHasReachedTarget)
        {
            bHasReachedTarget = true;
            WaitTimer = 0.0f;
        }

        WaitTimer += DeltaSeconds;
        if (WaitTimer >= PatrolWaitTime)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
