    // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JeuMultiCharacter.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceJoueur.generated.h"

/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API UAnimInstanceJoueur : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		bool IsFalling;

	UPROPERTY(EditAnyWhere, BluePrintReadOnly)
		bool IsMoving;

	UFUNCTION(BluePrintCallable, Category = "UpdateAnimationProperties")
		void UpdateAnimationProperties();

	UPROPERTY(EditAnyWhere, BluePrintReadOnly)
		int directionAnim = 0;

	UPROPERTY(EditAnyWhere, BluePrintReadOnly)
		int oldDirectionAnim = 0;
	
};
