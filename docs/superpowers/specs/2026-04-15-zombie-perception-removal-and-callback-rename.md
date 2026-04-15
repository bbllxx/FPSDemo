# 僵尸类修改设计文档

**日期：** 2026-04-15
**修改内容：** 去除僵尸感知系统 + 重命名动画回调函数

---

## 1. 概述

移除 `ZombieBase` 类及其子类中的 `UPawnSensingComponent` 感知系统组件，并将动画回调函数重命名以明确其用途。

---

## 2. 修改详情

### 2.1 移除感知系统

**涉及文件：**
- `Source/FPSDemo/Public/Character/Zombies/ZombieBase.h`
- `Source/FPSDemo/Private/Character/Zombies/ZombieBase.cpp`

**移除内容：**

| 类型 | 位置 | 内容 |
|------|------|------|
| 成员变量 | h 第107-108行 | `UPawnSensingComponent* PawnSensingComp` |
| 回调声明 | h 第114-116行 | `OnSeePawn(APawn* Pawn)` |
| 回调声明 | h 第118-120行 | `OnHearNoise(APawn*, FVector, float)` |
| 组件创建 | cpp 第27行 | `CreateDefaultSubobject<UPawnSensingComponent>` |
| 组件配置 | cpp 第28-33行 | 感知半径、视野角度、听觉阈值配置 |
| 回调绑定 | cpp 第52-58行 | `OnSeePawn` 和 `OnHearNoise` 绑定 |
| 头文件引用 | cpp 第6行 | `#include "Perception/PawnSensingComponent.h"` |

### 2.2 重命名动画回调

**涉及文件：**
- `Source/FPSDemo/Public/Character/Zombies/ZombieBase.h`
- `Source/FPSDemo/Private/Character/Zombies/ZombieBase.cpp`

**重命名对应：**

| 原函数名 | 新函数名 | 类型 |
|----------|----------|------|
| `OnAttack()` | `OnAttackAnim()` | BlueprintImplementableEvent |
| `OnTakeDamage(float, FVector)` | `OnTakeDamageAnim(float, FVector)` | BlueprintImplementableEvent |
| `OnDeathEvent()` | `OnDeathAnim()` | BlueprintImplementableEvent |

**调用处更新：**
- `PerformAttack()` 中调用 `OnAttack()` → `OnAttackAnim()`
- `TakeDamage()` 中调用 `OnTakeDamage()` → `OnTakeDamageAnim()`
- `OnDeath()` 中调用 `OnDeathEvent()` → `OnDeathAnim()`

---

## 3. 注意事项

- 移除感知组件后，僵尸的目标玩家将由 `ZombieAIController` 通过 AI 感知系统进行管理
- `ZombieBase` 的子类（`HeavyZombie`、`LightZombie`、`RangedZombie`）不直接使用感知系统，无需修改
- Blueprint 中实现 `OnAttackAnim`、`OnTakeDamageAnim`、`OnDeathAnim` 的地方需要同步重命名

---

## 4. 验证步骤

1. 编译 C++ 代码，确认无错误
2. 检查 Blueprint 中对应的事件节点是否需要重命名（需手动在 UE 编辑器中操作）
3. 运行游戏验证僵尸行为正常
