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
	AEnemy();   //构造函数

	/** <AActor>*/
	virtual void Tick(float DeltaTime) override;   //重写Tick函数
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor>*/


	/** <IHitInterface>*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	/** </IHitInterface>*/

protected:
	//人物巡逻/追逐/攻击状态机
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	/** <AActor>*/
	virtual void BeginPlay() override;
	/** </AActor>*/

	//攻击函数
	virtual void Attack() override;

	virtual bool CanAttack() override;

	virtual void HandleDamage(float DamageAmount) override;

	virtual int32 PlayDeathMontage() override;

	virtual void AttackEnd() override;

	//人物存活/死亡枚举
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	//定义Death动画蒙太奇函数
	virtual void Die() override;




private:

	/* AI行为*/
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
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
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	//AI发现Pawn
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	//生成武器
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	//添加HUD
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	//眼睛组件
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	//记录攻击的对象
	UPROPERTY()
	AActor* CombatTarget;

	//记录血条可视范围
	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	//添加攻击检测范围
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	//Navigation
	UPROPERTY()
	class AAIController* EnemyController;

	//巡逻目标点
	UPROPERTY(EditInstanceOnly, category = "AI Navigation")
	AActor* PatrolTarget;

	//巡逻目标数组
	UPROPERTY(EditInstanceOnly, category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	//巡逻半径
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	//巡逻等待事件范围
	FTimerHandle PatrolTimer;
	
	//等待时长
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 1.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 3.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;


	//攻击事件范围
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 3.f;

};
