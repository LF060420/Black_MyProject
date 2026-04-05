#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//设置默认生命值
	MaxHealth = 100.f;
	Health = MaxHealth;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

//获取伤害值
void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);   //Clamp限制范围

	UE_LOG(LogTemp, Warning, TEXT("Health reduced to: %.2f"), Health);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);

	UE_LOG(LogTemp, Warning, TEXT("Stamina reduced to: %.2f"), Stamina);
}

//获取血量百分比
float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina/MaxStamina;
}

//判断敌人是否死亡
bool UAttributeComponent::isAlive()
{
	return Health>0.f;
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//RegenStamina(DeltaTime);
	
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

