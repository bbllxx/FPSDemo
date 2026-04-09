// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Zombies/LightZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ALightZombie::ALightZombie()
{
    ZombieType = EZombieType::Light;

    MaxHealth = 50.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 100.0f;
    AttackCooldown = 1.0f;

    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCapsuleComponent()->InitCapsuleSize(40.0f, 90.0f);
}

void ALightZombie::BeginPlay()
{
    Super::BeginPlay();
}

