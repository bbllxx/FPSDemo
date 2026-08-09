#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ZombieFinishAttack.generated.h"

UCLASS(meta = (DisplayName = "Zombie Finish Attack"))
class FPSDEMO_API UAnimNotify_ZombieFinishAttack : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    virtual FString GetNotifyName_Implementation() const override;
};
