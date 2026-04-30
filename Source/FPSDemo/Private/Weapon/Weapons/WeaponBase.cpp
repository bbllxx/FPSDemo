#include "Weapon/Weapons/WeaponBase.h"

#include "Camera/CameraComponent.h"
#include "Character/FPSDemoCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Weapon/Components/WeaponInventoryComponent.h"
#include "Weapon/Data/WeaponDataAsset.h"
#include "Weapon/WeaponTypes.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneRoot;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MuzzlePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(WeaponMesh);

    WeaponData = nullptr;
    CurrentMagazineAmmo = 0;
    bReloading = false;
    OwnerCharacter = nullptr;
    OwningInventory = nullptr;
    LastFireTime = -TNumericLimits<float>::Max();
}

void AWeaponBase::SetWeaponData(UWeaponDataAsset* NewWeaponData)
{
    WeaponData = NewWeaponData;
    CurrentMagazineAmmo = GetMagazineCapacity();
    bReloading = false;
}

void AWeaponBase::InitializeWeapon(AFPSDemoCharacter* NewOwnerCharacter, UWeaponInventoryComponent* NewOwningInventory)
{
    OwnerCharacter = NewOwnerCharacter;
    OwningInventory = NewOwningInventory;
    SetOwner(NewOwnerCharacter);
    SetInstigator(NewOwnerCharacter);
}

bool AWeaponBase::StartFire()
{
    if (!WeaponData)
    {
        return false;
    }

    const bool bFired = FireOnce();
    if (bFired && WeaponData->FireMode == EWeaponFireMode::FullAuto && GetWorld())
    {
        const float Interval = FMath::Max(WeaponData->FireInterval, 0.01f);
        GetWorld()->GetTimerManager().SetTimer(AutoFireTimerHandle, this, &AWeaponBase::HandleAutoFire, Interval, true);
    }

    return bFired;
}

void AWeaponBase::HandleAutoFire()
{
    if (!FireOnce())
    {
        StopFire();
    }
}

void AWeaponBase::StopFire()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoFireTimerHandle);
    }
}

bool AWeaponBase::FireOnce()
{
    FVector TraceStart;
    FVector TraceDirection;
    if (!GetOwnerViewpoint(TraceStart, TraceDirection))
    {
        return false;
    }

    return FireFromViewpoint(TraceStart, TraceDirection);
}

bool AWeaponBase::FireFromViewpoint(const FVector& TraceStart, const FVector& TraceDirection)
{
    if (!CanFire())
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    CurrentMagazineAmmo = FMath::Max(CurrentMagazineAmmo - 1, 0);
    LastFireTime = World->GetTimeSeconds();

    if (WeaponData && WeaponData->FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, WeaponData->FireSound, GetActorLocation());
    }

    FHitResult LastHit;
    AActor* LastHitActor = nullptr;
    bool bAnyHeadshot = false;
    const TArray<FVector> ShotDirections = BuildShotDirections(TraceDirection);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WeaponTrace), true);
    QueryParams.AddIgnoredActor(this);
    if (OwnerCharacter)
    {
        QueryParams.AddIgnoredActor(OwnerCharacter);
    }
    if (GetOwner())
    {
        QueryParams.AddIgnoredActor(GetOwner());
    }

    for (const FVector& ShotDirection : ShotDirections)
    {
        const FVector SafeDirection = ShotDirection.GetSafeNormal();
        const FVector TraceEnd = TraceStart + SafeDirection * WeaponData->Range;
        FHitResult Hit;
        if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, FPSDemoWeapon::WeaponTraceChannel, QueryParams))
        {
            LastHit = Hit;
            LastHitActor = Hit.GetActor();
            const bool bHeadshot = IsHeadshotBone(Hit.BoneName);
            bAnyHeadshot |= bHeadshot;
            ApplyDamageToHit(Hit, SafeDirection);
        }
    }

    OnWeaponFired.Broadcast(this, LastHitActor, bAnyHeadshot);
    ReceiveWeaponFired(LastHit, bAnyHeadshot);
    BroadcastAmmoChanged();
    return true;
}

bool AWeaponBase::StartReload()
{
    if (!WeaponData || bReloading || CurrentMagazineAmmo >= GetMagazineCapacity())
    {
        return false;
    }

    bReloading = true;
    OnReloadStarted.Broadcast(this);
    ReceiveReloadStarted();
    return true;
}

bool AWeaponBase::FinishReload()
{
    if (!bReloading)
    {
        return false;
    }

    bReloading = false;
    OnReloadFinished.Broadcast(this);
    ReceiveReloadFinished();
    BroadcastAmmoChanged();
    return true;
}

bool AWeaponBase::CanFire() const
{
    if (!WeaponData || bReloading || CurrentMagazineAmmo <= 0)
    {
        return false;
    }

    const UWorld* World = GetWorld();
    if (!World)
    {
        return true;
    }

    return World->GetTimeSeconds() - LastFireTime >= FMath::Max(WeaponData->FireInterval, 0.0f);
}

void AWeaponBase::SetCurrentMagazineAmmo(int32 NewAmmo)
{
    CurrentMagazineAmmo = FMath::Clamp(NewAmmo, 0, GetMagazineCapacity());
    BroadcastAmmoChanged();
}

int32 AWeaponBase::GetMagazineCapacity() const
{
    return WeaponData ? FMath::Max(WeaponData->MagazineCapacity, 0) : 0;
}

float AWeaponBase::CalculateDamageForBone(FName BoneName) const
{
    if (!WeaponData)
    {
        return 0.0f;
    }

    const float Multiplier = IsHeadshotBone(BoneName) ? FMath::Max(WeaponData->HeadshotMultiplier, 1.0f) : 1.0f;
    return FMath::Max(WeaponData->BaseDamage, 0.0f) * Multiplier;
}

bool AWeaponBase::IsHeadshotBone(FName BoneName)
{
    return BoneName.ToString().Contains(TEXT("head"), ESearchCase::IgnoreCase);
}

TArray<FVector> AWeaponBase::BuildShotDirections(const FVector& AimDirection) const
{
    TArray<FVector> Directions;
    const FVector SafeAimDirection = AimDirection.GetSafeNormal();
    if (SafeAimDirection.IsNearlyZero())
    {
        return Directions;
    }

    const int32 PelletCount = WeaponData ? FMath::Max(WeaponData->PelletsPerShot, 1) : 1;
    Directions.Reserve(PelletCount);

    const float SpreadRadians = WeaponData ? FMath::DegreesToRadians(FMath::Max(WeaponData->SpreadAngleDegrees, 0.0f)) : 0.0f;
    for (int32 Index = 0; Index < PelletCount; ++Index)
    {
        Directions.Add(SpreadRadians > 0.0f ? FMath::VRandCone(SafeAimDirection, SpreadRadians) : SafeAimDirection);
    }

    return Directions;
}

float AWeaponBase::ApplyDamageToHit(const FHitResult& Hit, const FVector& ShotDirection) const
{
    AActor* HitActor = Hit.GetActor();
    if (!HitActor && Hit.GetComponent())
    {
        HitActor = Hit.GetComponent()->GetOwner();
    }
    if (!HitActor)
    {
        return 0.0f;
    }

    AController* InstigatorController = nullptr;
    if (OwnerCharacter)
    {
        InstigatorController = OwnerCharacter->GetController();
    }
    if (!InstigatorController)
    {
        InstigatorController = GetInstigatorController();
    }

    return UGameplayStatics::ApplyPointDamage(
        HitActor,
        CalculateDamageForBone(Hit.BoneName),
        ShotDirection.GetSafeNormal(),
        Hit,
        InstigatorController,
        const_cast<AWeaponBase*>(this),
        nullptr);
}

bool AWeaponBase::GetOwnerViewpoint(FVector& OutStart, FVector& OutDirection) const
{
    if (OwnerCharacter && OwnerCharacter->GetFirstPersonCameraComponent())
    {
        const UCameraComponent* Camera = OwnerCharacter->GetFirstPersonCameraComponent();
        OutStart = Camera->GetComponentLocation();
        OutDirection = Camera->GetForwardVector();
        return true;
    }

    if (MuzzlePoint)
    {
        OutStart = MuzzlePoint->GetComponentLocation();
        OutDirection = MuzzlePoint->GetForwardVector();
        return true;
    }

    return false;
}

void AWeaponBase::BroadcastAmmoChanged()
{
    if (OwningInventory)
    {
        OwningInventory->NotifyWeaponAmmoChanged(this);
    }
}
