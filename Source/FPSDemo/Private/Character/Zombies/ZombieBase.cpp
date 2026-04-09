// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Zombies/ZombieBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AZombieBase::AZombieBase()
{
    PrimaryActorTick.bCanEverTick = true;

    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 10.0f;
    AttackRange = 150.0f;
    AttackCooldown = 1.5f;
    ZombieType = EZombieType::None;
    TargetPlayer = nullptr;
    LastAttackTime = -AttackCooldown;

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SetPeripheralVisionAngle(120.0f);
    PawnSensingComp->SightRadius = 2000.0f;
    PawnSensingComp->HearingThreshold = 1000.0f;
    PawnSensingComp->LOSHearingThreshold = 1500.0f;
    PawnSensingComp->bSeePawns = true;
    PawnSensingComp->bHearNoises = true;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AZombieBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    if (PawnSensingComp)
    {
        PawnSensingComp->OnSeePawn.AddDynamic(this, &AZombieBase::OnSeePawn);
        PawnSensingComp->OnHearNoise.AddDynamic(this, &AZombieBase::OnHearNoise);
    }
}

void AZombieBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AZombieBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AZombieBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.0f)
    {
        CurrentHealth -= ActualDamage;

        FVector HitLocation = GetActorLocation();
        if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
        {
            FPointDamageEvent* PointDamage = (FPointDamageEvent*)&DamageEvent;
            HitLocation = PointDamage->HitInfo.ImpactPoint;
        }

        OnTakeDamage(ActualDamage, HitLocation);

        if (CurrentHealth <= 0.0f)
        {
            OnDeath();
        }
    }

    return ActualDamage;
}

void AZombieBase::OnDeath()
{
    OnDeathEvent();

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(3.0f);
}

void AZombieBase::OnSeePawn(APawn* Pawn)
{
    if (Pawn && Pawn->IsPlayerControlled())
    {
        TargetPlayer = Pawn;
    }
}

void AZombieBase::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
    if (!TargetPlayer.IsValid() && PawnInstigator && PawnInstigator->IsPlayerControlled())
    {
        TargetPlayer = PawnInstigator;
    }
}

bool AZombieBase::CanAttack() const
{
    return (GetWorld()->GetTimeSeconds() - LastAttackTime) >= AttackCooldown;
}

bool AZombieBase::IsTargetInAttackRange() const
{
    if (!TargetPlayer.IsValid())
    {
        return false;
    }

    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer.Get()->GetActorLocation());
    return Distance <= AttackRange;
}

void AZombieBase::PerformAttack()
{
    if (!CanAttack())
    {
        return;
    }

    LastAttackTime = GetWorld()->GetTimeSeconds();
    OnAttack();
    DealDamageToTarget();
}

void AZombieBase::DealDamageToTarget()
{
    if (!TargetPlayer.IsValid())
    {
        return;
    }

    if (!IsTargetInAttackRange())
    {
        return;
    }

    UGameplayStatics::ApplyDamage(TargetPlayer.Get(), AttackDamage, GetController(), this, nullptr);
}

