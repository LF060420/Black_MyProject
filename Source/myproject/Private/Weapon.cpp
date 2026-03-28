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

//装备武器函数
void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	//武器设置所有权和触发者
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	//将武器附着在右手插槽上
	AttachMeshToSocket(InParent, InSocketName);

	//将枚举设为装备状态
	ItemState = EItemState::EIS_Equipped;

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

	//拾取武器后取消碰撞检测
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (EmbersEffect)
	{
		//拾取后关闭Niagara特效
		EmbersEffect->Deactivate();
	}
}

//将武器附着在背上
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

//当球体组件开始重叠时
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//调用父类的重叠事件
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

//当球体组件结束重叠时
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//调用父类的重叠事件
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

//当方体组件开始重叠时
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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
	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(
		this,                    // WorldContextObject - 当前世界上下文
		Start,                   // 检测起点（FVector）
		End,                     // 检测终点（FVector）
		FVector(5.f, 5.f, 5.f),   // 盒体半尺寸（5单位立方体盒体）
		BoxTraceStart->GetComponentRotation(), // 盒体的旋转
		ETraceTypeQuery::TraceTypeQuery1, // 检测通道类型
		false,                   // 是否忽略复杂碰撞
		ActorsToIgnore,          // 要忽略的Actor数组
		EDrawDebugTrace::None, // 调试显示方式
		BoxHit,                  // 输出：碰撞结果
		true                     // 是否忽略自己
	);
	if (BoxHit.GetActor())
	{

		//应用伤害
		UGameplayStatics::ApplyDamage
		(
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
		}
		// 将被击中的Actor添加到忽略列表中
		// 避免在同一攻击动作中多次击中同一个目标
		IgnoreActors.AddUnique(BoxHit.GetActor());
	
		//传递击中的位置给引力场
		CreateFields(BoxHit.ImpactPoint);

	}
}
