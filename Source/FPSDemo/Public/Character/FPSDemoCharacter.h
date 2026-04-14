// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FPSDemoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFPSDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn网格体：第一人称视角（手臂；仅自己可见） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** 第一人称摄像机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** 跳跃输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** 移动输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

public:
	AFPSDemoCharacter();

protected:
	virtual void BeginPlay() override;

public:

	/** 视角输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

protected:
	/** 处理移动输入 */
	void Move(const FInputActionValue& Value);

	/** 处理视角输入 */
	void Look(const FInputActionValue& Value);

protected:
	// APawn接口
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// APawn接口结束

public:
	/** 返回Mesh1P子对象 **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** 返回FirstPersonCameraComponent子对象 **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
