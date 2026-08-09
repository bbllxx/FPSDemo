#include "Animation/AnimNotify_ZombieFinishAttack.h"
#include "Character/Zombies/ZombieBase.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_ZombieFinishAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp)
    {
        return;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(MeshComp->GetOwner());
    if (!Zombie)
    {
        return;
    }

    Zombie->FinishAttack();
}

FString UAnimNotify_ZombieFinishAttack::GetNotifyName_Implementation() const
{
    return TEXT("结束僵尸攻击");
}
