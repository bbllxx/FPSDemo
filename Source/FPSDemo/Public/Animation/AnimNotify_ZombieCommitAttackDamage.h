#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ZombieCommitAttackDamage.generated.h"

UCLASS(meta = (DisplayName = "Zombie Commit Attack Damage"))
class FPSDEMO_API UAnimNotify_ZombieCommitAttackDamage : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    virtual FString GetNotifyName_Implementation() const override;
};
