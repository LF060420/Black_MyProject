// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBar.h"
#include "HealthBarComponent.generated.h"


class AEnemy;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	//新建 [获取人物血量百分比] 事件
	void SetHealthPercent(float Percent);
private:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;

	UPROPERTY()
	AEnemy* Enemy;
};
