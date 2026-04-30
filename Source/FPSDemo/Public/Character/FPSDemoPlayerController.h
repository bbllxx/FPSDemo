#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSDemoPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class FPSDEMO_API AFPSDemoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** 默认移动和视角输入映射 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputMappingContext* InputMappingContext;

	/** 武器输入映射，蓝图中配置开火、换弹和切枪输入 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input)
	UInputMappingContext* WeaponInputMappingContext;

	virtual void BeginPlay() override;
};
