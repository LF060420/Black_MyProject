// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "SlashAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	//新建变量
	UPROPERTY(BlueprintReadOnly)
	class ASlashCharacter* SlashCharacter;
	UPROPERTY(BlueprintReadOnly,Category=Movement)   //角色
	class UCharacterMovementComponent* SlashCharacterMovement;    //角色移动组件
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;               //移动速度
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;                 //是否下落
	UPROPERTY(BlueprintReadOnly,Category="Movement | Character State")
	ECharacterState CharacterState;         //状态枚举
};
