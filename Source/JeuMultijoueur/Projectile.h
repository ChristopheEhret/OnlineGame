// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealNetwork.h"
#include "JeuMultiCharacter.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class JEUMULTIJOUEUR_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	int vitesse = 300.f;

	UPROPERTY(Replicated)
		UStaticMeshComponent* visuel;

	AJeuMultiCharacter* tireur;
};
