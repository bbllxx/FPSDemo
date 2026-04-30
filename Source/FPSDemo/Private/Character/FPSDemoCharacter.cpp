#include "Character/FPSDemoCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Weapon/Components/WeaponInventoryComponent.h"
#include "Weapon/WeaponTypes.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AFPSDemoCharacter::AFPSDemoCharacter()
{
	// 设置碰撞胶囊体大小。
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// 创建第一人称相机。
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// 创建第一人称手臂网格体。
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// 创建武器库存组件。
	WeaponInventory = CreateDefaultSubobject<UWeaponInventoryComponent>(TEXT("WeaponInventory"));
}

void AFPSDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponInventory)
	{
		WeaponInventory->InitializeInventory(this);
	}
}

void AFPSDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSDemoCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSDemoCharacter::Look);

		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFPSDemoCharacter::StartFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AFPSDemoCharacter::StopFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &AFPSDemoCharacter::StopFire);
		}

		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AFPSDemoCharacter::ReloadWeapon);
		}

		if (NextWeaponAction)
		{
			EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Started, this, &AFPSDemoCharacter::EquipNextWeapon);
		}

		if (PreviousWeaponAction)
		{
			EnhancedInputComponent->BindAction(PreviousWeaponAction, ETriggerEvent::Started, this, &AFPSDemoCharacter::EquipPreviousWeapon);
		}

		if (Slot1Action)
		{
			EnhancedInputComponent->BindAction(Slot1Action, ETriggerEvent::Started, this, &AFPSDemoCharacter::EquipWeaponSlot1);
		}

		if (Slot2Action)
		{
			EnhancedInputComponent->BindAction(Slot2Action, ETriggerEvent::Started, this, &AFPSDemoCharacter::EquipWeaponSlot2);
		}

		if (Slot3Action)
		{
			EnhancedInputComponent->BindAction(Slot3Action, ETriggerEvent::Started, this, &AFPSDemoCharacter::EquipWeaponSlot3);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("未找到 Enhanced Input Component，请确认角色输入组件使用 Enhanced Input。"));
	}
}

void AFPSDemoCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSDemoCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSDemoCharacter::StartFire()
{
	if (WeaponInventory)
	{
		WeaponInventory->StartFire();
	}
}

void AFPSDemoCharacter::StopFire()
{
	if (WeaponInventory)
	{
		WeaponInventory->StopFire();
	}
}

void AFPSDemoCharacter::ReloadWeapon()
{
	if (WeaponInventory)
	{
		WeaponInventory->ReloadCurrentWeapon();
	}
}

void AFPSDemoCharacter::EquipNextWeapon()
{
	if (WeaponInventory)
	{
		WeaponInventory->EquipNextWeapon();
	}
}

void AFPSDemoCharacter::EquipPreviousWeapon()
{
	if (WeaponInventory)
	{
		WeaponInventory->EquipPreviousWeapon();
	}
}

void AFPSDemoCharacter::EquipWeaponSlot1()
{
	if (WeaponInventory)
	{
		WeaponInventory->EquipWeaponSlot(EWeaponSlot::Primary);
	}
}

void AFPSDemoCharacter::EquipWeaponSlot2()
{
	if (WeaponInventory)
	{
		WeaponInventory->EquipWeaponSlot(EWeaponSlot::Secondary);
	}
}

void AFPSDemoCharacter::EquipWeaponSlot3()
{
	if (WeaponInventory)
	{
		WeaponInventory->EquipWeaponSlot(EWeaponSlot::Special);
	}
}
