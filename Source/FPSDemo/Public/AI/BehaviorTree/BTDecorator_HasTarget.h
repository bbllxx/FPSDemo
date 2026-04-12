// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasTarget.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "HasTarget"))
class FPSDEMO_API UBTDecorator_HasTarget : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_HasTarget();

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
