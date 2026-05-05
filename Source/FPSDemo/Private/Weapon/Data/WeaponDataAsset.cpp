#include "Weapon/Data/WeaponDataAsset.h"

UWeaponDataAsset::UWeaponDataAsset()
{
    WeaponId = NAME_None;
    Slot = EWeaponSlot::Primary;
    AmmoType = EWeaponAmmoType::Rifle;
    FireMode = EWeaponFireMode::SemiAuto;
    MagazineCapacity = 30;
    InitialReserveAmmo = 90;
    MaxReserveAmmo = 999;
    BaseDamage = 25.0f;
    HeadshotMultiplier = 2.0f;
    Range = 8000.0f;
    FireInterval = 0.12f;
    ReloadDuration = 1.0f;
    PelletsPerShot = 1;
    SpreadAngleDegrees = 0.0f;
    FireSound = nullptr;
}
