#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponPickup.generated.h"

class AWeaponBase;
class USphereComponent;

UCLASS(Blueprintable)
class FPSDEMO_API AWeaponPickup : public AActor
{
    GENERATED_BODY()

public:
    AWeaponPickup();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* PickupSphere;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnPickupSphereBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};
