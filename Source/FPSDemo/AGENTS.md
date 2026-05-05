# AGENTS.md

## 生成代码版权规范

- 生成或新增代码文件时，不要添加 `// Copyright Epic Games, Inc. All Rights Reserved.`。
- 这是 Epic 官方代码的版权注释，不适用于按用户需求生成的项目代码。
- 如果修改现有文件时碰到这类 Epic 官方版权注释，应按项目注释规范移除或替换为合适的个人说明。

## 代码注释规范

- 所有代码注释必须使用中文。
- 修改代码时，如果碰到原有英文注释，需要同步翻译为中文。

## 项目信息

- 这是一个 UE5.4 FPS 打僵尸游戏项目。
- 项目根目录：`C:\Users\Administrator.DESKTOP-V16TMRT\Documents\Unreal Projects\FPSDemo`
- 当前源码模块目录：`C:\Users\Administrator.DESKTOP-V16TMRT\Documents\Unreal Projects\FPSDemo\Source\FPSDemo`
- UE5 源码路径：`E:\UE_5.4\Engine\Source\Runtime\Engine`
- `.uproject` 使用 `EngineAssociation` 5.4，主模块为 `FPSDemo`。
- 插件启用：`ModelingToolsEditorMode`、`VisualStudioTools`、`BlueprintMCP`。

## MCP 工具使用约定

- 可以使用 BlueprintMCP 查询虚幻蓝图资源、蓝图图表、默认值和相关设置；除非用户明确要求修改，否则不得通过 BlueprintMCP 创建、删除、移动、连线或改写任何蓝图资源、节点或属性。
- 使用 BlueprintMCP 创建或调整蓝图图表时，必须把可读性作为硬性约束：新增节点不得重叠或集中在同一坐标；应按执行流、数据流或状态流方向分层排布，相关节点靠近、不同职责留出间距，尽量减少连线交叉；若工具没有自动布局能力，应在创建节点时显式指定位置或创建后调整位置，保证蓝图便于人工检查和后续维护。
- 遇到由动画蓝图变量、过渡条件、移动速度或加速度判断引起的问题时，优先说明需要用户在蓝图中手动调整；除非用户明确要求，不要用 C++ 反射或 BlueprintMCP 改写蓝图来绕过。

## 编译命令

- 编辑器目标：

```powershell
Set-Location "C:\Users\Administrator.DESKTOP-V16TMRT\Documents\Unreal Projects\FPSDemo"
& "E:\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" FPSDemoEditor Win64 Development -Project="C:\Users\Administrator.DESKTOP-V16TMRT\Documents\Unreal Projects\FPSDemo\FPSDemo.uproject" -WaitMutex -NoHotReload
```

- 游戏目标：

```powershell
Set-Location "C:\Users\Administrator.DESKTOP-V16TMRT\Documents\Unreal Projects\FPSDemo"
& "E:\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" FPSDemo Win64 Development -Project="C:\Users\Administrator.DESKTOP-V16TMRT\Documents\Unreal Projects\FPSDemo\FPSDemo.uproject" -WaitMutex -NoHotReload
```

## 模块与依赖

- `FPSDemo.Build.cs` 依赖：`Core`、`CoreUObject`、`Engine`、`InputCore`、`EnhancedInput`、`GameplayTasks`、`AIModule`、`NavigationSystem`。
- `FPSDemo.Target.cs` 是 Game 目标，`FPSDemoEditor.Target.cs` 是 Editor 目标，均使用 `BuildSettingsVersion.V5` 和 `EngineIncludeOrderVersion.Unreal5_4`。

## 源码结构速查

- `Private/Core`、`Public/Core`：主模块入口，`IMPLEMENT_PRIMARY_GAME_MODULE`。
- `Private/Game`、`Public/Game`：`AFPSDemoGameMode`，默认 Pawn 指向 `/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter`。
- `Private/Character`、`Public/Character`：第一人称角色与玩家控制器，使用 Enhanced Input。
- `Private/Character/Zombies`、`Public/Character/Zombies`：僵尸基类与三种僵尸子类。
- `Private/AI`、`Public/AI`：僵尸 AIController。
- `Private/AI/BehaviorTree`、`Public/AI/BehaviorTree`：行为树 Task 和 Decorator。
- `Private/Weapon`、`Public/Weapon`：武器组件、拾取组件、投射物。

## 玩家与武器系统

- `AFPSDemoCharacter` 是第一人称角色，包含 `Mesh1P`、`FirstPersonCameraComponent`、`JumpAction`、`MoveAction`、`LookAction`。
- `AFPSDemoPlayerController` 在 `BeginPlay` 中把 `InputMappingContext` 加入 Enhanced Input Local Player Subsystem。
- `UTP_WeaponComponent` 继承 `USkeletalMeshComponent`，负责附加到 `AFPSDemoCharacter::GetMesh1P()` 的 `GripPoint`，绑定 `FireAction`，发射 `AFPSDemoProjectile`。
- `UTP_PickUpComponent` 继承 `USphereComponent`，重叠到 `AFPSDemoCharacter` 后广播 `OnPickUp`，然后移除重叠监听。
- `AFPSDemoProjectile` 使用 `USphereComponent` 和 `UProjectileMovementComponent`，默认碰撞 Profile 为 `Projectile`，目前 `OnHit` 只对物理模拟组件施加冲量并销毁。

## 僵尸系统

- `AZombieBase` 继承 `ACharacter`，提供血量、攻击伤害、攻击距离、攻击冷却、目标引用、受伤、死亡和攻击逻辑。
- `EZombieType` 类型：`Heavy`、`Light`、`Ranged`、`None`。
- `AZombieBase::TryStartAttack()` 检查目标、冷却和待结算状态后触发 `OnAttackAnim()`，不判断攻击距离。
- `AZombieBase::CommitAttackDamage()` 由攻击动画通知调用，结算时再检查当前目标是否仍在攻击范围内。
- `AZombieBase::DealDamageToTarget()` 对 `TargetPlayer` 调用 `UGameplayStatics::ApplyDamage`，前提是目标有效且在攻击范围内。
- `AZombieBase::OnDeath()` 触发 `OnDeathAnim()`，关闭胶囊体碰撞，禁用移动，3 秒后销毁。
- `AHeavyZombie`：`MaxHealth=200`、`AttackDamage=25`、`AttackRange=150`、`AttackCooldown=2`、`MaxWalkSpeed=200`、`DamageReduction=0.5`。
- `ALightZombie`：`MaxHealth=50`、`AttackDamage=10`、`AttackRange=100`、`AttackCooldown=1`、`MaxWalkSpeed=600`。
- `ARangedZombie`：`MaxHealth=100`、`AttackDamage=15`、`AttackRange=2000`、`AttackCooldown=2`、`MaxWalkSpeed=350`、`IdealAttackRange=1500`、`ProjectileSpeed=2000`。
- `ARangedZombie` 重写 `TryStartAttack()`，通过 `FireProjectile()` 生成 `ProjectileClass`，远程伤害预期由投射物自身处理。

## AI 系统

- `AZombieAIController` 继承 `AAIController`，包含 `UAIPerceptionComponent` 和视觉感知配置。
- 视觉感知默认：`SightRadius=2000`、`LoseSightRadius=2500`、`PeripheralVisionAngleDegrees=120`、`MaxAge=2`。
- AI 控制器字段：`BehaviorTreeAsset`、`BlackboardAsset`、`TargetPlayer`、`bCanPerceiveTarget`、`CurrentState`、`ChaseRange=2000`、`LoseTargetRange=2500`、`HomeLocation`。
- `OnPossess()` 记录出生点，启动 Behavior Tree，把 `HomeLocation` 写入 Blackboard，并每 0.5 秒调用 `CheckTargetVisibility()`。
- `OnPerceptionUpdated()` 从感知到的玩家中选最近目标，调用 `SetTargetPlayer()` 并同步到受控僵尸。
- `CheckTargetVisibility()` 超出 `LoseTargetRange` 或在不可见且超出 `ChaseRange` 时调用 `OnTargetLost()`。
- `ChaseTarget()` 使用 `MoveToActor(TargetPlayer, 100)`，`AttackTarget()` 调用僵尸 `TryStartAttack()`，`StopAIMovement()` 调用 `StopMovement()`。

## Behavior Tree 约定

- Blackboard 常用键：`Target`、`State`、`HomeLocation`。
- `Target` 是否存在使用 UE 内置 Blackboard Decorator 检查，不再维护自定义 `HasTarget`。
- `UBTDecorator_InAttackRange`：从 AIController Pawn 转为 `AZombieBase`，调用 `IsTargetInAttackRange()`。
- `UBTDecorator_ZombieAttackCooldown`：从 AIController Pawn 转为 `AZombieBase`，调用 `CanAttackAboutCooldown()`。
- `UBTTask_SetState`：把 Blackboard 的 `State` 写成任务配置的 `StateValue`，立即成功。
- `UBTTask_FindPatrolLocation`：围绕 `HomeLocation` 生成导航巡逻点并写入 `PatrolLocation`，不移动、不等待、不 Tick。
- `UBTTask_ZombieMoveTo`：继承 UE 的 `UBTTask_MoveTo`，发起移动前读取僵尸自身的追击接受半径，不自己 Tick，也不重复 `MoveToActor()`。
- `UBTTask_Attack`：把 `State` 设为 `Attack`，只调用 `TryStartAttack()` 并立即返回，不自己判断攻击范围或循环等待冷却。

## 已知注意点

- 当前多个源码文件中的中文注释在 PowerShell 输出中显示为乱码；编辑代码时应保持或恢复为正常中文注释。
- 旧追击任务和旧巡逻任务已删除；行为树应改用 `SetState`、`ZombieMoveTo`、`FindPatrolLocation`、内置 `Move To` 和内置 `Wait` 组合。
- `AFPSDemoProjectile::OnHit()` 目前只处理物理模拟组件，没有直接对僵尸造成伤害；后续武器伤害系统可能会改为 LineTrace 和爆头判定。
- 根目录 `待办事项.md` 记录的方向包括：Decorator/BehaviorTree 优化、攻击 AnimNotify、攻击流程重构、武器 LineTrace 与爆头判定。
