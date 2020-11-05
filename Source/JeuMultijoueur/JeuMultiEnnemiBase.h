// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JeuMultiAI.h"
#include "JeuMultiEnnemiBase.generated.h"

#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_RUN 2
#define ANIM_ATTACK 3
/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API AJeuMultiEnnemiBase : public AJeuMultiAI
{
	GENERATED_BODY()
	
public: 
	// Sets default values for this character's properties
	AJeuMultiEnnemiBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void verifAggroBasique();
	void bougeBasique();

	virtual void attaqueBasique();
	virtual void attaqueFinBasique();
	UPROPERTY(BlueprintReadOnly)
		bool attaque = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int directionAnimBas = ANIM_IDLE;
	UPROPERTY(Replicated, BlueprintReadOnly)
		int directionAnimHaut = ANIM_IDLE;
	void changeAnimBasique();

	float aggroHaute = 600.f;
	float aggroBasse = 300.f;
	float aggroChange = 100.f;
	float aggroPerte = 1500.f;
	float distanceAttaque = 80.f;
};
