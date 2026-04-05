// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetCoinText(int32 Gold);
	void SetSoulsText(int32 Souls);

private:

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthProgressBar;   //血条

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;   //耐力条

	UPROPERTY(meta = (BindWidget)) 
	class UTextBlock* CoinText;   //金币数量

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;   //灵魂数量	
};
