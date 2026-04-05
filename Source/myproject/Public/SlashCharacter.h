#pragma once

#include "CoreMinimal.h"
#include "items.h"
#include "Character/BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickUpInterface.h"
#include "Interfaces/HitInterface.h"
#include "SlashCharacter.generated.h"   //这一行必须放在最后


class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;   //为了添加眉毛和头发
class Aitems;       //添加变量以承载装备
class ASoul;
class ATreasure;
class UAnimMontage;    //动画蒙太奇类
class USlashOverlay;

UCLASS()
class MYPROJECT_API ASlashCharacter : public ABaseCharacter,public IPickUpInterface
{
	GENERATED_BODY() 

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(Aitems* Item) override;   //实现接口函数，设置当前重叠的物品
	virtual void AddSouls(ASoul* Soul) override;   //实现接口函数，添加Souls
	virtual void AddGold(ATreasure* Treasure) override;

protected:

	virtual void BeginPlay() override;
	
	/* 按键输入*/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void QKeyPressed();
	virtual void Jump() override;
	void Dodge();
	/* /按键输入*/
	virtual void DodgeEnd() override;
	
	
	//添加攻击输入函数
	virtual void Attack() override;
	
	//攻击动画蒙太奇结束播放通知
	virtual void AttackEnd() override;
	
	//判断是否能够攻击
	virtual bool CanAttack() override;
	
	//装备/放下武器动画蒙太奇通知
	void PlayEquipMontage(const FName& SectionName);

	//Die函数
	virtual void Die() override;
	
	//判断是否能够放下武器
	bool CanDisarm();
	
	//判断是否能装备武器
	bool CanArm();

	//定义将武器放置背后函数
	UFUNCTION(BlueprintCallable)
	void Disarm();
	
	//定义从背后取出武器函数
	UFUNCTION(BlueprintCallable)
	void Arm();
	
	//卸装/装备结束后重置默认值
	UFUNCTION(BlueprintCallable)
	void FinishEquip();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();


private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;   //初始化人物装备状态枚举
	UPROPERTY(BlueprintReadwrite,meta=(AllowPrivateAccess="true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;    //初始化人物的状态枚举

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;   //弹簧臂

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;    //摄像机
	
	UPROPERTY(VisibleAnywhere,Category=Hair)
	UGroomComponent* Hair;     //头发

	UPROPERTY(VisibleAnywhere,Category=Hair)
	UGroomComponent* Eyebrows;    //眉毛

	UPROPERTY(VisibleInstanceOnly)
	Aitems* OverlappingItem;     //武器


	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;     //在蓝图中添加Equip蒙太奇变量

	void InitializeSlashOverlay();

	void SetHUDHealth();

	UPROPERTY()
	USlashOverlay* SlashOverlay;
public:
	//FORCEINLINE void SetOverlappingItem(Aitems* Item) { OverlappingItem = Item; };   //将武器赋值给Item
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }   //状态机
	FORCEINLINE EActionState GetActionState() const { return ActionState; }   //返回动画蓝图中使用的状态机
};
