// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "items.generated.h"


class USphereComponent;

//添加枚举，判断武器是否被拿起
enum class EItemState :uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class MYPROJECT_API Aitems : public AActor
{
	GENERATED_BODY()
	
public:	
	Aitems();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Sine Parameters")   //仅编辑默认值   //括号内可改为“EditDefaultsOnly”
	float Amplitude=5.f;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Sine Parameters")   //仅编辑实例
	float TimeConstant = 6.f;

    UPROPERTY(VisibleAnywhere,BlueprintReadWrite)   //VisibleInstanceOnly  /   VisibleDefaultsOnly
	float RunningTime;

    UFUNCTION(BlueprintPure)    //BlueprintCallable蓝色函数
    float TransformedSin();   //构建sin函数

    UFUNCTION(BlueprintPure)   //构建cos函数
    float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);
	//当组件开始重叠时
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//当组件结束重叠时 
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;   //空指针，需要将其暴露给蓝图和反射系统
	
	//设置枚举默认值为Hovering
	EItemState ItemState=EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;    //新建碰撞球体类 

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* EmbersEffect;      //新建Niagara特效类

};

template<typename T>
inline T Aitems::Avg(T First, T Second)
{
	return (First+Second)/2;
}
