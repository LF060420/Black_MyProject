#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set Health"));
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Set Stamina"));
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetCoinText(int32 Gold)
{
	if (CoinText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		UE_LOG(LogTemp, Warning, TEXT("Set Coin"));
		CoinText->SetText(Text);
	}
}

void USlashOverlay::SetSoulsText(int32 Souls)
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		UE_LOG(LogTemp, Warning, TEXT("Set Souls"));
		SoulsText->SetText(Text);
	}
}
