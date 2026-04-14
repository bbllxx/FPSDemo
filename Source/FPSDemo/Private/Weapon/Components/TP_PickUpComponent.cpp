// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/Components/TP_PickUpComponent.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	// 设置球体碰撞
	SphereRadius = 32.f;
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// 注册重叠事件
	OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
}

void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 检查是否是有第一人称人物重叠
	AFPSDemoCharacter* Character = Cast<AFPSDemoCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// 通知物体被拾取
		OnPickUp.Broadcast(Character);

		// 注销重叠事件，不再触发
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
