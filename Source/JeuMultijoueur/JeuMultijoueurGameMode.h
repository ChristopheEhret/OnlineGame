// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JeuMultiEnnemiBase.h"
#include "Projectile.h"
#include "JeuMultiCharacter.h"
#include "MultiPlayerController.h"
#include "JeuMultiPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameMode.h"
#include "JeuMultijoueurGameMode.generated.h"

/**
 * 
 */
UCLASS()
class JEUMULTIJOUEUR_API AJeuMultijoueurGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AJeuMultijoueurGameMode();

	virtual void Tick(float DeltaSeconds) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Existing) override;

	//Positions qui encadrent la osition possible de spawn du joueur
	FVector posSpawnMin = FVector(-950.f, -950.f, 200.f);
	FVector posSpawnMax = FVector(950.f, 950.f, 200.f);

	//Listes des controllers et des joueurs connectés au serveur
	TArray<AMultiPlayerController*> listeControllers;
	TArray<AJeuMultiCharacter*> listeJoueurs;
	TArray<AJeuMultiEnnemiBase*> listeEnnemis;

	//Liste de projectiles présents sur le serveur
	TArray<AProjectile*> listeProjectiles;

	//Pseudo de la personne entrain de se connecter
	FString pseudoEnConnection = "New Player";

	//Vrai si qqun est entrain de se connecter (inutile pour le moment)
	bool connection = false;

	//dernier ID de joueur/controller utilisé
	float dernierID = 0;

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	//Fncts qui permettent de retrouver un controller grâce à son joueur et vice versa
	AJeuMultiCharacter* findJoueurByContr(AMultiPlayerController* controller);
	AMultiPlayerController* findContrByJoueur(AJeuMultiCharacter* joueur);
	void respawnJoueur(AJeuMultiCharacter* joueur);
};
