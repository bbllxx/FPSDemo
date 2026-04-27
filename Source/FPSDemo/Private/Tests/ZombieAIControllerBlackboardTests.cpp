#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AI/BehaviorTree/BTTask_FindPatrolLocation.h"
#include "AI/BehaviorTree/BTTask_SetState.h"
#include "AI/ZombieAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Name.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/Zombies/LightZombie.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "UObject/UnrealType.h"

namespace FPSDemo::Tests
{
static const FName TargetKeyName(TEXT("Target"));
static const FName StateKeyName(TEXT("State"));
static const FName HomeLocationKeyName(TEXT("HomeLocation"));
static const FName PatrolLocationKeyName(TEXT("PatrolLocation"));

UBlackboardData* CreateZombieBlackboardData()
{
    UBlackboardData* BlackboardData = NewObject<UBlackboardData>();

    UBlackboardKeyType_Object* TargetKeyType = BlackboardData->UpdatePersistentKey<UBlackboardKeyType_Object>(TargetKeyName);
    TargetKeyType->BaseClass = AActor::StaticClass();
    BlackboardData->UpdatePersistentKey<UBlackboardKeyType_Name>(StateKeyName);
    BlackboardData->UpdatePersistentKey<UBlackboardKeyType_Vector>(HomeLocationKeyName);
    BlackboardData->UpdatePersistentKey<UBlackboardKeyType_Vector>(PatrolLocationKeyName);

    BlackboardData->UpdateKeyIDs();
    return BlackboardData;
}

void SetTaskFNameProperty(UObject* Task, const FName PropertyName, const FName Value)
{
    FNameProperty* Property = FindFProperty<FNameProperty>(Task->GetClass(), PropertyName);
    if (Property)
    {
        Property->SetPropertyValue_InContainer(Task, Value);
    }
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FZombieAIControllerBlackboardSyncTest,
    "FPSDemo.AI.ZombieAIController.BlackboardSync",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FZombieAIControllerBlackboardSyncTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }
    World->CreateAISystem();

    AZombieAIController* Controller = World->SpawnActor<AZombieAIController>();
    AActor* TargetActor = World->SpawnActor<AActor>();
    UBlackboardData* BlackboardData = FPSDemo::Tests::CreateZombieBlackboardData();

    UBlackboardComponent* BlackboardComponent = nullptr;
    TestTrue(TEXT("AI 控制器应成功使用测试黑板"), Controller->UseBlackboard(BlackboardData, BlackboardComponent));
    TestNotNull(TEXT("黑板组件应有效"), BlackboardComponent);

    Controller->SetTargetPlayer(TargetActor);
    TestTrue(TEXT("控制器目标应更新"), Controller->GetTargetPlayer() == TargetActor);
    TestTrue(TEXT("黑板 Target 应同步更新"),
        BlackboardComponent->GetValueAsObject(FPSDemo::Tests::TargetKeyName) == TargetActor);

    Controller->SetTargetPlayer(nullptr);
    TestNull(TEXT("控制器目标应清空"), Controller->GetTargetPlayer());
    TestNull(TEXT("黑板 Target 应同步清空"),
        BlackboardComponent->GetValueAsObject(FPSDemo::Tests::TargetKeyName));

    const FName AttackState(TEXT("Attack"));
    Controller->SetCurrentState(AttackState);
    TestEqual(TEXT("黑板 State 应按 Name 类型写入"),
        BlackboardComponent->GetValueAsName(FPSDemo::Tests::StateKeyName), AttackState);

    World->DestroyWorld(false);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTTaskAttackOutOfRangeTest,
    "FPSDemo.AI.BTTask.AttackFailsWhenTargetOutOfRange",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBTTaskAttackOutOfRangeTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }
    World->CreateAISystem();

    AZombieAIController* Controller = World->SpawnActor<AZombieAIController>();
    ALightZombie* Zombie = World->SpawnActor<ALightZombie>(FVector::ZeroVector, FRotator::ZeroRotator);
    ACharacter* TargetActor = World->SpawnActor<ACharacter>(FVector(1000.0f, 0.0f, 0.0f), FRotator::ZeroRotator);

    UBlackboardData* BlackboardData = FPSDemo::Tests::CreateZombieBlackboardData();
    UBlackboardComponent* BlackboardComponent = nullptr;
    TestTrue(TEXT("AI 控制器应成功使用测试黑板"), Controller->UseBlackboard(BlackboardData, BlackboardComponent));

    Controller->Possess(Zombie);
    TestTrue(TEXT("控制 Pawn 后应重新绑定测试黑板"), Controller->UseBlackboard(BlackboardData, BlackboardComponent));
    BlackboardComponent = Controller->GetBlackboardComponent();
    TestNotNull(TEXT("控制器当前黑板组件应有效"), BlackboardComponent);
    TestTrue(TEXT("测试黑板应包含 Target 键"),
        BlackboardComponent->GetKeyID(FPSDemo::Tests::TargetKeyName) != FBlackboard::InvalidKey);
    BlackboardComponent->SetValueAsObject(FPSDemo::Tests::TargetKeyName, TargetActor);
    Zombie->SetTargetPlayer(TargetActor);

    UBehaviorTreeComponent* BehaviorTreeComponent = NewObject<UBehaviorTreeComponent>(Controller);
    Controller->AddOwnedComponent(BehaviorTreeComponent);
    BehaviorTreeComponent->RegisterComponent();
    BehaviorTreeComponent->CacheBlackboardComponent(BlackboardComponent);

    TestTrue(TEXT("行为树组件应绑定测试 AI 控制器"), BehaviorTreeComponent->GetAIOwner() == Controller);
    TestTrue(TEXT("行为树组件应绑定测试黑板"), BehaviorTreeComponent->GetBlackboardComponent() == BlackboardComponent);
    TestTrue(TEXT("AI 控制器应控制测试僵尸"), Controller->GetPawn() == Zombie);
    TestTrue(TEXT("黑板 Target 应持有测试目标"),
        BlackboardComponent->GetValueAsObject(FPSDemo::Tests::TargetKeyName) == TargetActor);
    TestTrue(TEXT("测试僵尸应持有测试目标"), Zombie->GetTargetPlayer() == TargetActor);
    TestTrue(TEXT("测试目标应远离测试僵尸"),
        FVector::Dist(Zombie->GetActorLocation(), TargetActor->GetActorLocation()) > 500.0f);
    TestFalse(TEXT("测试目标应位于攻击范围外"), Zombie->IsTargetInAttackRange());

    UBTTask_Attack* AttackTask = NewObject<UBTTask_Attack>();
    const EBTNodeResult::Type Result = AttackTask->ExecuteTask(*BehaviorTreeComponent, nullptr);
    TestEqual(TEXT("目标不在攻击范围内时 Attack 任务应失败"),
        Result, EBTNodeResult::Failed);

    World->DestroyWorld(false);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTTaskStateWritesNameTest,
    "FPSDemo.AI.BTTask.StateWritesName",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBTTaskStateWritesNameTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }
    World->CreateAISystem();

    AZombieAIController* Controller = World->SpawnActor<AZombieAIController>();
    ALightZombie* Zombie = World->SpawnActor<ALightZombie>(FVector::ZeroVector, FRotator::ZeroRotator);
    ACharacter* TargetActor = World->SpawnActor<ACharacter>(FVector(50.0f, 0.0f, 0.0f), FRotator::ZeroRotator);

    UBlackboardData* BlackboardData = FPSDemo::Tests::CreateZombieBlackboardData();
    UBlackboardComponent* BlackboardComponent = nullptr;
    Controller->Possess(Zombie);
    TestTrue(TEXT("AI 控制器应成功使用测试黑板"), Controller->UseBlackboard(BlackboardData, BlackboardComponent));
    BlackboardComponent->SetValueAsObject(FPSDemo::Tests::TargetKeyName, TargetActor);
    Zombie->SetTargetPlayer(TargetActor);

    UBehaviorTreeComponent* BehaviorTreeComponent = NewObject<UBehaviorTreeComponent>(Controller);
    Controller->AddOwnedComponent(BehaviorTreeComponent);
    BehaviorTreeComponent->RegisterComponent();
    BehaviorTreeComponent->CacheBlackboardComponent(BlackboardComponent);

    UBTTask_Attack* AttackTask = NewObject<UBTTask_Attack>();
    AttackTask->ExecuteTask(*BehaviorTreeComponent, nullptr);
    TestEqual(TEXT("Attack 任务应按 Name 类型写入 State"),
        BlackboardComponent->GetValueAsName(FPSDemo::Tests::StateKeyName), FName(TEXT("Attack")));

    World->DestroyWorld(false);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTTaskSetStateTest,
    "FPSDemo.AI.BTTask.SetStateWritesConfiguredName",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBTTaskSetStateTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }
    World->CreateAISystem();

    AZombieAIController* Controller = World->SpawnActor<AZombieAIController>();
    UBlackboardData* BlackboardData = FPSDemo::Tests::CreateZombieBlackboardData();
    UBlackboardComponent* BlackboardComponent = nullptr;
    TestTrue(TEXT("AI 控制器应成功使用测试黑板"), Controller->UseBlackboard(BlackboardData, BlackboardComponent));

    UBehaviorTreeComponent* BehaviorTreeComponent = NewObject<UBehaviorTreeComponent>(Controller);
    Controller->AddOwnedComponent(BehaviorTreeComponent);
    BehaviorTreeComponent->RegisterComponent();
    BehaviorTreeComponent->CacheBlackboardComponent(BlackboardComponent);

    UBTTask_SetState* SetStateTask = NewObject<UBTTask_SetState>();
    FPSDemo::Tests::SetTaskFNameProperty(SetStateTask, TEXT("StateValue"), TEXT("Chase"));

    const EBTNodeResult::Type Result = SetStateTask->ExecuteTask(*BehaviorTreeComponent, nullptr);
    TestEqual(TEXT("SetState 任务应立即成功"), Result, EBTNodeResult::Succeeded);
    TestEqual(TEXT("SetState 应按配置写入 State"),
        BlackboardComponent->GetValueAsName(FPSDemo::Tests::StateKeyName), FName(TEXT("Chase")));

    World->DestroyWorld(false);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBTTaskFindPatrolLocationTest,
    "FPSDemo.AI.BTTask.FindPatrolLocationWritesBlackboardVector",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBTTaskFindPatrolLocationTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("应创建测试世界"), World))
    {
        return false;
    }
    World->CreateAISystem();

    AZombieAIController* Controller = World->SpawnActor<AZombieAIController>();
    ALightZombie* Zombie = World->SpawnActor<ALightZombie>(FVector::ZeroVector, FRotator::ZeroRotator);
    Controller->Possess(Zombie);

    UBlackboardData* BlackboardData = FPSDemo::Tests::CreateZombieBlackboardData();
    UBlackboardComponent* BlackboardComponent = nullptr;
    TestTrue(TEXT("AI 控制器应成功使用测试黑板"), Controller->UseBlackboard(BlackboardData, BlackboardComponent));

    const FVector HomeLocation(300.0f, 400.0f, 0.0f);
    BlackboardComponent->SetValueAsVector(FPSDemo::Tests::HomeLocationKeyName, HomeLocation);

    UBehaviorTreeComponent* BehaviorTreeComponent = NewObject<UBehaviorTreeComponent>(Controller);
    Controller->AddOwnedComponent(BehaviorTreeComponent);
    BehaviorTreeComponent->RegisterComponent();
    BehaviorTreeComponent->CacheBlackboardComponent(BlackboardComponent);

    UBTTask_FindPatrolLocation* FindLocationTask = NewObject<UBTTask_FindPatrolLocation>();
    const EBTNodeResult::Type FindResult = FindLocationTask->ExecuteTask(*BehaviorTreeComponent, nullptr);
    TestEqual(TEXT("FindPatrolLocation 应只写巡逻点并立即成功"), FindResult, EBTNodeResult::Succeeded);
    TestEqual(TEXT("无导航数据时应回退写入 HomeLocation"),
        BlackboardComponent->GetValueAsVector(FPSDemo::Tests::PatrolLocationKeyName), HomeLocation);

    World->DestroyWorld(false);
    return true;
}

#endif
