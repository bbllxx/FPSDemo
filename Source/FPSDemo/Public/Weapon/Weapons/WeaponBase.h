#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class AFPSDemoCharacter;
class AWeaponBase;
class UArrowComponent;
class USkeletalMeshComponent;
class UWeaponDataAsset;
class UWeaponInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponFiredSignature, AWeaponBase*, Weapon, AActor*, HitActor, bool, bHeadshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponReloadSignature, AWeaponBase*, Weapon);

UCLASS(Blueprintable)
class FPSDEMO_API AWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWeaponBase();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UArrowComponent* MuzzlePoint;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    UWeaponDataAsset* WeaponData;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FWeaponFiredSignature OnWeaponFired;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FWeaponReloadSignature OnReloadStarted;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FWeaponReloadSignature OnReloadFinished;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetWeaponData(UWeaponDataAsset* NewWeaponData);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    UWeaponDataAsset* GetWeaponData() const { return WeaponData; }

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void InitializeWeapon(AFPSDemoCharacter* NewOwnerCharacter, UWeaponInventoryComponent* NewOwningInventory);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool StartFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StopFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool FireOnce();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool FireFromViewpoint(const FVector& TraceStart, const FVector& TraceDirection);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool StartReload();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool FinishReload();

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool CanFire() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsReloading() const { return bReloading; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetCurrentMagazineAmmo() const { return CurrentMagazineAmmo; }

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetCurrentMagazineAmmo(int32 NewAmmo);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetMagazineCapacity() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    float CalculateDamageForBone(FName BoneName) const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    static bool IsHeadshotBone(FName BoneName);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    TArray<FVector> BuildShotDirections(const FVector& AimDirection) const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    float ApplyDamageToHit(const FHitResult& Hit, const FVector& ShotDirection) const;

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void ReceiveWeaponFired(const FHitResult& Hit, bool bHeadshot);

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void ReceiveReloadStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void ReceiveReloadFinished();

private:
    void HandleAutoFire();
    bool GetOwnerViewpoint(FVector& OutStart, FVector& OutDirection) const;
    void BroadcastAmmoChanged();

    UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
    int32 CurrentMagazineAmmo;

    UPROPERTY(VisibleInstanceOnly, Category = "Weapon")
    bool bReloading;

    UPROPERTY()
    AFPSDemoCharacter* OwnerCharacter;

    UPROPERTY()
    UWeaponInventoryComponent* OwningInventory;

    FTimerHandle AutoFireTimerHandle;
    float LastFireTime;
};
