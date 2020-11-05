// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JeuMultiZombie.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceZombie.generated.h"

/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API UAnimInstanceZombie : public UAnimInstance
{
	GENERATED_BODY()
	
public :
	UFUNCTION(BluePrintCallable, Category = "UpdateAnimationProperties")
		void UpdateAnimationProperties();
	
	UPROPERTY(EditAnyWhere, BluePrintReadOnly)
		int directionAnimHaut = 0;

	UPROPERTY(EditAnyWhere, BluePrintReadOnly)
		int directionAnimBas = 0;
};
