// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "JeuMultiCharacter.generated.h"

//Différentes animations possibles
#define ANIM_IDLE 0
#define ANIM_GO_FORW 1
#define ANIM_GO_RIGHT 2
#define ANIM_GO_LEFT 3
#define ANIM_GO_BACKW 4
#define ANIM_SHOOT 5

UCLASS()
class AJeuMultiCharacter : public ACharacter
{
	GENERATED_BODY()

	//Bras de caméra
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	//Caméra
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	AJeuMultiCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//Connection au serveur
	UFUNCTION(BlueprintCallable, Category = "UMG Game")
		void Connect(FString servAdresse, FString pseudo);

	//Indique l'animation jouée actuellement
	UPROPERTY(Replicated)
		int directionAnim = ANIM_IDLE;

	//Fcnt d'envoi de l'animation actuelle au serveur
	void setDirectionAnim(int nDir);
	UFUNCTION(reliable, server, WithValidation)
		void SERVEUR_setDirectionAnim(int nDir);
	virtual void SERVEUR_setDirectionAnim_Implementation(int nDir);
	virtual bool SERVEUR_setDirectionAnim_Validate(int nDir);

	//True si le joueur souhaite tirer
	UPROPERTY(Replicated)
		bool tire = false;

	//Fncts d'initailisation et de renvoi de "tire"
	void tirer();
	void tirerDown();
	bool getTire();

	//Fnct d'envoi de "tire" au serveur
	void setTire(bool nT);
	UFUNCTION(reliable, server, WithValidation)
		void SERVEUR_setTire(bool nT);
	virtual void SERVEUR_setTire_Implementation(bool nT);
	virtual bool SERVEUR_setTire_Validate(bool nT);

	//Rotation actuelle de la caméra
	UPROPERTY(Replicated)
		FRotator rotationCamera = FRotator(0.f);

	//Fnct d'envoi de la rotation de la caméra au serveur
	void updateRotationCamera(FRotator nRC);
	UFUNCTION(reliable, server, WithValidation)
		void SERVEUR_updateRotationCamera(FRotator nRC);
	void SERVEUR_updateRotationCamera_Implementation(FRotator nRC);
	bool SERVEUR_updateRotationCamera_Validate(FRotator nRC);

	//Id du joueur, identique à celui de son controller, qui permet au serveur de le retrouver
	UPROPERTY(Replicated)
		float idMulti = 0;

	//Dernière position du controller dans le TArray du serveur, fait pour optimiser le temps utilisé pour retrouver le controller
	int lastControllerArrayIndex = 0;

	UPROPERTY(BlueprintReadOnly)
		bool afficherTableauScore = false;
	void afficheTableauScore();
	void cacheTableauScore();

	UPROPERTY(BlueprintReadOnly)
		bool mort = false;
	UFUNCTION(reliable, NetMulticast, WithValidation)
		void SERVEUR_meurt(FVector force);
	void SERVEUR_meurt_Implementation(FVector force);
	bool SERVEUR_meurt_Validate(FVector force);

	UPROPERTY(Replicated)
		bool respawn = false;
	void respawner();
	void setRespawn(bool nR);
	UFUNCTION(reliable, server, WithValidation)
		void SERVEUR_setRespawn(bool nR);
	void SERVEUR_setRespawn_Implementation(bool nR);
	bool SERVEUR_setRespawn_Validate(bool nR);

	UMaterialInterface* decalMaterial;
	UFUNCTION(reliable, NetMulticast, WithValidation)
		void SERVEUR_creerImpactBalle(AActor* acteur, FVector pos, FRotator rot);
	void SERVEUR_creerImpactBalle_Implementation(AActor* acteur, FVector pos, FRotator rot);
	bool SERVEUR_creerImpactBalle_Validate(AActor* acteur, FVector pos, FRotator rot);
protected:

	void sauter();

	//Avance ou reculer le joueur selon les inputs 
	void allerToutDroit(float AxisValue);

	//Fait aller le joueur à droite ou à gauche selon les inputs 
	void allerADroite(float AxisValue);

	//Tourne la caméra vers la droite ou la gauche selon les inputs
	void tournerVueDroite(float AxisValue);

	//Tourne la caméra vers le haut ou le bas selon les inputs
	void tournerVueHaut(float AxisValue);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	//Modèles FPS et TPS du joueur
	USkeletalMesh* MeshFPS;
	USkeletalMesh* MeshTPS;

	//Position du modèle FPS par rapport au joueur
	UPROPERTY(EditAnywhere, Category = "Mesh")
		FVector posMeshFPS = FVector(-5.f, -10.f, -145.f);
	//Position du modèle TPS par rapport au joueur
	UPROPERTY(EditAnywhere, Category = "Mesh")
		FVector posMeshTPS = FVector(0.f, 0.f, -90.f);

	//Distance de la caméra par rapport au joueur
	UPROPERTY(EditAnywhere, Category = "Camera")
		float distanceCamera = 0.f;
	//Position du bras de caméra par rapport au joueur
	UPROPERTY(EditAnywhere, Category = "Camera")
		FVector positionCamera = FVector(0.f, 0.f, 65.f);


	//Vitesse du joueur en unités par seconde
	UPROPERTY(EditAnywhere, Category = "Mouvement")
		float vitesse = 100.f;

	//Animations des modèles en FPS et TPS
	UAnimBlueprint *animBPTPS;
	UAnimBlueprint *animBPFPS;
};