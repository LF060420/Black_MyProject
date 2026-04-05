#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "myproject/DebugMacros.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ProgressBar.h"
#include "Weapon.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Soul.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// 将网格体碰撞设置为世界动态，能够接收武器碰撞通道，以及自动忽略摄像机碰撞
	// 同时将胶囊体碰撞也设置为忽略摄像机，并启用网格体重叠事件
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);


	// 创建"HUD"用户控件
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// 设置角色移动组件为朝向移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 创建视觉感知组件
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget)
	{
		// 设置初始血条百分比（确保 Attribute 已初始化）
		if (Attributes)
		{
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
		HideHealthBar();//HealthBarWidget->SetVisibility(true);
	}
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	
	//添加武器
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHand_Sword"), this, this);
		EquippedWeapon = DefaultWeapon;
	}

	Tags.Add(FName("Enemy"));
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	if (IsDead()) return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
	
}


void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	// 脱离范围后，敌人恢复巡逻状态
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::DisabledActor()
{
	// 禁用角色所有碰撞体
	SetActorEnableCollision(false);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}



//巡逻目标定时器
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

// 移动到目标处
void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	// 设置AI移动请求
	FAIMoveRequest MoveRequest;
	// 设置目标为PatrolTarget
	MoveRequest.SetGoalActor(Target);
	// 设置到达目标的接受半径（距离目标自动停止）
	MoveRequest.SetAcceptanceRadius(60.f);
	if(!IsAttacking() && !IsEngaged())
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	// 存储有效目标数组
	TArray<AActor*> ValidTargets;
	// 遍历所有巡逻目标
	for (AActor* Target : PatrolTargets)
	{
		// 排除当前巡逻目标
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	// 获取目标数量
	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

// AI看到Pawn
void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));
	
	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsEngaged() &&
		!IsAttacking() &&
		!IsDead();
	if (bCanAttack) UE_LOG(LogTemp, Warning, TEXT("CanAttack"));
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		//设置血量百分比
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

	}
}


//在动画蓝图中被调用的攻击结束函数
void AEnemy::AttackEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Zi::AttackEnd()"));
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

//死亡后掉落灵魂
void AEnemy::SpawnSoul()   
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if(SpawnedSoul) SpawnedSoul->SetSouls(SoulNumber);
		
	}
}   

// 播放Death蒙太奇函数
void AEnemy::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy::Die()"));
	
	if (EquippedWeapon)   //死亡后销毁武器(有bug，会误伤主角)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("EquippedWeapon nullptr"));
	}
	
	Super::Die();
	EnemyState = EEnemyState::EES_Dead;
	if(IsDead())
	DisabledActor();   // 禁用角色所有碰撞体
	
	ClearAttackTimer();
	
	HideHealthBar();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	//死亡后掉落灵魂
	SpawnSoul();
}

//攻击函数
void AEnemy::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemy::Attack()"));

	Super::Attack();
	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}




// 检查目标是否在目标范围内
bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}



//检查巡逻目标
void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	
	// 判断目标是否脱离战斗范围
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
		
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
		
	}
	else if (CanAttack())
	{
		StartAttackTimer();
		UE_LOG(LogTemp, Warning, TEXT("CheckCombatTarget()--CanAttack()"));
	}
}



//死亡后销毁武器
void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}


// 
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Hit"));
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	if(!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	StopAttackMontage();
}


float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	return DamageAmount;
}

  