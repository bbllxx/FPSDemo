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

    // 从指定视角执行射线开火，主要供玩家相机、测试或非标准持有者复用。
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

    // 按武器配置生成本次开火的弹道方向；霰弹枪会返回多条弹丸方向。
    UFUNCTION(BlueprintPure, Category = "Weapon")
    TArray<FVector> BuildShotDirections(const FVector& AimDirection) const;

    // 根据命中骨骼计算最终伤害，并把点伤害事件发送给受击对象。
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
