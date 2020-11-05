// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "JeuMultiCharacter.h"
#include "GameFramework/Character.h"
#include "JeuMultiAI.generated.h"

#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_RUN 2
#define ANIM_ATTACK 3

UCLASS()
class JEUMULTIJOUEUR_API AJeuMultiAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJeuMultiAI();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	int directionAnim = ANIM_IDLE;


	TArray<AJeuMultiCharacter*> listeJoueurs;
	AJeuMultiCharacter* cible;

	AJeuMultiCharacter* findJoueurById(float id);

	bool mort = false;
	UFUNCTION(reliable, NetMulticast)
		void meurt(FVector force);
	void meurt_Implementation(FVector force);

	float vitesseMarche = 300.f;
	float vitesseAggro = 600.f;

	//Os auquel va être appliqué la force de l'impact lors de la mort.
	FString osImpact = "spine_02";

	USkeletalMesh* mesh;

	AAIController* AIController;
};
