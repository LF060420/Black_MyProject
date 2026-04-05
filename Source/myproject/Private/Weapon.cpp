// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "SlashCharacter.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	//设置武器的碰撞不检测Pawn类
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//设置开始与结束的两个检测点
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

//当事件开始时
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::PlayEquipSound()
{
	//拾取武器时播放声音
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::DisableSphereCollision()
{
	//拾取武器后取消碰撞检测
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		//拾取后关闭Niagara特效
		ItemEffect->Deactivate();
	}
}

//装备武器函数
void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	//将枚举设为装备状态
	ItemState = EItemState::EIS_Equipped;

	//武器设置所有权和触发者
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);   //设置触发者为玩家角色

	if (NewOwner)
	{
		IgnoreActors.AddUnique(NewOwner);
	}
	AttachMeshToSocket(InParent, InSocketName);   //将武器附着在手上
	PlayEquipSound();   //播放拾取音效
	DisableSphereCollision();   //关闭碰撞检测
	DeactivateEmbers();   //关闭特效
}



//将武器附着在背上
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}


//判断当前武器持有者 和 碰撞到的Actor 是否同为敌人
bool AWeapon::ActorSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

//当方体组件开始重叠时
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("BoxOverlap"));

	if (ActorSameType(OtherActor)) return;
	FHitResult BoxHit;
	BoxTrace(BoxHit);   //获取碰撞结果
	IgnoreActors.AddUnique(BoxHit.GetActor());   // 将被击中的Actor添加到忽略列表中，避免在同一攻击动作中多次击中同一个目标
	if (BoxHit.GetActor())
	{
		if (ActorSameType(BoxHit.GetActor()))
		{
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Apply Damage"));

		//应用伤害
		UGameplayStatics::ApplyDamage
		(
			
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),   //造成伤害的控制器(用于计算归属)
			this,   //伤害源对象(如武器、技能等)
			UDamageType::StaticClass()   //伤害类型(可使用自定义伤害类型)
		);

		Execute_GetHit(BoxHit);

		//传递击中的位置给引力场
		CreateFields(BoxHit.ImpactPoint);

	}
}

void AWeapon::Execute_GetHit(FHitResult& BoxHit)   //传递击中的位置给接口函数
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{

		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}


void AWeapon::BoxTrace(FHitResult& BoxHit)   //获取碰撞结果函数
{
	UE_LOG(LogTemp, Warning, TEXT("BoxTrace"));
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	// 创建一个Actor数组，用于存储需要忽略检测的Actor
	TArray<AActor*>ActorsToIgnore;

	//记录已经击中过的Actor
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);   // AddUnique确保不添加重复的Actor
	}

	// 添加武器自身到忽略列表中，避免检测到自己
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::BoxTraceSingle(
		this,                    // WorldContextObject - 当前世界上下文
		Start,                   // 检测起点（FVector）
		End,                     // 检测终点（FVector）
		BoxTraceExtent,   // 盒体半尺寸（5单位立方体盒体）
		BoxTraceStart->GetComponentRotation(), // 盒体的旋转
		ETraceTypeQuery::TraceTypeQuery1, // 检测通道类型
		false,                   // 是否忽略复杂碰撞
		ActorsToIgnore,          // 要忽略的Actor数组
		bShowBoxDebug ? EDrawDebugTrace::None : EDrawDebugTrace::None, // 调试显示方式
		BoxHit,                  // 输出：碰撞结果
		true                     // 是否忽略自己
	);
}
