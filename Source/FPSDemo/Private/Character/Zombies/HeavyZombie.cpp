// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Zombies/HeavyZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AHeavyZombie::AHeavyZombie()
{
    ZombieType = EZombieType::Heavy;

    MaxHealth = 200.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 25.0f;
    AttackRange = 150.0f;
    AttackCooldown = 2.0f;
    DamageReduction = 0.5f;

    GetCharacterMovement()->MaxWalkSpeed = 200.0f;
    GetCapsuleComponent()->InitCapsuleSize(60.0f, 100.0f);
}

void AHeavyZombie::BeginPlay()
{
    Super::BeginPlay();
}

float AHeavyZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float ReducedDamage = DamageAmount * (1.0f - DamageReduction);
    return Super::TakeDamage(ReducedDamage, DamageEvent, EventInstigator, DamageCauser);
}

