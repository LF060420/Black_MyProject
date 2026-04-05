#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"   //方体碰撞盒子
#include "Components/AttributeComponent.h"
#include <Kismet/GameplayStatics.h>
#include <HUD/HealthBarComponent.h>


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 创建"Attribute"组件
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	UE_LOG(LogTemp, Warning, TEXT("BaseCharacter Hit"));

	if (IsAlive() && Hitter)
	{
		DirectionHitReact(Hitter->GetActorLocation());
	}
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();        //清空武器的忽略Actor列表
	}
}

//攻击事件
void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

//检测是否能攻击
bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->isAlive();
}

//攻击蒙太奇结束通知
void ABaseCharacter::AttackEnd()
{

}



//播放death蒙太奇
void ABaseCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("BaseCharacter::Die()"));

	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

//计算受击方向
void ABaseCharacter::DirectionHitReact(const FVector& ImpactPoint)
{
	// ---------- 计算被击方向 ----------

	// 获取敌人的面朝前方向
	const FVector Forward = GetActorForwardVector();

	// 将击中点的Z坐标设置为敌人的Z坐标，使计算在水平面上进行
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	// 计算从敌人位置到击中点的方向向量，并归一化
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// 计算前向向量与方向向量的点积，得到两向量夹角的余弦值
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// 通过反余弦函数得到夹角的弧度值
	double Theta = FMath::Acos(CosTheta);

	// 将弧度转换为角度，0-180度范围
	Theta = FMath::RadiansToDegrees(Theta);

	// ---------- 判断左右方向 ----------

	// 计算前向向量与击中方向向量的叉积
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	// 如果叉积Z分量为负，表示敌人在右侧（UE4中Z轴向上）
	// 需要将角度设为负值表示右侧
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	// ---------- 根据角度确定受击动画片段 ----------

	// 默认受击动画为后
	FName SectionName("ReactBack");

	// 前：角度在[-45°, 45°)范围内
	if (Theta >= -45.f && Theta < 45.f)
	{
		SectionName = FName("ReactFront");
	}
	// 左：角度在[-135°, -45°)范围内
	else if (Theta >= -135.f && Theta < -45.f)
	{
		SectionName = FName("ReactLeft");
	}
	// 右：角度在[45°, 135°)范围内
	else if (Theta >= 45.f && Theta < 135.f)
	{
		SectionName = FName("ReactRight");
	}
	// 后面：[135°, 180°] 和 [-180°, -135°)，使用默认的ReactBack

	// ---------- 播放对应的受击反应动画 ----------

	// 播放受击反应蒙太奇，使用计算得到的方向片段
	PlayHitReactMontage(SectionName);
}

//播放受击音效
void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	// 受击时播放音效
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

//释放受击特效
void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	// 受击时释放溅血特效
	if (HitParticle && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(),
			HitParticle,
			ImpactPoint
		);
	}
}

//停止播放Attack蒙太奇
void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::PlayDodgeMontage()
{
	if(DodgeMontage) PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::DodgeEnd()
{
}

// 获取战斗目标后方固定距离的平移对齐目标
FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(CombatTarget==nullptr) return FVector();
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;
	return CombatTargetLocation + TargetToMe;
}

// 获取旋转对齐所用的目标位置（看向敌人）
FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}



void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		const bool bWasAlive = Attributes->isAlive();

		Attributes->ReceiveDamage(DamageAmount);

		if (bWasAlive && !Attributes->isAlive())
		{
			Die();
		}
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage,const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);

}

int32 ABaseCharacter::PlayDeathMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::PlayDeathMontage"));

	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_Max)
	{
		DeathPose = Pose;

	}
	return Selection;
}

//播放受击蒙太奇
void ABaseCharacter::PlayHitReactMontage(const FName & SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}
