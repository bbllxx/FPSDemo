// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AFPSDemoCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSDEMO_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** 投射物类 */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AFPSDemoProjectile> ProjectileClass;

	/** 每次开火时播放的声音 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** 每次开火时播放的动画Montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** 枪口相对于人物位置的偏移 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** 输入映射上下文 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** 开火输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** 设置此组件的默认值 */
	UTP_WeaponComponent();

	/** 将武器附加到第一人称人物 */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AFPSDemoCharacter* TargetCharacter);

	/** 武器开火，发射投射物 */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

protected:
	/** 结束此组件的游戏逻辑 */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** 持有此武器的人物 */
	AFPSDemoCharacter* Character;
};
