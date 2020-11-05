// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "JeuMultiPlayerState.h"

void AJeuMultiPlayerState::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJeuMultiPlayerState, morts);
	DOREPLIFETIME(AJeuMultiPlayerState, tues);
	DOREPLIFETIME(AJeuMultiPlayerState, pseudo);
	DOREPLIFETIME(AJeuMultiPlayerState, connecte);
}

