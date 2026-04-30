#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/WeaponTypes.h"
#include "WeaponDataAsset.generated.h"

class USoundBase;

UCLASS(BlueprintType)
class FPSDEMO_API UWeaponDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UWeaponDataAsset();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName WeaponId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    EWeaponSlot Slot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    EWeaponAmmoType AmmoType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    EWeaponFireMode FireMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0"))
    int32 MagazineCapacity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0"))
    int32 MaxReserveAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "1"))
    float HeadshotMultiplier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "0"))
    float Range;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire", meta = (ClampMin = "0"))
    float FireInterval;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reload", meta = (ClampMin = "0"))
    float ReloadDuration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "1"))
    int32 PelletsPerShot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "0"))
    float SpreadAngleDegrees;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    USoundBase* FireSound;
};
