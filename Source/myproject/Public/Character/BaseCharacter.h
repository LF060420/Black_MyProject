// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"


class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class MYPROJECT_API ABaseCharacter : public ACharacter,public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	//设置是否启用盒子碰撞事件
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;

	//攻击事件
	virtual void Attack();

	//定义Death动画蒙太奇函数
	virtual void PlayDeathMontage();

	/*
	* Play Montage Function
	*/

	//播放攻击动画蒙太奇
	virtual void PlayAttackMontage();
	//定义播放HitReact动画蒙太奇函数
	void PlayHitReactMontage(const FName& SectionName);
	//计算受击方向
	void DirectionHitReact(const FVector& ImpactPoint);
	//播放击中音效
	void PlayHitSound(const FVector& ImpactPoint);
	//释放击中特效
	void SpawnHitParticles(const FVector& ImpactPoint);

	virtual void HandleDamage(float DamageAmount);

	//在蓝图中添加Death动画蒙太奇变量
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;        



	//攻击蒙太奇结束通知
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	//检测是否能攻击
	virtual bool CanAttack();

	bool IsAlive();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;       //武器变量

	/**
	* 动画蓝图
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;     //在蓝图中添加Attack动画蒙太奇变量

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;     //在蓝图中添加ReactFrom动画蒙太奇变量



	/*
	组件
	*/
	//添加组件"Attribute"
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAttributeComponent* Attributes;
	
private:

	//添加受击音效
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	//添加受击流血效果
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticle;
};
