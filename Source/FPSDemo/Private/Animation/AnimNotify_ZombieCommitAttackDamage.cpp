#include "Animation/AnimNotify_ZombieCommitAttackDamage.h"
#include "Character/Zombies/ZombieBase.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_ZombieCommitAttackDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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

    Zombie->CommitAttackDamage();
}

FString UAnimNotify_ZombieCommitAttackDamage::GetNotifyName_Implementation() const
{
    return TEXT("结算僵尸攻击伤害");
}
