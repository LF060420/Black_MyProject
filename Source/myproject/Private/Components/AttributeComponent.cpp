#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

//获取伤害值
void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, Health);   //Clamp限制范围
	UE_LOG(LogTemp, Warning, TEXT("Health reduced to: %.2f"), Health);
}

//获取血量百分比
float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

//判断敌人是否死亡
bool UAttributeComponent::isAlive()
{
	return Health>0.f;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

