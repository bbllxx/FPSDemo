#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Character/Zombies/LightZombie.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FZombieAttackKnockbackLaunchesTargetTest,
    "FPSDemo.AI.ZombieAttack.KnockbackLaunchesTargetOnCommittedHit",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FZombieAttackKnockbackLaunchesTargetTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }

    ALightZombie* Zombie = World->SpawnActor<ALightZombie>(FVector::ZeroVector, FRotator::ZeroRotator);
    ACharacter* Target = World->SpawnActor<ACharacter>(FVector(100.0f, 0.0f, 0.0f), FRotator::ZeroRotator);

    TestNotNull(TEXT("应生成测试僵尸"), Zombie);
    TestNotNull(TEXT("应生成测试玩家角色"), Target);
    if (!Zombie || !Target)
    {
        World->DestroyWorld(false);
        return false;
    }

    Target->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    Zombie->SetTargetPlayer(Target);
    TestTrue(TEXT("目标应位于攻击范围内"), Zombie->IsTargetInAttackRange());
    TestTrue(TEXT("僵尸应成功发起攻击"), Zombie->TryStartAttack());

    Zombie->CommitAttackDamage();

    const FVector PendingLaunchVelocity = Target->GetCharacterMovement()->PendingLaunchVelocity;
    TestTrue(TEXT("玩家应收到水平击退速度"), PendingLaunchVelocity.X > 0.0f);
    TestTrue(TEXT("击退方向应沿僵尸指向玩家的水平方向"), FMath::IsNearlyZero(PendingLaunchVelocity.Y));
    TestTrue(TEXT("击退不应包含向上速度"), FMath::IsNearlyZero(PendingLaunchVelocity.Z));

    World->DestroyWorld(false);
    return true;
}

#endif
