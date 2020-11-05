// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "MultiPlayerController.h"

void AMultiPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void AMultiPlayerController::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiPlayerController, idMulti);
}

