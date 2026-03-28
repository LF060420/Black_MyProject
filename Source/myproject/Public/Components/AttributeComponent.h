#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

private:
	//添加当前血量
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float Health;

	//添加最大生命值
	UPROPERTY(EditAnywhere,Category="Actor Attributes")
	float MaxHealth;

public:
	//接受伤害
	void ReceiveDamage(float Damage);
	
	//获取血量百分率
	float GetHealthPercent();

	//判断是否存活
	bool isAlive();
		
};
