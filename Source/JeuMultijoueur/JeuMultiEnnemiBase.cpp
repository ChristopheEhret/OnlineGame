// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "JeuMultiEnnemiBase.h"

AJeuMultiEnnemiBase::AJeuMultiEnnemiBase()
{
	
}

void AJeuMultiEnnemiBase::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJeuMultiEnnemiBase, directionAnimHaut);
	DOREPLIFETIME(AJeuMultiEnnemiBase, directionAnimBas);

}

void AJeuMultiEnnemiBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJeuMultiEnnemiBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AJeuMultiEnnemiBase::verifAggroBasique() {
	FVector ennemiPosYeux = this->GetActorLocation();
	ennemiPosYeux.Z += 85.f;

	FVector joueurPosYeux;
	FVector joueurPosPieds;
	FRotator rotation;
	
	bool obstacle = false;
	float distance = 0.f;

	for (int i = 0; i < listeJoueurs.Num(); i++) {
		joueurPosYeux = joueurPosPieds = listeJoueurs[i]->GetActorLocation();
		joueurPosYeux.Z += 85.f;
		joueurPosPieds.Z -= 85.f;

		obstacle = false;
		distance = (joueurPosYeux - ennemiPosYeux).Size();
		rotation = (listeJoueurs[i]->GetActorLocation() - this->GetActorLocation()).Rotation() - this->GetActorForwardVector().Rotation();
		if (listeJoueurs[i] != cible) {
			if (cible == nullptr) {
				if(distance < aggroHaute)
					if (distance < aggroChange) {
						cible = listeJoueurs[i];
					}
					else {
						FHitResult hit;
						FHitResult hitBas;
						FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, (listeJoueurs[i], this));
						obstacle = (this->GetWorld()->LineTraceSingleByChannel(hit, ennemiPosYeux, joueurPosYeux, ECC_WorldStatic, CombatCollisionQuery) || this->GetWorld()->LineTraceSingleByChannel(hitBas, ennemiPosYeux, joueurPosPieds, ECC_WorldStatic, CombatCollisionQuery));
						if (obstacle && hit.Actor->GetClass()->IsChildOf(AJeuMultiCharacter::StaticClass())) {
							if (distance < aggroBasse)
								cible = listeJoueurs[i];
							else {
								if (rotation.Yaw <= 90.f && rotation.Yaw >= -90.f) {
									cible = listeJoueurs[i];
								}
							}
						}
					}
			}
			else {
				if (distance < aggroChange)
					cible = listeJoueurs[i];
			}
		}
		else {
			if (distance > aggroPerte) 
				cible = nullptr;
			else {
				FHitResult hit;
				FHitResult hitBas;
				FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, this);
				obstacle = (this->GetWorld()->LineTraceSingleByChannel(hit, ennemiPosYeux, joueurPosYeux, ECC_WorldStatic, CombatCollisionQuery) || this->GetWorld()->LineTraceSingleByChannel(hitBas, ennemiPosYeux, joueurPosPieds, ECC_WorldStatic, CombatCollisionQuery));
				if (obstacle) {
					if(hit.Actor !=nullptr && hitBas.Actor != nullptr)
						if (!hit.Actor->GetClass()->IsChildOf(AJeuMultiCharacter::StaticClass()) && !hitBas.Actor->GetClass()->IsChildOf(AJeuMultiCharacter::StaticClass()))
							cible = nullptr;
				}
			}
		}
	}

	if (cible != nullptr) {
		if ((cible->GetActorLocation() - this->GetActorLocation()).Size() <= distanceAttaque) {
			if (!attaque)
				attaqueBasique();
		}
		else if (attaque)
			attaqueFinBasique();
	}
	else {
		if(attaque)
			attaqueFinBasique();
	}
}

void AJeuMultiEnnemiBase::bougeBasique() {
	if (cible != nullptr) {
		
		FVector posCible = cible->GetActorLocation();
		if (cible->GetMovementComponent()->IsFalling()) 
			posCible.Z = this->GetActorLocation().Z;

		this->AIController->MoveTo(posCible);
		
	}
	else {
		if (AIController != nullptr) 
			if (!this->AIController->IsFollowingAPath()) {

				if (FMath::FRandRange(0, 100) <= 1) {
					FVector posBouge = this->GetActorLocation();

					posBouge.X += FMath::FRandRange(-300, 300);
					posBouge.Y += FMath::FRandRange(-300, 300);

					this->AIController->MoveTo(posBouge);
				}
			}
		
	}
}

void AJeuMultiEnnemiBase::changeAnimBasique()
{
	if (this->cible != nullptr) {
		directionAnimBas = ANIM_RUN;
		directionAnimHaut = ANIM_RUN;
	}
	else if (this->AIController->IsFollowingAPath()) {
		directionAnimBas = ANIM_WALK;
		directionAnimHaut = ANIM_WALK;
	}
	else {
		directionAnimBas = ANIM_IDLE;
		directionAnimHaut = ANIM_IDLE;
	}

	if (attaque)
		directionAnimHaut = ANIM_ATTACK;
}

void AJeuMultiEnnemiBase::attaqueBasique() {
	attaque = true;
}

void AJeuMultiEnnemiBase::attaqueFinBasique() {
	attaque = false;
}