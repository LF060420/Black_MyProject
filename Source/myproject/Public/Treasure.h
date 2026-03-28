// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "items.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ATreasure : public Aitems
{
	GENERATED_BODY()
	
protected:
	//重写球体开始重叠事件
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere,Category=Sounds)
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere,Category="Treasure Properties")
	int32 Gold;
};
