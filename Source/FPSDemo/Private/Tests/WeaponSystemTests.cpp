#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Character/FPSDemoCharacter.h"
#include "Character/Zombies/LightZombie.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Weapon/Components/WeaponInventoryComponent.h"
#include "Weapon/Data/WeaponDataAsset.h"
#include "Weapon/Weapons/WeaponBase.h"

namespace FPSDemo::WeaponTests
{
UWorld* CreateTestWorld()
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (World && GEngine)
    {
        FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
        WorldContext.SetCurrentWorld(World);
    }
    return World;
}

void DestroyTestWorld(UWorld* World)
{
    if (!World)
    {
        return;
    }

    World->DestroyWorld(false);
    if (GEngine)
    {
        GEngine->DestroyWorldContext(World);
    }
}

UWeaponDataAsset* MakeWeaponData(
    EWeaponSlot Slot,
    EWeaponAmmoType AmmoType,
    int32 MagazineCapacity,
    int32 InitialReserveAmmo)
{
    UWeaponDataAsset* Data = NewObject<UWeaponDataAsset>();
    Data->Slot = Slot;
    Data->AmmoType = AmmoType;
    Data->MagazineCapacity = MagazineCapacity;
    Data->InitialReserveAmmo = InitialReserveAmmo;
    Data->MaxReserveAmmo = 999;
    Data->BaseDamage = 25.0f;
    Data->HeadshotMultiplier = 2.0f;
    Data->Range = 8000.0f;
    Data->FireInterval = 0.1f;
    Data->ReloadDuration = 0.5f;
    Data->PelletsPerShot = 1;
    Data->SpreadAngleDegrees = 0.0f;
    Data->FireMode = EWeaponFireMode::SemiAuto;
    return Data;
}

AWeaponBase* SpawnWeapon(UWorld* World, UWeaponDataAsset* Data)
{
    AWeaponBase* Weapon = World->SpawnActor<AWeaponBase>();
    Weapon->SetWeaponData(Data);
    return Weapon;
}

UWeaponInventoryComponent* CreateInventory(AFPSDemoCharacter* Character)
{
    UWeaponInventoryComponent* Inventory = NewObject<UWeaponInventoryComponent>(Character);
    Character->AddInstanceComponent(Inventory);
    Inventory->RegisterComponent();
    Inventory->InitializeInventory(Character);
    return Inventory;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseHeadshotDamageTest,
    "FPSDemo.Weapon.WeaponBase.HeadshotDamageUsesBoneName",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseHeadshotDamageTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    UWeaponDataAsset* Data = FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 90);
    AWeaponBase* Weapon = FPSDemo::WeaponTests::SpawnWeapon(World, Data);

    TestTrue(TEXT("小写 head 应识别为爆头骨骼"), AWeaponBase::IsHeadshotBone(TEXT("head")));
    TestTrue(TEXT("混合大小写 Head 应识别为爆头骨骼"), AWeaponBase::IsHeadshotBone(TEXT("Head")));
    TestTrue(TEXT("包含 head 的骨骼名应识别为爆头骨骼"), AWeaponBase::IsHeadshotBone(TEXT("neck_head_jnt")));
    TestFalse(TEXT("身体骨骼不应识别为爆头"), AWeaponBase::IsHeadshotBone(TEXT("spine_03")));
    TestEqual(TEXT("爆头伤害应乘以爆头倍率"), Weapon->CalculateDamageForBone(TEXT("Head")), 50.0f);
    TestEqual(TEXT("非爆头应使用基础伤害"), Weapon->CalculateDamageForBone(TEXT("spine_03")), 25.0f);

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponInventoryEquipSlotsTest,
    "FPSDemo.Weapon.Inventory.EquipsConfiguredSlots",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponInventoryEquipSlotsTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    AFPSDemoCharacter* Character = World->SpawnActor<AFPSDemoCharacter>();
    UWeaponInventoryComponent* Inventory = FPSDemo::WeaponTests::CreateInventory(Character);

    AWeaponBase* Rifle = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 90));
    AWeaponBase* Pistol = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Secondary, EWeaponAmmoType::Pistol, 12, 60));
    AWeaponBase* Shotgun = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Special, EWeaponAmmoType::Shell, 8, 32));

    TestTrue(TEXT("应能添加步枪"), Inventory->AddWeapon(Rifle));
    TestTrue(TEXT("应能添加手枪"), Inventory->AddWeapon(Pistol));
    TestTrue(TEXT("应能添加霰弹枪"), Inventory->AddWeapon(Shotgun));

    TestTrue(TEXT("应能切换到主武器槽"), Inventory->EquipWeaponSlot(EWeaponSlot::Primary));
    TestTrue(TEXT("当前武器应为步枪"), Inventory->GetCurrentWeapon() == Rifle);
    TestTrue(TEXT("应能切换到副武器槽"), Inventory->EquipWeaponSlot(EWeaponSlot::Secondary));
    TestTrue(TEXT("当前武器应为手枪"), Inventory->GetCurrentWeapon() == Pistol);
    TestTrue(TEXT("应能切换到特殊武器槽"), Inventory->EquipWeaponSlot(EWeaponSlot::Special));
    TestTrue(TEXT("当前武器应为霰弹枪"), Inventory->GetCurrentWeapon() == Shotgun);

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponInventoryReplaceSlotTest,
    "FPSDemo.Weapon.Inventory.ReplacesWeaponInSameSlot",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponInventoryReplaceSlotTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    AFPSDemoCharacter* Character = World->SpawnActor<AFPSDemoCharacter>();
    UWeaponInventoryComponent* Inventory = FPSDemo::WeaponTests::CreateInventory(Character);

    AWeaponBase* OldRifle = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 90));
    AWeaponBase* NewRifle = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 90));

    TestTrue(TEXT("应能添加旧步枪"), Inventory->AddWeapon(OldRifle));
    TestTrue(TEXT("应能用新步枪替换同槽武器"), Inventory->AddWeapon(NewRifle));

    TestTrue(TEXT("主武器槽应指向新步枪"), Inventory->GetWeaponInSlot(EWeaponSlot::Primary) == NewRifle);
    TestTrue(TEXT("旧步枪应进入销毁流程"), OldRifle->IsActorBeingDestroyed());

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponInventoryReloadSharedReserveTest,
    "FPSDemo.Weapon.Inventory.ReloadUsesSharedReserveAmmo",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponInventoryReloadSharedReserveTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    AFPSDemoCharacter* Character = World->SpawnActor<AFPSDemoCharacter>();
    UWeaponInventoryComponent* Inventory = FPSDemo::WeaponTests::CreateInventory(Character);
    AWeaponBase* Rifle = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 0));

    TestTrue(TEXT("应能添加步枪"), Inventory->AddWeapon(Rifle));
    TestTrue(TEXT("应能装备步枪"), Inventory->EquipWeaponSlot(EWeaponSlot::Primary));
    Rifle->SetCurrentMagazineAmmo(5);
    Inventory->SetReserveAmmo(EWeaponAmmoType::Rifle, 10);

    TestTrue(TEXT("应能开始换弹"), Inventory->ReloadCurrentWeapon());
    TestTrue(TEXT("应能完成换弹"), Inventory->FinishReloadCurrentWeapon());
    TestEqual(TEXT("弹匣应从共享备弹补充 10 发"), Rifle->GetCurrentMagazineAmmo(), 15);
    TestEqual(TEXT("共享备弹应扣减为 0"), Inventory->GetReserveAmmo(EWeaponAmmoType::Rifle), 0);

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseCannotFireWhileEmptyOrReloadingTest,
    "FPSDemo.Weapon.WeaponBase.CannotFireWhenEmptyOrReloading",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseCannotFireWhileEmptyOrReloadingTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    AWeaponBase* Weapon = FPSDemo::WeaponTests::SpawnWeapon(
        World,
        FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 90));

    Weapon->SetCurrentMagazineAmmo(0);
    TestFalse(TEXT("空弹匣时不能开火"), Weapon->CanFire());

    Weapon->SetCurrentMagazineAmmo(1);
    TestTrue(TEXT("有子弹且未换弹时可以开火"), Weapon->CanFire());
    TestTrue(TEXT("应能进入换弹状态"), Weapon->StartReload());
    TestFalse(TEXT("换弹中不能开火"), Weapon->CanFire());

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseShotgunPelletCountTest,
    "FPSDemo.Weapon.WeaponBase.ShotgunBuildsConfiguredPellets",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseShotgunPelletCountTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    UWeaponDataAsset* Data = FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Special, EWeaponAmmoType::Shell, 8, 32);
    Data->PelletsPerShot = 8;
    Data->SpreadAngleDegrees = 6.0f;
    AWeaponBase* Weapon = FPSDemo::WeaponTests::SpawnWeapon(World, Data);

    TestEqual(TEXT("霰弹枪应按配置生成 8 条弹丸方向"),
        Weapon->BuildShotDirections(FVector::ForwardVector).Num(),
        8);

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWeaponBaseAppliesPointDamageTest,
    "FPSDemo.Weapon.WeaponBase.AppliesPointDamageToZombie",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWeaponBaseAppliesPointDamageTest::RunTest(const FString& Parameters)
{
    UWorld* World = FPSDemo::WeaponTests::CreateTestWorld();
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    UWeaponDataAsset* Data = FPSDemo::WeaponTests::MakeWeaponData(EWeaponSlot::Primary, EWeaponAmmoType::Rifle, 30, 90);
    AWeaponBase* Weapon = FPSDemo::WeaponTests::SpawnWeapon(World, Data);
    UGameInstance* GameInstance = NewObject<UGameInstance>();
    World->SetGameInstance(GameInstance);
    TestTrue(TEXT("测试世界应能创建 GameMode 以允许 Pawn 接收伤害"), World->SetGameMode(FURL()));
    TestNotNull(TEXT("测试世界应存在权威 GameMode"), World->GetAuthGameMode());
    ALightZombie* Zombie = World->SpawnActor<ALightZombie>(FVector::ZeroVector, FRotator::ZeroRotator);
    const float HealthBefore = Zombie->GetCurrentHealth();

    FHitResult Hit(Zombie, Zombie->GetMesh(), Zombie->GetActorLocation(), FVector::UpVector);
    Hit.BoneName = TEXT("head");
    const float AppliedDamage = Weapon->ApplyDamageToHit(Hit, FVector::ForwardVector);

    TestEqual(TEXT("爆头点伤害应返回最终伤害"), AppliedDamage, 50.0f);
    TestEqual(TEXT("僵尸生命值应扣除爆头伤害"), Zombie->GetCurrentHealth(), HealthBefore - 50.0f);

    FPSDemo::WeaponTests::DestroyTestWorld(World);
    return true;
}

#endif
