// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombies/ZombieBase.h"
#include "LightZombie.generated.h"

UCLASS()
class FPSDEMO_API ALightZombie : public AZombieBase
{
    GENERATED_BODY()

public:
    ALightZombie();

    virtual void BeginPlay() override;
};

