#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FPSDemoCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
class USkeletalMeshComponent;
class UWeaponInventoryComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFPSDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** 第一人称手臂网格体，仅自己可见 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** 第一人称相机 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** 跳跃输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** 移动输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** 视角输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** 开火输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	/** 换弹输入动作 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	/** 切换到下一把武器 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* NextWeaponAction;

	/** 切换到上一把武器 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* PreviousWeaponAction;

	/** 装备 1 号武器槽 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* Slot1Action;

	/** 装备 2 号武器槽 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* Slot2Action;

	/** 装备 3 号武器槽 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* Slot3Action;

	/** 玩家武器库存组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess = "true"))
	UWeaponInventoryComponent* WeaponInventory;

public:
	AFPSDemoCharacter();

protected:
	virtual void BeginPlay() override;

	/** 处理移动输入 */
	void Move(const FInputActionValue& Value);

	/** 处理视角输入 */
	void Look(const FInputActionValue& Value);

	/** 开始开火 */
	void StartFire();

	/** 停止开火 */
	void StopFire();

	/** 换弹 */
	void ReloadWeapon();

	/** 装备下一把武器 */
	void EquipNextWeapon();

	/** 装备上一把武器 */
	void EquipPreviousWeapon();

	/** 装备 1 号武器槽 */
	void EquipWeaponSlot1();

	/** 装备 2 号武器槽 */
	void EquipWeaponSlot2();

	/** 装备 3 号武器槽 */
	void EquipWeaponSlot3();

	/** APawn 输入绑定接口 */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** 返回第一人称手臂网格体 */
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** 返回第一人称相机 */
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** 返回武器库存组件 */
	UWeaponInventoryComponent* GetWeaponInventory() const { return WeaponInventory; }
};
