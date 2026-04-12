# Zombie AI 行为系统设计

## 1. 架构总览

### 行为树结构

```
Root
 └── Selector (状态选择)
      ├── Sequence: HasTarget (有目标时)
      │    ├── Decorator: InAttackRange? (在攻击范围？)
      │    │    └── Task: Attack
      │    └── Task: Chase (追击)
      └── Sequence: NoTarget (无目标)
           └── Task: Patrol (随机巡逻)
```

### 状态枚举（与 Blackboard Key "State" 同步）

| 状态 | 说明 |
|------|------|
| `Idle` | 初始/等待状态 |
| `Patrol` | 随机巡逻 |
| `Chase` | 追击目标 |
| `Attack` | 攻击 |

### 状态转换图

```
[Idle] → 发现玩家 → [Chase]
   ↓                   ↓
[Patrol] ← 丢失目标 ← [Attack]
```

**行为逻辑：**
- 丢失目标后 → 直接进入 Patrol
- 巡逻过程中发现玩家 → 进入 Chase

---

## 2. 组件列表

### C++ 实现（7个类）

| 类名 | 继承 | 职责 |
|------|------|------|
| `UBTTask_Idle` | UBTTaskNode | 进入 Idle 状态 |
| `UBTTask_Patrol` | UBTTaskNode | 随机巡逻（计算随机导航目标并移动） |
| `UBTTask_Chase` | UBTTaskNode | 追击目标（MoveToActor） |
| `UBTTask_Attack` | UBTTaskNode | 攻击（调用 Zombie::PerformAttack） |
| `UBTDecorator_HasTarget` | UBTDecorator | 检查 Target 是否有值 |
| `UBTDecorator_InAttackRange` | UBTDecorator | 检查目标是否在 AttackRange 内 |

### 蓝图资源（需创建）

| 资源 | 类型 | 说明 |
|------|------|------|
| `BB_Zombie` | Blackboard Data Asset | 含 Key: State (Name), Target (Object) |
| `BT_Zombie` | Behavior Tree Asset | 行为树主结构 |

### 修改的现有文件

- `AZombieAIController` — 添加 `HomeLocation` 属性，初始化到 Blackboard，配置 BehaviorTree/Blackboard 引用

---

## 3. 核心逻辑流程

### Patrol（随机巡逻）

1. 检查是否已有目标 → 有则结束（返回 Failed）
2. 计算随机巡逻点：以 HomeLocation 为圆心，500~1000 单位内随机选一点
3. MoveTo 随机点
4. 等待到达或超时（5秒）
5. 循环

### Chase（追击）

1. Target 无效则结束（返回 Failed）
2. MoveToActor(Target, 100 单位停止距离)
3. 更新状态为 `Chase`
4. 返回 InProgress（持续追击直到条件不满足）

### Attack（攻击）

1. Target 无效则结束（返回 Failed）
2. 检查冷却：调用 `Zombie->CanAttack()`
3. 调用 `Zombie->PerformAttack()`
4. 更新状态为 `Attack`
5. 返回 InProgress

### Decorator 逻辑

| 条件 | 逻辑 |
|------|------|
| `HasTarget` | Blackboard Key `Target` != None |
| `InAttackRange` | 获取 Target，计算距离 <= Zombie.AttackRange |

---

## 4. 寻敌优先级

**最近距离优先：** 当多个玩家存在时，选择距离最近的玩家作为 Target。

（在感知回调 `OnPerceptionUpdated` 中实现）

---

## 5. 实现顺序

1. **创建 C++ 任务/条件节点类**（6个类）
2. **修改 ZombieAIController**（添加 HomeLocation，配置 BT/BB）
3. **创建蓝图资源**（BB_Zombie, BT_Zombie）
4. **配置 AIController 的 BehaviorTree 和 Blackboard 引用**

---

## 6. 文件位置

### 新增 C++ 文件

**头文件（Public）：**
```
Source/FPSDemo/Public/AI/BehaviorTree/
├── BTTask_Idle.h
├── BTTask_Patrol.h
├── BTTask_Chase.h
├── BTTask_Attack.h
├── BTDecorator_HasTarget.h
└── BTDecorator_InAttackRange.h
```

**实现文件（Private）：**
```
Source/FPSDemo/Private/AI/BehaviorTree/
├── BTTask_Idle.cpp
├── BTTask_Patrol.cpp
├── BTTask_Chase.cpp
├── BTTask_Attack.cpp
├── BTDecorator_HasTarget.cpp
└── BTDecorator_InAttackRange.cpp
```

### 修改的 C++ 文件

```
Source/FPSDemo/Public/AI/ZombieAIController.h
Source/FPSDemo/Private/AI/ZombieAIController.cpp
```

### 新增蓝图资源

```
Content/AI/Zombie/
├── BB_Zombie.uasset
└── BT_Zombie.uasset
```

---

## 7. 依赖关系

- 依赖 `AIModule`（已添加到 FPSDemo.Build.cs）
- 依赖 `NavigationSystem`（用于随机巡逻点的导航查询）
- ZombieBase 已有接口 `CanAttack()`, `IsTargetInAttackRange()`, `PerformAttack()` 可直接复用
