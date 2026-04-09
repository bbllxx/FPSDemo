// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Zombies/RangedZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARangedZombie::ARangedZombie()
{
    ZombieType = EZombieType::Ranged;

    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 15.0f;
    AttackRange = 2000.0f;
    AttackCooldown = 2.0f;

    GetCharacterMovement()->MaxWalkSpeed = 350.0f;

    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(GetMesh());
    MuzzleLocation->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

    IdealAttackRange = 1500.0f;
    ProjectileSpeed = 2000.0f;
}

void ARangedZombie::BeginPlay()
{
    Super::BeginPlay();
}

void ARangedZombie::PerformAttack()
{
    if (!CanAttack())
    {
        return;
    }

    LastAttackTime = GetWorld()->GetTimeSeconds();
    OnAttack();
    FireProjectile();
}

void ARangedZombie::FireProjectile()
{
    if (!ProjectileClass)
    {
        return;
    }

    if (!TargetPlayer.IsValid())
    {
        return;
    }

    FVector SpawnLocation = MuzzleLocation ? MuzzleLocation->GetComponentLocation() : GetActorLocation() + GetActorForwardVector() * 50.0f;

    FVector TargetLocation = TargetPlayer.Get()->GetActorLocation();
    FVector Direction = TargetLocation - SpawnLocation;
    Direction.Normalize();
    FRotator SpawnRotation = Direction.Rotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (Projectile)
    {
        UProjectileMovementComponent* ProjMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>();
        if (ProjMovement)
        {
            ProjMovement->InitialSpeed = ProjectileSpeed;
            ProjMovement->MaxSpeed = ProjectileSpeed;
            ProjMovement->Velocity = Direction * ProjectileSpeed;
        }
    }
}

void ARangedZombie::DealDamageToTarget()
{
}

bool ARangedZombie::ShouldMoveCloser() const
{
    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer.Get()->GetActorLocation());
    return Distance > IdealAttackRange + 200.0f;
}

bool ARangedZombie::ShouldMoveAway() const
{
    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer.Get()->GetActorLocation());
    return Distance < IdealAttackRange - 200.0f;
}

