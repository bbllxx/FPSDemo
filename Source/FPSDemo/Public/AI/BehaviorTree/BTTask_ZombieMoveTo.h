// 版权所有 Epic Games, Inc. 保留所有权利。

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_ZombieMoveTo.generated.h"

class UAITask_MoveTo;

/**
 * 基于 UE 内置 MoveTo 的僵尸移动任务。
 * 只负责把接受半径切换为当前僵尸自己的配置，其余移动逻辑继续交给引擎处理。
 */
UCLASS(Blueprintable)
class FPSDEMO_API UBTTask_ZombieMoveTo : public UBTTask_MoveTo
{
    GENERATED_BODY()

public:
    UBTTask_ZombieMoveTo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual UAITask_MoveTo* PrepareMoveTask(
        UBehaviorTreeComponent& OwnerComp,
        UAITask_MoveTo* ExistingTask,
        FAIMoveRequest& MoveRequest) override;
};
