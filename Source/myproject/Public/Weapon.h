// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "items.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

UCLASS()
class MYPROJECT_API AWeapon : public Aitems
{
	GENERATED_BODY()
public:
	//构造函数
	AWeapon();
	
	//将武器附着在手上
	void Equip(USceneComponent* InParent, FName InSocketName,AActor* NewOwner,APawn* NewInstigator);
	
	//将武器附着在背部上
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	//记录被击中的角色
	TArray < AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;

	//当球体组件开始重叠时
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	//当球体组件结束重叠时
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;
	
	//当方体组件开始重叠时
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//添加Field事件，以便在蓝图中使用
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	//拾取武器音效
	UPROPERTY(EditAnywhere,Category="Weapon Properties")
	USoundBase* EquipSound;
	
	//方体碰撞盒子
	UPROPERTY(VisibleAnywhere,Category="Weapon Properties")
	UBoxComponent* WeaponBox;
	
	//开始与结束的检测点
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	//添加变量伤害
	UPROPERTY(EditAnywhere,Category = "Weapon Properties")
	float Damage = 20.f;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
