// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArmeCac.h"
#include "JeuMultiEnnemiBase.h"
#include "JeuMultiZombie.generated.h"

/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API AJeuMultiZombie : public AJeuMultiEnnemiBase
{
	GENERATED_BODY()
	
public:
	AJeuMultiZombie();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void attaqueBasique();
	void attaqueFinBasique();
	
	AArmeCac* arme;

	UAnimBlueprint *animZombie;
};
