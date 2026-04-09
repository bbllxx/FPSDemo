// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/ZombieAIController.h"
#include "Character/Zombies/ZombieBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

AZombieAIController::AZombieAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = 2500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 120.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());

    TargetPlayer = nullptr;
    bCanSeeTarget = false;
    CurrentState = FName("Idle");
    ChaseRange = 2000.0f;
    LoseTargetRange = 2500.0f;
}

void AZombieAIController::BeginPlay()
{
    Super::BeginPlay();

    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnPerceptionUpdated);
    }
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    StartBehaviorTree();
    GetWorldTimerManager().SetTimer(VisibilityCheckTimer, this, &AZombieAIController::CheckTargetVisibility, 0.5f, true);
}

void AZombieAIController::OnUnPossess()
{
    Super::OnUnPossess();

    StopBehaviorTree();
    GetWorldTimerManager().ClearTimer(VisibilityCheckTimer);
}

void AZombieAIController::StartBehaviorTree()
{
    if (BlackboardAsset)
    {
        UBlackboardComponent* BlackboardComp = nullptr;
        UseBlackboard(BlackboardAsset, BlackboardComp);
        Blackboard = BlackboardComp;
    }

    if (BehaviorTreeAsset && Blackboard)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AZombieAIController::StopBehaviorTree()
{
    if (BrainComponent)
    {
        BrainComponent->StopLogic(FString("Unpossessed"));
    }
}

void AZombieAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        ACharacter* SensedCharacter = Cast<ACharacter>(Actor);
        if (SensedCharacter && SensedCharacter->IsPlayerControlled())
        {
            FActorPerceptionBlueprintInfo PerceptionInfo;
            AIPerceptionComp->GetActorsPerception(Actor, PerceptionInfo);

            bool bIsSensed = false;
            for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
            {
                if (Stimulus.WasSuccessfullySensed())
                {
                    bIsSensed = true;
                    break;
                }
            }

            if (bIsSensed)
            {
                SetTargetPlayer(Actor);
                bCanSeeTarget = true;

                AZombieBase* Zombie = GetControlledZombie();
                if (Zombie)
                {
                    Zombie->SetTargetPlayer(Actor);
                }
            }
            else if (TargetPlayer == Actor)
            {
                CheckTargetVisibility();
            }
        }
    }
}

void AZombieAIController::CheckTargetVisibility()
{
    if (!TargetPlayer.IsValid())
    {
        bCanSeeTarget = false;
        return;
    }

    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), TargetPlayer.Get()->GetActorLocation());

    if (Distance > LoseTargetRange)
    {
        OnTargetLost();
        return;
    }

    FActorPerceptionBlueprintInfo PerceptionInfo;
    AIPerceptionComp->GetActorsPerception(TargetPlayer.Get(), PerceptionInfo);

    bool bIsSensed = false;
    for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            bIsSensed = true;
            break;
        }
    }

    if (!bIsSensed && Distance > ChaseRange)
    {
        OnTargetLost();
    }
    else
    {
        bCanSeeTarget = bIsSensed;
    }
}

void AZombieAIController::OnTargetLost()
{
    TargetPlayer = nullptr;
    bCanSeeTarget = false;

    AZombieBase* Zombie = GetControlledZombie();
    if (Zombie)
    {
        Zombie->SetTargetPlayer(nullptr);
    }
}

void AZombieAIController::SetTargetPlayer(AActor* NewTarget)
{
    TargetPlayer = NewTarget;

    AZombieBase* Zombie = GetControlledZombie();
    if (Zombie)
    {
        Zombie->SetTargetPlayer(NewTarget);
    }
}

void AZombieAIController::SetCurrentState(FName NewState)
{
    CurrentState = NewState;

    if (Blackboard)
    {
        Blackboard->SetValueAsName(FName("State"), NewState);
    }
}

void AZombieAIController::ChaseTarget()
{
    if (!TargetPlayer.IsValid() || !GetPawn())
    {
        return;
    }

    MoveToActor(TargetPlayer.Get(), 100.0f, true, true, false, nullptr, true);
}

void AZombieAIController::AttackTarget()
{
    AZombieBase* Zombie = GetControlledZombie();
    if (Zombie)
    {
        Zombie->PerformAttack();
    }
}

void AZombieAIController::StopAIMovement()
{
    StopMovement();
}

AZombieBase* AZombieAIController::GetControlledZombie() const
{
    return Cast<AZombieBase>(GetPawn());
}

