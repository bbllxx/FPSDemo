# UE5 FPS 多武器 LineTrace 枪械系统计划

## Summary

在现有投射物武器之外新增一套独立枪械框架：`AWeaponBase` 负责 LineTrace 开火、爆头和伤害结算；`UWeaponInventoryComponent` 管理 3 个武器槽、切枪、弹匣和共享备弹；`UWeaponDataAsset` 统一配置步枪、手枪、霰弹枪数值。现有 `UTP_WeaponComponent`、`AFPSDemoProjectile` 先保留，避免破坏当前蓝图资产。

## Key Changes

- 新增枪械核心：
  - `AWeaponBase`：包含 `USkeletalMeshComponent` 武器模型、`UArrowComponent` 发射点、`StartFire/StopFire/FireOnce/Reload`。
  - 使用相机位置和相机方向做 `LineTraceSingleByChannel`；霰弹枪用多条 Trace 和散布角。
  - 爆头规则：`Hit.BoneName` 忽略大小写包含 `"head"` 时应用爆头倍率。
  - 伤害调用优先用 `UGameplayStatics::ApplyPointDamage`，保留命中点、骨骼和 `FHitResult`，仍进入僵尸现有 `TakeDamage()` 流程。
- 新增配置与库存：
  - `UWeaponDataAsset` 配置槽位、弹药类型、弹匣容量、备弹上限、基础伤害、爆头倍率、射程、射速、换弹时间、开火模式、弹丸数量、散布角、音效。
  - `UWeaponInventoryComponent` 挂到 `AFPSDemoCharacter`，管理 `Primary/Rifle`、`Secondary/Pistol`、`Special/Shotgun` 三槽；同槽拾取新武器时替换旧武器并销毁旧枪。
  - 备弹按弹药类型共享：`RifleAmmo`、`PistolAmmo`、`ShellAmmo`。
- 输入与蓝图配置：
  - `AFPSDemoCharacter` 绑定 `Fire`、`Reload`、`NextWeapon`、`PreviousWeapon`、`Slot1`、`Slot2`、`Slot3`，转发给库存组件。
  - `AFPSDemoPlayerController` 可增加武器输入 MappingContext；具体 `IA_*` 和 `IMC_Weapons` 在蓝图/Content 中配置。
  - C++ 提供 `BlueprintImplementableEvent`/动态委托：开火特效、换弹开始/结束、当前武器变化、弹药变化；枪口粒子和武器蒙太奇由 BP 实现。
- 碰撞与命中：
  - 在 `Config/DefaultEngine.ini` 新增 `WeaponTrace` Trace Channel。
  - `AZombieBase` 构造中让胶囊体忽略 `WeaponTrace`，让僵尸 Mesh 阻挡 `WeaponTrace`，确保 LineTrace 能命中骨骼并拿到 `Hit.BoneName`。
  - 玩家第一人称手臂、当前武器和持有者 Actor 会被 Trace 忽略。
- 联机边界：
  - 第一版按单机可用实现，但接口预留服务端权威：`ServerStartFire/ServerStopFire/ServerReload/ServerEquipSlot`。
  - 不做客户端预测和完整 Multicast 特效；复制当前武器槽、弹匣和备弹状态，为后续多人同步留接口。

## Initial Weapon Defaults

- 步枪：自动，伤害 25，爆头 x2，射程 8000，弹匣 30，初始备弹 90，射击间隔 0.12 秒。
- 手枪：半自动，伤害 35，爆头 x2，射程 6000，弹匣 12，初始备弹 60，射击间隔 0.35 秒。
- 霰弹枪：半自动，8 弹丸，每弹丸伤害 12，爆头 x1.5，射程 3000，弹匣 8，初始备弹 32，散布角 6 度，射击间隔 0.9 秒。

## Implementation Files

- 新增：
  - `Public/Weapon/WeaponTypes.h`
  - `Public/Weapon/Data/WeaponDataAsset.h`
  - `Public/Weapon/Weapons/WeaponBase.h`
  - `Public/Weapon/Components/WeaponInventoryComponent.h`
  - `Public/Weapon/Pickups/WeaponPickup.h`
  - 对应 `Private/Weapon/.../*.cpp`
  - `Private/Tests/WeaponSystemTests.cpp`
- 修改：
  - `AFPSDemoCharacter`：添加库存组件、武器输入 Action 属性和输入转发。
  - `AFPSDemoPlayerController`：添加武器 MappingContext 支持。
  - `AZombieBase`：配置 `WeaponTrace` 对胶囊体和 Mesh 的响应。
  - `Config/DefaultEngine.ini`：新增 `WeaponTrace` 通道。
- 代码规范：
  - 新增文件不加 Epic 版权注释。
  - 修改现有文件时移除/替换 Epic 版权注释。
  - 所有新增/触碰注释使用中文；遇到英文注释同步翻译。

## Test Plan

- 自动化测试新增 `FPSDemo.Weapon.*`：
  - 库存添加三把武器后能按槽位切换。
  - 同槽拾取新武器会替换旧武器并销毁旧实例。
  - 共享备弹换弹时正确从 Reserve 转入 Magazine。
  - 弹匣为空时不能开火，换弹中不能重复开火。
  - `"head"`、`"Head"`、`"head_jnt"` 骨骼名触发爆头倍率，非头部不触发。
  - 霰弹枪按配置生成固定数量弹丸方向。
  - `ApplyPointDamage` 能让僵尸现有 `TakeDamage()` 扣血并保留命中点。
- 编译验证：
  - `UnrealBuildTool.exe FPSDemoEditor Win64 Development -Project="...\FPSDemo.uproject" -WaitMutex -NoHotReload`
- 自动化验证：
  - `UnrealEditor-Cmd.exe "...FPSDemo.uproject" -Unattended -NullRHI -ExecCmds="Automation RunTests FPSDemo.Weapon; Quit" -TestExit="Automation Test Queue Empty" -Log`

## Assumptions

- 第一版只改 C++ 和必要碰撞配置；不直接编辑 `.uasset`。
- 蓝图侧后续创建 3 个 `UWeaponDataAsset`，并把 `BP_PickUp_Rifle` 迁移到新的 `AWeaponPickup` 派生蓝图。
- 旧投射物武器系统暂不删除，等蓝图迁移完成后再清理。
