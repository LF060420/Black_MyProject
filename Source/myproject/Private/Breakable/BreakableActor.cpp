#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Treasure.h"
#include "Components/CapsuleComponent.h"

ABreakableActor::ABreakableActor()
{
	//"false"使不需要每帧执行逻辑
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	//让物品生成重叠事件
	GeometryCollection->SetGenerateOverlapEvents(true);
	//创建胶囊体碰撞组件
	Capsule=CreateDefaultSubobject < UCapsuleComponent >(TEXT("Capsule"));
	// 将胶囊体组件附加到根组件（GeometryCollection）
	Capsule->SetupAttachment(GetRootComponent()); 
	//使胶囊体除人物外碰撞属于Ignore状态
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	
}

//接口[命中]事件
void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	//如果变量"broken"是true，则Actor是破碎的
	if (broken)
	{
		return;
	}
	broken = true;
	//生成Treasure
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		//获取Actor自身位置
		FVector Location = GetActorLocation();
		//生成随机数
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		//根据随机数实现"SpawnActor"事件生成Treasure
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}
	//将胶囊体对人物的阻挡改成Ignore
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//设置2s后整个Actor消失在世界中
	SetLifeSpan(2.0f);
   
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

