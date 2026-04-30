#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "WeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
    None UMETA(DisplayName = "无"),
    Primary UMETA(DisplayName = "主武器"),
    Secondary UMETA(DisplayName = "副武器"),
    Special UMETA(DisplayName = "特殊武器")
};

UENUM(BlueprintType)
enum class EWeaponAmmoType : uint8
{
    None UMETA(DisplayName = "无"),
    Rifle UMETA(DisplayName = "步枪弹药"),
    Pistol UMETA(DisplayName = "手枪弹药"),
    Shell UMETA(DisplayName = "霰弹")
};

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
    SemiAuto UMETA(DisplayName = "半自动"),
    FullAuto UMETA(DisplayName = "全自动")
};

namespace FPSDemoWeapon
{
    // DefaultEngine.ini 中 WeaponTrace 对应 ECC_GameTraceChannel2。
    static constexpr ECollisionChannel WeaponTraceChannel = ECC_GameTraceChannel2;
}
