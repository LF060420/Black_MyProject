// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
void ASlashHUD::Begin()
{
	//Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("HUDBeginPlay"));
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SlashOverlayClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("HUDAddSuccess"));
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
			SlashOverlay->AddToViewport();
		}
	}
}
