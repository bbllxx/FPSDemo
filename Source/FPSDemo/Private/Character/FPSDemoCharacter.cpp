// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/FPSDemoCharacter.h"
#include "Weapon/Projectiles/FPSDemoProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFPSDemoCharacter

AFPSDemoCharacter::AFPSDemoCharacter()
{
	// 设置碰撞胶囊体大小
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// 创建摄像机组件
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // 设置摄像机位置
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// 创建网格体组件，用于第一人称视角（控制此Pawn时可见）
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AFPSDemoCharacter::BeginPlay()
{
	// 调用基类
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////// 输入

void AFPSDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 设置动作绑定
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 跳跃
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// 移动
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSDemoCharacter::Move);

		// 视角
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSDemoCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AFPSDemoCharacter::Move(const FInputActionValue& Value)
{
	// 输入为二维向量
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 添加移动输入
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSDemoCharacter::Look(const FInputActionValue& Value)
{
	// 输入为二维向量
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 添加偏航和俯仰输入到控制器
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
