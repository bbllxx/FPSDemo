// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

UENUM(BlueprintType)
enum class EZombieType : uint8
{
    Heavy     UMETA(DisplayName = "Heavy Zombie"),
    Light     UMETA(DisplayName = "Light Zombie"),
    Ranged    UMETA(DisplayName = "Ranged Zombie"),
    None      UMETA(DisplayName = "None")
};

UCLASS(Abstract)
class FPSDEMO_API AZombieBase : public ACharacter
{
    GENERATED_BODY()

public:
    AZombieBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    virtual void PerformAttack();

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    bool CanAttack() const;

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    bool IsTargetInAttackRange() const;

    UFUNCTION(BlueprintPure, Category = "Zombie")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Zombie")
    AActor* GetTargetPlayer() const { return TargetPlayer.Get(); }

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    void SetTargetPlayer(AActor* NewTarget) { TargetPlayer = NewTarget; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    EZombieType ZombieType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float MaxHealth;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float CurrentHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float AttackDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float AttackRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Stats")
    float AttackCooldown;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    TWeakObjectPtr<AActor> TargetPlayer;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Zombie AI")
    float LastAttackTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UPawnSensingComponent>  PawnSensingComp;

    UFUNCTION()
    virtual void OnDeath();

    UFUNCTION()
    virtual void OnSeePawn(APawn* Pawn);

    UFUNCTION()
    virtual void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

    UFUNCTION(BlueprintCallable, Category = "Zombie")
    virtual void DealDamageToTarget();

    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie")
    void OnAttack();

    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie")
    void OnTakeDamage(float DamageAmount, FVector HitLocation);

    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie")
    void OnDeathEvent();
};

