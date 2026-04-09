// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombies/ZombieBase.h"
#include "RangedZombie.generated.h"

UCLASS()
class FPSDEMO_API ARangedZombie : public AZombieBase
{
    GENERATED_BODY()

public:
    ARangedZombie();

    virtual void BeginPlay() override;

    virtual void PerformAttack() override;

    UFUNCTION(BlueprintCallable, Category = "Ranged Zombie")
    void FireProjectile();

    UFUNCTION(BlueprintPure, Category = "Ranged Zombie")
    float GetIdealAttackRange() const { return IdealAttackRange; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Zombie")
    TSubclassOf<class AActor> ProjectileClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* MuzzleLocation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Zombie")
    float IdealAttackRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged Zombie")
    float ProjectileSpeed;

    virtual void DealDamageToTarget() override;

    UFUNCTION(BlueprintPure, Category = "Ranged Zombie")
    bool ShouldMoveCloser() const;

    UFUNCTION(BlueprintPure, Category = "Ranged Zombie")
    bool ShouldMoveAway() const;
};

