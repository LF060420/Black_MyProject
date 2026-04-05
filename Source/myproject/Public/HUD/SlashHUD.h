// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;

UCLASS()
class MYPROJECT_API ASlashHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	

private:
	

	UPROPERTY(EditDefaultsOnly,Category = Slash)
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:
	void Begin();
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
};
