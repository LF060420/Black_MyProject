// Fill out your copyright notice in the Description page of Project Settings.


#include "Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// 初始化各种组件和基本属性
ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(15.f);    //设置胶囊默认半高
	Capsule->SetCapsuleRadius(15.f);     //设置胶囊默认半径
	SetRootComponent(Capsule);
	//添加组件[骨骼网格体]
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));    
	BirdMesh->SetupAttachment(GetRootComponent());     
	//添加组件[弹簧臂]
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));   
	CameraBoom->SetupAttachment(GetRootComponent());    
	CameraBoom->TargetArmLength = 300.f;
	//添加组件[摄像机]
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));      
	ViewCamera->SetupAttachment(CameraBoom);

	//设置自动控制玩家
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}
//当事件开始执行
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}
//设置玩家输入向前
void ABird::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
}
//设置玩家鼠标转动视角
void ABird::Turn(float Value)
{
	AddControllerYawInput(Value);
}
void ABird::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
//每一帧运行的事件
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 用于将功能绑定到输入（操作）
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//绑定前进的输入映射
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
	//添加鼠标转动视角的输入映射
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ABird::LookUp);
}

