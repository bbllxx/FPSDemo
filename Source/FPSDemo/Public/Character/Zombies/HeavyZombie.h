// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombies/ZombieBase.h"
#include "HeavyZombie.generated.h"

UCLASS()
class FPSDEMO_API AHeavyZombie : public AZombieBase
{
    GENERATED_BODY()

public:
    AHeavyZombie();

    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy Zombie")
    float DamageReduction;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
};

