// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

UCLASS()
class FPSDEMO_API AZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    AZombieAIController();

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    AActor* GetTargetPlayer() const { return TargetPlayer.Get(); }

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void SetTargetPlayer(AActor* NewTarget);

    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    bool CanSeeTarget() const { return bCanSeeTarget; }

    UFUNCTION(BlueprintPure, Category = "Zombie AI")
    FName GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void SetCurrentState(FName NewState);

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void ChaseTarget();

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void AttackTarget();

    UFUNCTION(BlueprintCallable, Category = "Zombie AI")
    void StopAIMovement();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<class UBlackboardData> BlackboardAsset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UAIPerceptionComponent> AIPerceptionComp;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    TWeakObjectPtr<class AActor> TargetPlayer;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    bool bCanSeeTarget;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    FName CurrentState;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie AI")
    float ChaseRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie AI")
    float LoseTargetRange;

    FTimerHandle VisibilityCheckTimer;

    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    UFUNCTION()
    void CheckTargetVisibility();

    UFUNCTION()
    void OnTargetLost();

    UFUNCTION()
    class AZombieBase* GetControlledZombie() const;

    UFUNCTION()
    void StartBehaviorTree();

    UFUNCTION()
    void StopBehaviorTree();
};

