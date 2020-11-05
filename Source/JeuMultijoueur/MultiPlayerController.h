// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "MultiPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API AMultiPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated)
		float idMulti = 0;

	int lastJoueurArrayIndex = 0;
};
