#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Character/Zombies/ZombieBase.h"
#include "BTTask_SetZombieMoveSpeed.generated.h"

/**
 * 设置当前僵尸移动速度的通用任务。
 * 该节点只负责把僵尸自身配置的巡逻或追逐速度应用到 CharacterMovement。
 */
UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_SetZombieMoveSpeed : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_SetZombieMoveSpeed();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie AI")
    EZombieMoveSpeedMode MoveSpeedMode = EZombieMoveSpeedMode::Patrol;
};
