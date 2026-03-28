// Fill out your copyright notice in the Description page of Project Settings.
#include "items.h"
#include "myproject/DebugMacros.h"
#define THIRTY 30
#include "myproject/myproject.h"
#include "Components/SphereComponent.h"
#include "Slashcharacter.h"


Aitems::Aitems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));  //创建默认子对象,并将其指向指针

	//设置自身碰撞设置
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh; 

	Sphere= CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	//赋值Niagara特效
	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}
// Called when the game starts or when spawned
void Aitems::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();       //获取玩家所在的世界
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &Aitems::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &Aitems::OnSphereEndOverlap);
}
float Aitems::TransformedSin()   //通过游戏时长设置sin值
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}
float Aitems::TransformedCos()   //通过游戏时长设置cos值
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}
// 组件开始重叠
void Aitems::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FString OtherActorName=OtherActor->GetName();
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);
	}
}
//组件结束重叠
void Aitems::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const FString OtherActorName=OtherActor->GetName();
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}

}
void Aitems::Tick(float DeltaTime)   
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;    //计算游戏时长
	//如果武器处于未装备状态，则悬浮在空中
	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

