// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InAttackRange.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "InAttackRange"))
class FPSDEMO_API UBTDecorator_InAttackRange : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_InAttackRange();

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
