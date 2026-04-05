// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class MYPROJECT_API ABreakableActor : public AActor , public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	//重写接口命中事件
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	//添加变量[broken],放置出现循环bug
	UPROPERTY(BlueprintReadWrite,Category="Breakable")
	bool broken = false;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	//添加胶囊使人物无法穿过物品
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

private:
	//新建"TreasureClass"(数组类型)以选择生成的物品
	UPROPERTY(EditAnywhere,Category="Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;      //"TSubclassOf"特殊指定类型
	   
};
