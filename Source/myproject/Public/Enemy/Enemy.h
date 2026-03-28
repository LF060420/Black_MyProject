#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "CharacterTypes.h"
#include "Enemy.generated.h"


class UHealthBarComponent;
class UPawnSensingComponent;   //视觉组件，相当于加上眼睛

UCLASS()
class MYPROJECT_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();

	//死亡后销毁手中的武器
	virtual void Destroyed() override;
	
	//添加"Implementation"设为C++版本使用
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;     //重写被击中函数

	

	//重写应用伤害事件
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	
	//生成武器
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	//添加HUD
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	//眼睛组件
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;


	/**
	* 动画蓝图
	*/


	//记录攻击的对象
	UPROPERTY()
	AActor* CombatTarget;

	//记录血条可视范围
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	//添加攻击检测范围
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	//Navigation
	UPROPERTY()
	class AAIController* EnemyController;

	//巡逻目标点
	UPROPERTY(EditInstanceOnly,category="AI Navigation")
	AActor* PatrolTarget;

	//巡逻目标数组
	UPROPERTY(EditInstanceOnly, category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	//巡逻半径
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 500.f;

	//设置定时器移动
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	//移动函数
	void MoveToTarget(AActor* Target);

	//选择目标
	AActor* ChoosePatrolTarget();

	//AI发现Pawn
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	//等待时长
	UPROPERTY(EditAnywhere,Category="AI Navigation")
	float WaitMin = 1.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 3.f;

	
	/*
	* AI Behavior
	*/
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void DisabledActor();

	/*
	* Combat
	*/
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere,Category="Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.0f;

	UPROPERTY(EditAnywhere,Category="Combat")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 400.f;

	UPROPERTY(EditAnywhere,Category="Combat")
	float DeathLifeSpan = 3.f;

protected:

	//人物巡逻/追逐/攻击状态机
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;


	//重写BeginPlay事件
	virtual void BeginPlay() override;


	//检测目标是否在范围之内
	bool InTargetRange(AActor* Target,double Radius);

	//人物存活/死亡枚举
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	//定义Death动画蒙太奇函数
	virtual void Die() override;

	//攻击函数
	virtual void Attack() override;

	virtual bool CanAttack() override;

	virtual void HandleDamage(float DamageAmount) override;

	virtual int32 PlayDeathMontage() override;
};
