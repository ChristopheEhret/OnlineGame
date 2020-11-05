// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "AnimInstanceJoueur.h"

void UAnimInstanceJoueur::UpdateAnimationProperties()
{
	AJeuMultiCharacter* Character = Cast<AJeuMultiCharacter>(TryGetPawnOwner());

	if (Character) {
		IsFalling = Character->GetMovementComponent()->IsFalling();

		IsMoving = Character->GetMovementComponent()->IsMovingOnGround();

		oldDirectionAnim = directionAnim;

		directionAnim = Character->directionAnim;


	}
}


