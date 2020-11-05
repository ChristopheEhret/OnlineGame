// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "AnimInstanceZombie.h"


void UAnimInstanceZombie::UpdateAnimationProperties()
{
	AJeuMultiZombie* Zombie = Cast<AJeuMultiZombie>(TryGetPawnOwner());

	if (Zombie) {

		directionAnimHaut = Zombie->directionAnimHaut;

		directionAnimBas = Zombie->directionAnimBas;

	}
}

