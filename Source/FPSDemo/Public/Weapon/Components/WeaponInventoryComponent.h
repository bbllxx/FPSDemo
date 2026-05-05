#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponTypes.h"
#include "WeaponInventoryComponent.generated.h"

class AFPSDemoCharacter;
class AWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentWeaponChangedSignature, AWeaponBase*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponAmmoChangedSignature, AWeaponBase*, Weapon, int32, MagazineAmmo, int32, ReserveAmmo);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPSDEMO_API UWeaponInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponInventoryComponent();

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FCurrentWeaponChangedSignature OnCurrentWeaponChanged;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FWeaponAmmoChangedSignature OnWeaponAmmoChanged;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void InitializeInventory(AFPSDemoCharacter* NewOwnerCharacter);

    // 添加武器会按数据资产的槽位归档，同槽已有武器会被替换。
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool AddWeapon(AWeaponBase* NewWeapon);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool EquipWeaponSlot(EWeaponSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool EquipNextWeapon();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool EquipPreviousWeapon();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool StartFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StopFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool ReloadCurrentWeapon();

    // 完成换弹时才搬运共享备弹，便于由动画通知控制真正装填时机。
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool FinishReloadCurrentWeapon();

    UFUNCTION(BlueprintPure, Category = "Weapon")
    AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    AWeaponBase* GetWeaponInSlot(EWeaponSlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetReserveAmmo(EWeaponAmmoType AmmoType) const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetReserveAmmo(EWeaponAmmoType AmmoType, int32 NewReserveAmmo);

    // 备弹按弹药类型共享，不按单把武器单独存储。
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void AddReserveAmmo(EWeaponAmmoType AmmoType, int32 AmmoToAdd, int32 MaxReserveAmmo);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void NotifyWeaponAmmoChanged(AWeaponBase* Weapon);

    UFUNCTION(Server, Reliable)
    void ServerStartFire();

    UFUNCTION(Server, Reliable)
    void ServerStopFire();

    UFUNCTION(Server, Reliable)
    void ServerReload();

    UFUNCTION(Server, Reliable)
    void ServerEquipSlot(EWeaponSlot Slot);

protected:
    virtual void BeginPlay() override;

private:
    AWeaponBase*& GetWeaponSlotRef(EWeaponSlot Slot);
    bool HasWeaponInSlot(EWeaponSlot Slot) const;
    TArray<EWeaponSlot> GetOccupiedSlots() const;
    void AttachWeaponToOwner(AWeaponBase* Weapon);
    void SetWeaponVisible(AWeaponBase* Weapon, bool bVisible) const;

    UPROPERTY()
    AFPSDemoCharacter* OwnerCharacter;

    UPROPERTY()
    AWeaponBase* PrimaryWeapon;

    UPROPERTY()
    AWeaponBase* SecondaryWeapon;

    UPROPERTY()
    AWeaponBase* SpecialWeapon;

    UPROPERTY()
    AWeaponBase* CurrentWeapon;

    UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
    EWeaponSlot CurrentSlot;

    UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
    TMap<EWeaponAmmoType, int32> ReserveAmmoByType;
};
