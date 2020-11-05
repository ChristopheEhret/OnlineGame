// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "JeuMultijoueurGameMode.h"

// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "message");
// GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, message);

AJeuMultijoueurGameMode::AJeuMultijoueurGameMode()
{
	//Initialisation de la classe du pawn de base
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Script/JeuMultijoueur.JeuMultiCharacter"));
	if (PlayerPawnClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnClass.Class;
	}

}

void AJeuMultijoueurGameMode::BeginPlay()
{
	Super::BeginPlay();

	listeEnnemis.Empty();
	for (int i = 0; i < this->GetWorld()->GetCurrentLevel()->Actors.Num(); i++) {
		if (this->GetWorld()->GetCurrentLevel()->Actors[i] != nullptr)
			if (this->GetWorld()->GetCurrentLevel()->Actors[i]->GetClass()->IsChildOf(AJeuMultiEnnemiBase::StaticClass())) {
				listeEnnemis.Add(Cast<AJeuMultiEnnemiBase>(this->GetWorld()->GetCurrentLevel()->Actors[i]));
			}
	}
}

void AJeuMultijoueurGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	//Vérifie si un joueur souhaite tirer
	for (int i = 0; i < listeJoueurs.Num(); i++) {
		if (listeJoueurs[i] != nullptr) {
			if (listeJoueurs[i]->tire) { //Si oui
				//Spawn du projectile
				const FActorSpawnParameters asp = FActorSpawnParameters();
				FRotator r = FRotator(listeJoueurs[i]->rotationCamera);
				FVector v = FVector(listeJoueurs[i]->GetCameraBoom()->GetComponentLocation() + (listeJoueurs[i]->rotationCamera.Vector() * 100.f)); //Fait en sorte que la position du projectile ne soit pas dans la collision du joueur qui a tiré
				AProjectile* nProjectile = Cast<AProjectile>(listeJoueurs[i]->GetWorld()->SpawnActor(AProjectile::StaticClass(), &v, &r, asp));
				
				nProjectile->tireur = listeJoueurs[i];

				listeProjectiles.Add(nProjectile); //Ajout du projectile à la liste

				listeJoueurs[i]->setTire(false); //Anonce au joueur qu'il a fini de tirer
			}

			if (listeJoueurs[i]->respawn && listeJoueurs[i]->mort) {
				respawnJoueur(listeJoueurs[i]);
			}
		}
	}


	//Vérifie si un joueur se fait toucher par un projectile
	for (int i = 0; i < listeProjectiles.Num(); i++) {
		if (listeProjectiles[i] != nullptr) {

			FHitResult hit;
			FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, (listeProjectiles[i], listeProjectiles[i]->tireur));

			if (listeProjectiles[i]->GetWorld()->LineTraceSingleByChannel(hit, listeProjectiles[i]->GetActorLocation(), listeProjectiles[i]->GetActorLocation() + (listeProjectiles[i]->GetActorForwardVector() * listeProjectiles[i]->vitesse), ECC_WorldStatic, CombatCollisionQuery) && hit.Actor != listeProjectiles[i]) {
				if (hit.GetActor()->GetClass()->IsChildOf(AJeuMultiCharacter::StaticClass())) {

					//Si oui
					for (int j = 0; j < listeControllers.Num(); j++) {
						if (listeControllers[j]->GetPawn() == Cast<AJeuMultiCharacter>(hit.GetActor())) { //Recherche du controller du joueur touché
							if (!Cast<AJeuMultiCharacter>(listeControllers[j]->GetPawn())->mort) {
								Cast<AJeuMultiCharacter>(listeControllers[j]->GetPawn())->SERVEUR_meurt(listeProjectiles[i]->GetActorForwardVector() * listeProjectiles[i]->vitesse);

								//Cast<AJeuMultiPlayerState>(listeControllers[j]->PlayerState)->morts += 1;
								Cast<AJeuMultiPlayerState>(listeProjectiles[i]->tireur->PlayerState)->tues += 1;
							}
						
						}
					}

				}
				if (hit.GetActor()->GetClass()->IsChildOf(AJeuMultiEnnemiBase::StaticClass())) {
					Cast<AJeuMultiEnnemiBase>(hit.GetActor())->meurt(listeProjectiles[i]->GetActorForwardVector() * listeProjectiles[i]->vitesse);

					Cast<AJeuMultiPlayerState>(listeProjectiles[i]->tireur->PlayerState)->tues += 1;
				}
				else if(!hit.GetActor()->GetClass()->IsChildOf(AJeuMultiCharacter::StaticClass())) {
					listeProjectiles[i]->tireur->SERVEUR_creerImpactBalle(hit.GetActor(), hit.Location, (-hit.ImpactNormal).Rotation());
				}

				//Supression du projectile
				AProjectile* projectile = listeProjectiles[i];
				if (hit.Actor != projectile) {
					listeProjectiles.Remove(projectile);
					projectile->Destroy();
				}

			}
			else {
				//Si non, on fait avancer le projectile
				listeProjectiles[i]->SetActorLocation(listeProjectiles[i]->GetActorLocation() + listeProjectiles[i]->GetActorForwardVector() * listeProjectiles[i]->vitesse);
			}
		}
	}
}

//Recupétation du pseudo
void AJeuMultijoueurGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) {
#if WITH_EDITOR
	pseudoEnConnection = "NewPlayer";
#else
	if (listeControllers.Num() >= 10)
		ErrorMessage = "Serveur complet. Veuillez réessayer plus tard."; //Max joueurs : 10 prs

	FString pseudo = Options; //"pseudo" est de la forme : "?Pseudo="pseudo"?..."
	pseudo.RemoveAt(0, 8); //Enlève "?Pseudo="
	
	//Trouve la longeur du pseudo (de la 1re lettre au 1er "?")
	int longPseudo = 0;
	for (int i = 1; pseudo[i - 1] != '?'; i++) {
		longPseudo = i;
	}

	//Enlève tout ce qui est après le pseudo
	while(pseudo.Len() != longPseudo)
		pseudo.RemoveAt(longPseudo, 1);

	//Le pseudo de la personne entrain de se connecter est donc "pseudo"
	pseudoEnConnection = pseudo;
#endif
}

void AJeuMultijoueurGameMode::PostLogin(APlayerController* NewPlayer) {

	//Qqun se connecte
	connection = true;

	//Initialisation du spawn du nouveau joueur
	const FActorSpawnParameters asp = FActorSpawnParameters();
	FRotator rotSpawn = FRotator(0.f);

	//Créé une position de spawn aléatoire
	FVector posSpawn = FVector(0.f, 0.f, 500.f);
	posSpawn.X = FMath::FRandRange(posSpawnMin.X, posSpawnMax.X);
	posSpawn.Y = FMath::FRandRange(posSpawnMin.Y, posSpawnMax.Y);

	//Spawn du joueur

	AJeuMultiCharacter* nouveauJoueur = Cast<AJeuMultiCharacter>(NewPlayer->GetWorld()->SpawnActor(AJeuMultiCharacter::StaticClass(), &posSpawn, &rotSpawn, asp));;
	while (nouveauJoueur == nullptr) {
		posSpawn.X = FMath::FRandRange(posSpawnMin.X, posSpawnMax.X);
		posSpawn.Y = FMath::FRandRange(posSpawnMin.Y, posSpawnMax.Y);

		nouveauJoueur = Cast<AJeuMultiCharacter>(NewPlayer->GetWorld()->SpawnActor(AJeuMultiCharacter::StaticClass(), &posSpawn, &rotSpawn, asp));
	}
	AMultiPlayerController* nouveauController = Cast<AMultiPlayerController>(NewPlayer);

	//Attache le joueur précédemment créé au nouveau controller
	nouveauController->SetPawn(nouveauJoueur);
	nouveauController->Possess(nouveauController->GetPawn());
	SetPlayerDefaults(nouveauController->GetPawn());

	//Initialisation des ID
	nouveauController->idMulti = dernierID;
	nouveauController->lastJoueurArrayIndex = listeJoueurs.Num();
	nouveauJoueur->idMulti = dernierID;
	nouveauJoueur->lastControllerArrayIndex = listeControllers.Num();
	++dernierID;

	//Evite que le premier controller connecté se retrouve sans joueur
	if (listeControllers.Num() >= 1) {
		AMultiPlayerController* firstPC = Cast<AMultiPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		if (firstPC != nullptr) {
			firstPC->SetPawn(findJoueurByContr(firstPC));
			firstPC->Possess(firstPC->GetPawn());
			SetPlayerDefaults(firstPC->GetPawn());
		}
	}

	Cast<AJeuMultiPlayerState>(nouveauController->PlayerState)->pseudo = pseudoEnConnection;

	//Ajoute aux listes le controller et son joueur
	listeJoueurs.Add(nouveauJoueur);
	listeControllers.Add(nouveauController);

	for (int i = 0; i < listeEnnemis.Num(); i++){
		listeEnnemis[i]->listeJoueurs.Add(nouveauJoueur);
	}
}

void AJeuMultijoueurGameMode::Logout(AController* Existing) {

	if (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer)
	{
		//Enlève le controller qui s'est déconnecté et son joueur des listes
		if (listeJoueurs.Num() != 0) {
			listeJoueurs.Remove(findJoueurByContr(Cast<AMultiPlayerController>(Existing)));
			Cast<AJeuMultiPlayerState>(Existing->PlayerState)->connecte = false;
		}

		for (int i = 0; i < listeEnnemis.Num(); i++) {
			if (listeEnnemis[i]->cible == findJoueurByContr(Cast<AMultiPlayerController>(Existing)))
				listeEnnemis[i]->cible = nullptr;

			listeEnnemis[i]->listeJoueurs.Remove(findJoueurByContr(Cast<AMultiPlayerController>(Existing)));
		}

		if (listeControllers.Num() != 0)
			listeControllers.Remove(Cast<AMultiPlayerController>(Existing));
	}
}

AJeuMultiCharacter* AJeuMultijoueurGameMode::findJoueurByContr(AMultiPlayerController* controller) {

	//Vérifie si la dernière position du joueur enregistée dans la liste est valable
	if(controller->lastJoueurArrayIndex < listeJoueurs.Num())
		if (listeJoueurs[controller->lastJoueurArrayIndex]->idMulti == controller->idMulti) {
			return listeJoueurs[controller->lastJoueurArrayIndex];
		}

	//Sinon, trouve si le joueur avec les ID
	for (int i = 0; i < listeJoueurs.Num(); i++) {
		if (listeJoueurs[i]->idMulti == controller->idMulti) {
			controller->lastJoueurArrayIndex = i;

			return listeJoueurs[i];
		}
	}

	//Sinon, on retourne rien
	return nullptr;
	
}

AMultiPlayerController* AJeuMultijoueurGameMode::findContrByJoueur(AJeuMultiCharacter* joueur) {

	//Vérifie si la dernière position du controller enregistée est valable
	if (joueur->lastControllerArrayIndex < listeControllers.Num())
		if (listeControllers[joueur->lastControllerArrayIndex]->idMulti == joueur->idMulti) {
			return listeControllers[joueur->lastControllerArrayIndex];
		}
	
	//Sinon, trouve si le joueur avec les ID
	for (int i = 0; i < listeControllers.Num(); i++) {
		if (listeControllers[i]->idMulti == joueur->idMulti) {
			joueur->lastControllerArrayIndex = i;

			return listeControllers[i];
		}
	}

	//Sinon, on retourne rien
	return nullptr;
	
}

void AJeuMultijoueurGameMode::respawnJoueur(AJeuMultiCharacter* joueur) {

	AMultiPlayerController* contr = findContrByJoueur(joueur);

	if (contr != nullptr) {
		Cast<AJeuMultiPlayerState>(joueur->PlayerState)->morts += 1;

		listeJoueurs.Remove(joueur);
		joueur->Destroy();

		//Initialisation du spawn du nouveau joueur
		const FActorSpawnParameters asp = FActorSpawnParameters();
		FRotator rotSpawn = FRotator(0.f);

		//Créé une position de spawn aléatoire
		FVector posSpawn = FVector(0.f, 0.f, 500.f);
		posSpawn.X = FMath::FRandRange(posSpawnMin.X, posSpawnMax.X);
		posSpawn.Y = FMath::FRandRange(posSpawnMin.Y, posSpawnMax.Y);

		//Spawn du joueur
		AJeuMultiCharacter* nouveauJoueur = Cast<AJeuMultiCharacter>(contr->GetWorld()->SpawnActor(AJeuMultiCharacter::StaticClass(), &posSpawn, &rotSpawn, asp));


		//Attache le joueur précédemment créé à son controller
		contr->SetPawn(nouveauJoueur);
		contr->Possess(contr->GetPawn());
		SetPlayerDefaults(contr->GetPawn());

		//Initialisation des ID
		nouveauJoueur->idMulti = contr->idMulti;
		int index = 0;
		listeControllers.Find(contr, index);
		nouveauJoueur->lastControllerArrayIndex = index;
		contr->lastJoueurArrayIndex = listeJoueurs.Num();

		listeJoueurs.Add(nouveauJoueur); //Ajout du joueur à la liste

		nouveauJoueur->mort = false;
		nouveauJoueur->setRespawn(false);

		//Evite que le premier controller connecté se retrouve sans joueur
		if (listeControllers.Num() >= 1) {
			AMultiPlayerController* firstPC = Cast<AMultiPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

			if (firstPC != nullptr) {
				firstPC->SetPawn(findJoueurByContr(firstPC));
				firstPC->Possess(firstPC->GetPawn());
				SetPlayerDefaults(firstPC->GetPawn());
			}
		}

		for (int i = 0; i < listeEnnemis.Num(); i++) {
			listeEnnemis[i]->listeJoueurs.Add(nouveauJoueur);
		}
	}
}
