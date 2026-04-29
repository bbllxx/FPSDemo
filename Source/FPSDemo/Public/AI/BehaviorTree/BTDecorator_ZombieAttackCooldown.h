#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ZombieAttackCooldown.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "ZombieAttackCooldown"))
class FPSDEMO_API UBTDecorator_ZombieAttackCooldown : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_ZombieAttackCooldown();

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
