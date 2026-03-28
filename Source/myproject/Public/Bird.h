// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"   //这一行必须放在最后

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MYPROJECT_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
	//添加输入映射
	void MoveForward(float Value);
	//添加输入鼠标转动视角
	void Turn(float Value);
	void LookUp(float Value);
private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;   //骨骼网格体

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;   //弹簧臂
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;    //摄像机

	
};
