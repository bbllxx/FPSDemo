#include "Weapon/Pickups/WeaponPickup.h"

#include "Character/FPSDemoCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Weapon/Components/WeaponInventoryComponent.h"
#include "Weapon/Weapons/WeaponBase.h"

AWeaponPickup::AWeaponPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    PickupSphere->InitSphereRadius(64.0f);
    PickupSphere->SetCollisionProfileName(TEXT("Trigger"));
    RootComponent = PickupSphere;

    WeaponClass = nullptr;
}

void AWeaponPickup::BeginPlay()
{
    Super::BeginPlay();
    PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickup::OnPickupSphereBeginOverlap);
}

void AWeaponPickup::OnPickupSphereBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AFPSDemoCharacter* Character = Cast<AFPSDemoCharacter>(OtherActor);
    if (!Character || !WeaponClass)
    {
        return;
    }

    UWeaponInventoryComponent* Inventory = Character->FindComponentByClass<UWeaponInventoryComponent>();
    if (!Inventory)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Character;
    SpawnParams.Instigator = Character;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 拾取物只是世界中的触发器，真正装备的是新生成的武器实例。
    AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetActorTransform(), SpawnParams);
    if (Weapon && Inventory->AddWeapon(Weapon))
    {
        PickupSphere->OnComponentBeginOverlap.RemoveAll(this);
        Destroy();
    }
    else if (Weapon)
    {
        Weapon->Destroy();
    }
}
