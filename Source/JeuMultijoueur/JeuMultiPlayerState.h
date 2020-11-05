// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "JeuMultiPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API AJeuMultiPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, BlueprintReadOnly)
		int morts = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int tues = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FString pseudo = "NewPlayer";

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool connecte = true;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
