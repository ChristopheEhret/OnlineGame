// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "JeuMultiAI.h"


// Sets default values
AJeuMultiAI::AJeuMultiAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void AJeuMultiAI::BeginPlay()
{
	Super::BeginPlay();
	
	if (Controller != nullptr)
		AIController = Cast<AAIController>(Controller);
	else {
		this->SpawnDefaultController();
		AIController = Cast<AAIController>(Controller);
	}

	this->GetCharacterMovement()->MaxWalkSpeed = vitesseMarche;
}

// Called every frame
void AJeuMultiAI::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AJeuMultiAI::meurt_Implementation(FVector force)
{

	this->directionAnim = ANIM_IDLE;
	this->GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("root"), true);
	this->GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("root"), true);
	this->GetMesh()->AddImpulse(force * 2, FName(*osImpact));
	this->GetMesh()->SetCollisionProfileName(FName("Ragdoll"));

	this->mort = true;
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}