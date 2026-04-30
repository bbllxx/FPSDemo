#include "Weapon/Components/WeaponInventoryComponent.h"

#include "Character/FPSDemoCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon/Data/WeaponDataAsset.h"
#include "Weapon/Weapons/WeaponBase.h"

UWeaponInventoryComponent::UWeaponInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    OwnerCharacter = nullptr;
    PrimaryWeapon = nullptr;
    SecondaryWeapon = nullptr;
    SpecialWeapon = nullptr;
    CurrentWeapon = nullptr;
    CurrentSlot = EWeaponSlot::None;
}

void UWeaponInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!OwnerCharacter)
    {
        InitializeInventory(Cast<AFPSDemoCharacter>(GetOwner()));
    }
}

void UWeaponInventoryComponent::InitializeInventory(AFPSDemoCharacter* NewOwnerCharacter)
{
    OwnerCharacter = NewOwnerCharacter;
}

bool UWeaponInventoryComponent::AddWeapon(AWeaponBase* NewWeapon)
{
    if (!NewWeapon || !NewWeapon->GetWeaponData())
    {
        return false;
    }

    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<AFPSDemoCharacter>(GetOwner());
    }

    UWeaponDataAsset* Data = NewWeapon->GetWeaponData();
    if (Data->Slot == EWeaponSlot::None)
    {
        return false;
    }

    AWeaponBase*& SlotWeapon = GetWeaponSlotRef(Data->Slot);
    const bool bReplacingCurrentSlot = CurrentSlot == Data->Slot;
    if (SlotWeapon && SlotWeapon != NewWeapon)
    {
        SlotWeapon->StopFire();
        SlotWeapon->Destroy();
    }

    SlotWeapon = NewWeapon;
    NewWeapon->InitializeWeapon(OwnerCharacter, this);
    AttachWeaponToOwner(NewWeapon);
    AddReserveAmmo(Data->AmmoType, Data->InitialReserveAmmo, Data->MaxReserveAmmo);

    if (!CurrentWeapon || bReplacingCurrentSlot)
    {
        return EquipWeaponSlot(Data->Slot);
    }

    SetWeaponVisible(NewWeapon, false);
    NotifyWeaponAmmoChanged(NewWeapon);
    return true;
}

bool UWeaponInventoryComponent::EquipWeaponSlot(EWeaponSlot Slot)
{
    AWeaponBase* WeaponToEquip = GetWeaponInSlot(Slot);
    if (!WeaponToEquip)
    {
        return false;
    }

    if (CurrentWeapon == WeaponToEquip)
    {
        return true;
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
        SetWeaponVisible(CurrentWeapon, false);
    }

    CurrentWeapon = WeaponToEquip;
    CurrentSlot = Slot;
    SetWeaponVisible(CurrentWeapon, true);
    OnCurrentWeaponChanged.Broadcast(CurrentWeapon);
    NotifyWeaponAmmoChanged(CurrentWeapon);
    return true;
}

bool UWeaponInventoryComponent::EquipNextWeapon()
{
    const TArray<EWeaponSlot> OccupiedSlots = GetOccupiedSlots();
    if (OccupiedSlots.Num() == 0)
    {
        return false;
    }

    int32 CurrentIndex = OccupiedSlots.IndexOfByKey(CurrentSlot);
    if (CurrentIndex == INDEX_NONE)
    {
        CurrentIndex = -1;
    }

    const int32 NextIndex = (CurrentIndex + 1) % OccupiedSlots.Num();
    return EquipWeaponSlot(OccupiedSlots[NextIndex]);
}

bool UWeaponInventoryComponent::EquipPreviousWeapon()
{
    const TArray<EWeaponSlot> OccupiedSlots = GetOccupiedSlots();
    if (OccupiedSlots.Num() == 0)
    {
        return false;
    }

    int32 CurrentIndex = OccupiedSlots.IndexOfByKey(CurrentSlot);
    if (CurrentIndex == INDEX_NONE)
    {
        CurrentIndex = 0;
    }

    const int32 PreviousIndex = (CurrentIndex - 1 + OccupiedSlots.Num()) % OccupiedSlots.Num();
    return EquipWeaponSlot(OccupiedSlots[PreviousIndex]);
}

bool UWeaponInventoryComponent::StartFire()
{
    return CurrentWeapon ? CurrentWeapon->StartFire() : false;
}

void UWeaponInventoryComponent::StopFire()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->StopFire();
    }
}

bool UWeaponInventoryComponent::ReloadCurrentWeapon()
{
    if (!CurrentWeapon || !CurrentWeapon->GetWeaponData())
    {
        return false;
    }

    const UWeaponDataAsset* Data = CurrentWeapon->GetWeaponData();
    if (CurrentWeapon->GetCurrentMagazineAmmo() >= CurrentWeapon->GetMagazineCapacity())
    {
        return false;
    }

    if (GetReserveAmmo(Data->AmmoType) <= 0)
    {
        return false;
    }

    return CurrentWeapon->StartReload();
}

bool UWeaponInventoryComponent::FinishReloadCurrentWeapon()
{
    if (!CurrentWeapon || !CurrentWeapon->GetWeaponData() || !CurrentWeapon->IsReloading())
    {
        return false;
    }

    const UWeaponDataAsset* Data = CurrentWeapon->GetWeaponData();
    const int32 MissingAmmo = CurrentWeapon->GetMagazineCapacity() - CurrentWeapon->GetCurrentMagazineAmmo();
    const int32 CurrentReserveAmmo = GetReserveAmmo(Data->AmmoType);
    const int32 AmmoToLoad = FMath::Min(MissingAmmo, CurrentReserveAmmo);
    if (AmmoToLoad <= 0)
    {
        CurrentWeapon->FinishReload();
        return false;
    }

    CurrentWeapon->SetCurrentMagazineAmmo(CurrentWeapon->GetCurrentMagazineAmmo() + AmmoToLoad);
    SetReserveAmmo(Data->AmmoType, CurrentReserveAmmo - AmmoToLoad);
    const bool bFinished = CurrentWeapon->FinishReload();
    NotifyWeaponAmmoChanged(CurrentWeapon);
    return bFinished;
}

AWeaponBase* UWeaponInventoryComponent::GetWeaponInSlot(EWeaponSlot Slot) const
{
    switch (Slot)
    {
    case EWeaponSlot::Primary:
        return PrimaryWeapon;
    case EWeaponSlot::Secondary:
        return SecondaryWeapon;
    case EWeaponSlot::Special:
        return SpecialWeapon;
    default:
        return nullptr;
    }
}

int32 UWeaponInventoryComponent::GetReserveAmmo(EWeaponAmmoType AmmoType) const
{
    const int32* ReserveAmmo = ReserveAmmoByType.Find(AmmoType);
    return ReserveAmmo ? *ReserveAmmo : 0;
}

void UWeaponInventoryComponent::SetReserveAmmo(EWeaponAmmoType AmmoType, int32 NewReserveAmmo)
{
    if (AmmoType == EWeaponAmmoType::None)
    {
        return;
    }

    ReserveAmmoByType.FindOrAdd(AmmoType) = FMath::Max(NewReserveAmmo, 0);
    if (CurrentWeapon && CurrentWeapon->GetWeaponData() && CurrentWeapon->GetWeaponData()->AmmoType == AmmoType)
    {
        NotifyWeaponAmmoChanged(CurrentWeapon);
    }
}

void UWeaponInventoryComponent::AddReserveAmmo(EWeaponAmmoType AmmoType, int32 AmmoToAdd, int32 MaxReserveAmmo)
{
    if (AmmoType == EWeaponAmmoType::None || AmmoToAdd <= 0)
    {
        return;
    }

    const int32 CurrentReserveAmmo = GetReserveAmmo(AmmoType);
    const int32 NewReserveAmmo = MaxReserveAmmo > 0
        ? FMath::Min(CurrentReserveAmmo + AmmoToAdd, MaxReserveAmmo)
        : CurrentReserveAmmo + AmmoToAdd;
    SetReserveAmmo(AmmoType, NewReserveAmmo);
}

void UWeaponInventoryComponent::NotifyWeaponAmmoChanged(AWeaponBase* Weapon)
{
    if (!Weapon || !Weapon->GetWeaponData())
    {
        return;
    }

    OnWeaponAmmoChanged.Broadcast(
        Weapon,
        Weapon->GetCurrentMagazineAmmo(),
        GetReserveAmmo(Weapon->GetWeaponData()->AmmoType));
}

void UWeaponInventoryComponent::ServerStartFire_Implementation()
{
    StartFire();
}

void UWeaponInventoryComponent::ServerStopFire_Implementation()
{
    StopFire();
}

void UWeaponInventoryComponent::ServerReload_Implementation()
{
    ReloadCurrentWeapon();
}

void UWeaponInventoryComponent::ServerEquipSlot_Implementation(EWeaponSlot Slot)
{
    EquipWeaponSlot(Slot);
}

AWeaponBase*& UWeaponInventoryComponent::GetWeaponSlotRef(EWeaponSlot Slot)
{
    switch (Slot)
    {
    case EWeaponSlot::Primary:
        return PrimaryWeapon;
    case EWeaponSlot::Secondary:
        return SecondaryWeapon;
    case EWeaponSlot::Special:
        return SpecialWeapon;
    default:
        return PrimaryWeapon;
    }
}

bool UWeaponInventoryComponent::HasWeaponInSlot(EWeaponSlot Slot) const
{
    return GetWeaponInSlot(Slot) != nullptr;
}

TArray<EWeaponSlot> UWeaponInventoryComponent::GetOccupiedSlots() const
{
    TArray<EWeaponSlot> OccupiedSlots;
    if (HasWeaponInSlot(EWeaponSlot::Primary))
    {
        OccupiedSlots.Add(EWeaponSlot::Primary);
    }
    if (HasWeaponInSlot(EWeaponSlot::Secondary))
    {
        OccupiedSlots.Add(EWeaponSlot::Secondary);
    }
    if (HasWeaponInSlot(EWeaponSlot::Special))
    {
        OccupiedSlots.Add(EWeaponSlot::Special);
    }
    return OccupiedSlots;
}

void UWeaponInventoryComponent::AttachWeaponToOwner(AWeaponBase* Weapon)
{
    if (!Weapon || !OwnerCharacter || !OwnerCharacter->GetMesh1P())
    {
        return;
    }

    USkeletalMeshComponent* Mesh1P = OwnerCharacter->GetMesh1P();
    const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    const FName GripSocketName(TEXT("GripPoint"));
    const FName AttachSocketName = Mesh1P->GetSkeletalMeshAsset() && Mesh1P->DoesSocketExist(GripSocketName)
        ? GripSocketName
        : NAME_None;
    Weapon->AttachToComponent(Mesh1P, AttachmentRules, AttachSocketName);
}

void UWeaponInventoryComponent::SetWeaponVisible(AWeaponBase* Weapon, bool bVisible) const
{
    if (!Weapon)
    {
        return;
    }

    Weapon->SetActorHiddenInGame(!bVisible);
    Weapon->SetActorEnableCollision(false);
}
