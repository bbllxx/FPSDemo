# Zombie AI 行为系统实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 为 FPSDemo 项目实现完整的僵尸 AI 行为系统，包含 4 状态（Idle/Patrol/Chase/Attack）的 BehaviorTree 和配套的 C++ 任务/条件节点。

**Architecture:** C++ 实现 6 个 BehaviorTree 节点类（4 Task + 2 Decorator），蓝图组装 BehaviorTree 结构，ZombieAIController 初始化并管理行为树。

**Tech Stack:** UE5.4, C++, BehaviorTree, Blackboard, AIPerception, Blueprint MCP

---

## 文件结构

### 新增 C++ 头文件（Public）

| 文件 | 职责 |
|------|------|
| `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Idle.h` | Idle 任务节点 |
| `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Patrol.h` | Patrol 任务节点 |
| `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Chase.h` | Chase 任务节点 |
| `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Attack.h` | Attack 任务节点 |
| `Source/FPSDemo/Public/AI/BehaviorTree/BTDecorator_HasTarget.h` | HasTarget 条件节点 |
| `Source/FPSDemo/Public/AI/BehaviorTree/BTDecorator_InAttackRange.h` | InAttackRange 条件节点 |

### 新增 C++ 实现文件（Private）

| 文件 | 职责 |
|------|------|
| `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Idle.cpp` | Idle 任务实现 |
| `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Patrol.cpp` | Patrol 任务实现 |
| `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Chase.cpp` | Chase 任务实现 |
| `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Attack.cpp` | Attack 任务实现 |
| `Source/FPSDemo/Private/AI/BehaviorTree/BTDecorator_HasTarget.cpp` | HasTarget 条件实现 |
| `Source/FPSDemo/Private/AI/BehaviorTree/BTDecorator_InAttackRange.cpp` | InAttackRange 条件实现 |

### 修改的 C++ 文件

| 文件 | 修改内容 |
|------|---------|
| `Source/FPSDemo/Public/AI/ZombieAIController.h` | 添加 HomeLocation 属性 |
| `Source/FPSDemo/Private/AI/ZombieAIController.cpp` | OnPossess 中初始化 HomeLocation 和 Blackboard，配置感知回调中的最近距离选择 |

### 新增蓝图资源

| 资源 | 类型 |
|------|------|
| `Content/AI/Zombie/BB_Zombie` | Blackboard Data Asset |
| `Content/AI/Zombie/BT_Zombie` | Behavior Tree Asset |

---

## 任务列表

### Task 1: 创建 UBTTask_Idle

**Files:**
- Create: `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Idle.h`
- Create: `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Idle.cpp`

- [ ] **Step 1: 创建头文件 BTTask_Idle.h**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTaskNode.h"
#include "BTTask_Idle.generated.h"

UCLASS()
class FPSDEMO_API UBTTask_Idle : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Idle();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
```

- [ ] **Step 2: 创建实现文件 BTTask_Idle.cpp**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Idle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Idle::UBTTask_Idle()
{
    NodeName = TEXT("Idle");
}

EBTNodeResult::Type UBTTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (Blackboard)
    {
        Blackboard->SetValueAsName(FName("State"), FName("Idle"));
    }

    return EBTNodeResult::Succeeded;
}
```

---

### Task 2: 创建 UBTTask_Patrol

**Files:**
- Create: `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Patrol.h`
- Create: `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Patrol.cpp`

- [ ] **Step 1: 创建头文件 BTTask_Patrol.h**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTaskNode.h"
#include "BTTask_Patrol.generated.h"

UCLASS()
class FPSDEMO_API UBTTask_Patrol : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Patrol();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadiusMin = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolRadiusMax = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    float PatrolWaitTime = 2.0f;

private:
    FVector TargetLocation;
    bool bHasReachedTarget;
    float WaitTimer;
};
```

- [ ] **Step 2: 创建实现文件 BTTask_Patrol.cpp**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Patrol.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Patrol::UBTTask_Patrol()
{
    NodeName = TEXT("Patrol");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (Blackboard)
    {
        Blackboard->SetValueAsName(FName("State"), FName("Patrol"));
    }

    FVector HomeLocation = Pawn->GetActorLocation();

    float RandomRadius = FMath::RandRange(PatrolRadiusMin, PatrolRadiusMax);
    FVector RandomPoint = HomeLocation + FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal() * RandomRadius;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
    if (NavSys)
    {
        FNavLocation RandomNavLocation;
        if (NavSys->GetRandomReachablePointInRadius(RandomPoint, 100.0f, RandomNavLocation))
        {
            TargetLocation = RandomNavLocation.Location;
        }
        else
        {
            TargetLocation = RandomPoint;
        }
    }
    else
    {
        TargetLocation = RandomPoint;
    }

    AIController->MoveToLocation(TargetLocation);
    bHasReachedTarget = false;
    WaitTimer = 0.0f;

    return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (Blackboard && Blackboard->GetValueAsObject(FName("Target")))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetLocation);
    if (Distance < 100.0f)
    {
        if (!bHasReachedTarget)
        {
            bHasReachedTarget = true;
            WaitTimer = 0.0f;
        }

        WaitTimer += DeltaSeconds;
        if (WaitTimer >= PatrolWaitTime)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }
}
```

---

### Task 3: 创建 UBTTask_Chase

**Files:**
- Create: `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Chase.h`
- Create: `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Chase.cpp`

- [ ] **Step 1: 创建头文件 BTTask_Chase.h**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTaskNode.h"
#include "BTTask_Chase.generated.h"

UCLASS()
class FPSDEMO_API UBTTask_Chase : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Chase();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase")
    float AcceptableRadius = 100.0f;
};
```

- [ ] **Step 2: 创建实现文件 BTTask_Chase.cpp**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Chase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Chase::UBTTask_Chase()
{
    NodeName = TEXT("Chase");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        return EBTNodeResult::Failed;
    }

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(FName("Target")));
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsName(FName("State"), FName("Chase"));

    AIController->MoveToActor(Target, AcceptableRadius);

    return EBTNodeResult::InProgress;
}

void UBTTask_Chase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(FName("Target")));
    if (!Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    AIController->MoveToActor(Target, AcceptableRadius);
}
```

---

### Task 4: 创建 UBTTask_Attack

**Files:**
- Create: `Source/FPSDemo/Public/AI/BehaviorTree/BTTask_Attack.h`
- Create: `Source/FPSDemo/Private/AI/BehaviorTree/BTTask_Attack.cpp`

- [ ] **Step 1: 创建头文件 BTTask_Attack.h**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

UCLASS()
class FPSDEMO_API UBTTask_Attack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_Attack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    float AttackInterval = 1.0f;

    float AttackTimer;
};
```

- [ ] **Step 2: 创建实现文件 BTTask_Attack.cpp**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieBase.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        return EBTNodeResult::Failed;
    }

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(FName("Target")));
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsName(FName("State"), FName("Attack"));

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (Zombie && Zombie->CanAttack())
    {
        Zombie->PerformAttack();
    }

    AttackTimer = 0.0f;

    return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(FName("Target")));
    if (!Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (Zombie->IsTargetInAttackRange() && Zombie->CanAttack())
    {
        Zombie->PerformAttack();
    }

    AttackTimer += DeltaSeconds;
    if (AttackTimer >= AttackInterval)
    {
        AttackTimer = 0.0f;
    }
}
```

---

### Task 5: 创建 UBTDecorator_HasTarget

**Files:**
- Create: `Source/FPSDemo/Public/AI/BehaviorTree/BTDecorator_HasTarget.h`
- Create: `Source/FPSDemo/Private/AI/BehaviorTree/BTDecorator_HasTarget.cpp`

- [ ] **Step 1: 创建头文件 BTDecorator_HasTarget.h**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator.h"
#include "BTDecorator_HasTarget.generated.h"

UCLASS()
class FPSDEMO_API UBTDecorator_HasTarget : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_HasTarget();

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
```

- [ ] **Step 2: 创建实现文件 BTDecorator_HasTarget.cpp**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTDecorator_HasTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HasTarget::UBTDecorator_HasTarget()
{
    NodeName = TEXT("HasTarget");
    FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_HasTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Blackboard)
    {
        return false;
    }

    UObject* Target = Blackboard->GetValueAsObject(FName("Target"));
    return Target != nullptr;
}
```

---

### Task 6: 创建 UBTDecorator_InAttackRange

**Files:**
- Create: `Source/FPSDemo/Public/AI/BehaviorTree/BTDecorator_InAttackRange.h`
- Create: `Source/FPSDemo/Private/AI/BehaviorTree/BTDecorator_InAttackRange.cpp`

- [ ] **Step 1: 创建头文件 BTDecorator_InAttackRange.h**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator.h"
#include "BTDecorator_InAttackRange.generated.h"

UCLASS()
class FPSDEMO_API UBTDecorator_InAttackRange : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_InAttackRange();

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
```

- [ ] **Step 2: 创建实现文件 BTDecorator_InAttackRange.cpp**

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BehaviorTree/BTDecorator_InAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieBase.h"

UBTDecorator_InAttackRange::UBTDecorator_InAttackRange()
{
    NodeName = TEXT("InAttackRange");
    FlowAbortMode = EBTFlowAbortMode::None;
}

bool UBTDecorator_InAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIController();
    if (!AIController)
    {
        return false;
    }

    AZombieBase* Zombie = Cast<AZombieBase>(AIController->GetPawn());
    if (!Zombie)
    {
        return false;
    }

    return Zombie->IsTargetInAttackRange();
}
```

---

### Task 7: 修改 ZombieAIController

**Files:**
- Modify: `Source/FPSDemo/Public/AI/ZombieAIController.h`
- Modify: `Source/FPSDemo/Private/AI/ZombieAIController.cpp`

- [ ] **Step 1: 修改 ZombieAIController.h，添加 HomeLocation 属性**

在 `protected:` 区域添加：

```cpp
// 出生点位置（用于巡逻范围判定）
UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
FVector HomeLocation;
```

- [ ] **Step 2: 修改 ZombieAIController.cpp，修改 OnPossess 方法**

在 `OnPossess` 方法开头添加：

```cpp
void AZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 初始化出生点位置
    if (InPawn)
    {
        HomeLocation = InPawn->GetActorLocation();
    }

    // ... 现有代码 ...
}
```

同时在 `OnPossess` 中 BehaviorTree 启动后，将 HomeLocation 写入 Blackboard：

```cpp
if (Blackboard)
{
    Blackboard->SetValueAsVector(FName("HomeLocation"), HomeLocation);
}
```

- [ ] **Step 3: 修改 OnPerceptionUpdated，实现最近距离优先**

将 `OnPerceptionUpdated` 方法中目标选择逻辑改为：

```cpp
void AZombieAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    AActor* ClosestActor = nullptr;
    float ClosestDistance = FLT_MAX;

    for (AActor* Actor : UpdatedActors)
    {
        ACharacter* SensedCharacter = Cast<ACharacter>(Actor);
        if (SensedCharacter && SensedCharacter->IsPlayerControlled())
        {
            FActorPerceptionBlueprintInfo PerceptionInfo;
            AIPerceptionComp->GetActorsPerception(Actor, PerceptionInfo);

            bool bIsSensed = false;
            for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
            {
                if (Stimulus.WasSuccessfullySensed())
                {
                    bIsSensed = true;
                    break;
                }
            }

            if (bIsSensed)
            {
                float Distance = FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation());
                if (Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestActor = Actor;
                }
            }
        }
    }

    if (ClosestActor)
    {
        SetTargetPlayer(ClosestActor);
        bCanSeeTarget = true;

        AZombieBase* Zombie = GetControlledZombie();
        if (Zombie)
        {
            Zombie->SetTargetPlayer(ClosestActor);
        }
    }
}
```

- [ ] **Step 4: 在 ZombieAIController.cpp 顶部添加引用**

```cpp
#include "Character/Zombies/ZombieBase.h"
```

---

### Task 8: 创建蓝图资源 BB_Zombie

**Files:**
- Create: `Content/AI/Zombie/BB_Zombie.uasset`

**注意:** Blueprint MCP 不直接支持创建 Blackboard Asset，需要手动在 UE 编辑器中创建。

- [ ] **Step 1: 在 UE 编辑器中手动创建 Blackboard**

1. 在 Content Browser 中导航到 `Content/AI/Zombie/`（如不存在需先创建文件夹）
2. 右键 → AI → Blackboard
3. 命名为 `BB_Zombie`
4. 打开 Blackboard，添加以下 Keys：

| Key Name | Key Type | Description |
|----------|----------|-------------|
| `State` | Name | 当前 AI 状态 |
| `Target` | Object | 目标玩家 Actor |
| `HomeLocation` | Vector | 出生点位置 |

---

### Task 9: 创建蓝图资源 BT_Zombie

**Files:**
- Create: `Content/AI/Zombie/BT_Zombie.uasset`

- [ ] **Step 1: 创建 BehaviorTree Asset**

使用 Blueprint MCP 工具构建树结构：

1. 创建根节点（默认 Selector）
2. 添加 `Sequence: HasTarget` 分支
   - 添加 `Decorator: HasTarget`（链接 BB_Zombie.Target）
   - 添加 `Decorator: InAttackRange`（链接 ZombieBase）
   - 添加 `Task: Attack`（使用 UBTTask_Attack）
3. 在 HasTarget 分支下添加 `Task: Chase`（使用 UBTTask_Chase）
4. 添加 `Sequence: NoTarget` 分支
   - 添加 `Task: Patrol`（使用 UBTTask_Patrol）

**BehaviorTree 节点结构：**

```
Root
 └── Selector
      ├── Sequence: HasTarget
      │    ├── BTDecorator: HasTarget (C++ 类)
      │    ├── BTDecorator: InAttackRange (C++ 类)
      │    └── BTTask: Attack (C++ 类)
      ├── BTTask: Chase (C++ 类)
      └── Sequence: NoTarget
           └── BTTask: Patrol (C++ 类)
```

- [ ] **Step 2: 设置 Root Decorator Chain**

在 HasTarget Sequence 中，Decorator 应组成链式条件：先检查 HasTarget，再检查 InAttackRange。

---

### Task 10: 配置 AI Controller 和僵尸蓝图

**Files:**
- Create/Modify: `Content/AI/Zombie/BP_ZombieAIController`（如已存在则修改）

**说明:** 如果你的僵尸（如 BP_HeavyZombie）已经设置了 `AI Controller Class = AZombieAIController`，则可以直接在僵尸蓝图中设置 BehaviorTree 和 Blackboard。

- [ ] **Step 1: 设置 BT 和 BB 引用**

**方案 A - 在僵尸蓝图中设置（推荐）：**
1. 打开你的僵尸蓝图（如 `BP_HeavyZombie`）
2. 在 Details 面板中找到 `AI Controller Class`
3. 确保设置为 `BP_ZombieAIController` 或 `AZombieAIController`
4. 在 `BehaviorTree` 属性中设置 `BT_Zombie`
5. 在 `Blackboard Asset` 属性中设置 `BB_Zombie`

**方案 B - 创建独立的 AIController 蓝图：**
1. 在 Content Browser 中创建 Blueprint Class，继承自 `BP_ZombieAIController` 或 `ZombieAIController`
2. 设置 `BehaviorTreeAsset` = `BT_Zombie`
3. 设置 `BlackboardAsset` = `BB_Zombie`
4. 在僵尸蓝图的 `AI Controller Class` 中选择你创建的 Controller

---

## 实施顺序

1. Task 1-6: 创建 C++ 节点类
2. Task 7: 修改 ZombieAIController
3. Task 8-9: 创建蓝图资源
4. Task 10: 配置并测试

---

## 验证步骤

1. 编译 C++ 代码，确认无错误
2. 在 UE 编辑器中打开 `BB_Zombie`，确认 Keys 正确（State, Target, HomeLocation）
3. 打开 `BT_Zombie`，确认树结构正确（Root → Selector → HasTarget Sequence + NoTarget Sequence）
4. 确认 BT_Zombie 和 BB_Zombie 已赋值给僵尸蓝图或 AIController
5. 运行游戏，验证：僵尸索敌（感知玩家）、追击（向玩家移动）、攻击（在攻击范围内造成伤害）、巡逻（丢失目标后随机游走）
