// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Character/FPSDemoCharacter.h"
#include "TP_PickUpComponent.generated.h"

// 委托声明：当有人拾取此物品时会被调用
// 拾取的人物作为参数传递
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, AFPSDemoCharacter*, PickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPSDEMO_API UTP_PickUpComponent : public USphereComponent
{
	GENERATED_BODY()

public:

	/** 任何人都可以订阅以接收此事件的委托 */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPickUp OnPickUp;

	UTP_PickUpComponent();
protected:

	/** 游戏开始时调用 */
	virtual void BeginPlay() override;

	/** 当有物体与此组件重叠时的处理代码 */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
