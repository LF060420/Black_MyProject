// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//让其在C++的"HealthBar"与用户控件蓝图的"HealthBar"绑定，名字必须一样才能绑定
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
};
