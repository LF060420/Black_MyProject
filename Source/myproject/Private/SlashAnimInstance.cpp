// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"
#include "SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();//因为重写，所以调动父版本
	//蓝图节点“cast to the BP_SCharacter”
	SlashCharacter=Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);//因为重写，所以调动父版本
	if (SlashCharacterMovement)
	{
		//获取地面移动速度大小
		GroundSpeed=UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);
		//判断是否下落
		IsFalling = SlashCharacterMovement->IsFalling();
		//获取人物的状态枚举
		CharacterState = SlashCharacter->GetCharacterState();
	}
}
