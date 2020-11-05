// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "JeuMultiZombie.h"

AJeuMultiZombie::AJeuMultiZombie() 
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> meshFinder(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Zombie.SK_Zombie'"));
	if (meshFinder.Succeeded())
	{
		mesh = meshFinder.Object;
	}
	this->GetMesh()->SetSkeletalMesh(mesh);
	this->GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	this->GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> ABZ(TEXT("AnimBlueprint'/Game/Mannequin/Animations/AnimBlueprintZombie.AnimBlueprintZombie'"));
	if (ABZ.Succeeded())
	{
		animZombie = ABZ.Object;
	}
	this->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	this->GetMesh()->SetAnimInstanceClass(animZombie->GeneratedClass);

	vitesseMarche = 100.f;
	vitesseAggro = 300.f;
}

void AJeuMultiZombie::BeginPlay()
{
	Super::BeginPlay();

	const FActorSpawnParameters asp = FActorSpawnParameters();
	FRotator r = FRotator(0.f);
	FVector v = FVector(0.f);
	arme = Cast<AArmeCac>(GetWorld()->SpawnActor(AArmeCac::StaticClass(), &v, &r, asp));
	arme->AttachRootComponentTo(this->GetMesh(), FName("Arme"), EAttachLocation::SnapToTargetIncludingScale);
	this->GetCapsuleComponent()->IgnoreComponentWhenMoving(arme->collision, true);
}

void AJeuMultiZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (cible == nullptr) {
		this->GetCharacterMovement()->MaxWalkSpeed = vitesseMarche;
	}
	else {
		this->GetCharacterMovement()->MaxWalkSpeed = vitesseAggro;
	}

	if (AIController != nullptr) {
		verifAggroBasique();
		bougeBasique();

		changeAnimBasique();

		if (attaque && cible != nullptr) {
			FCollisionQueryParams CombatCollisionQuery(FName(TEXT("CombatTrace")), true, this);
			if (arme->collision->ComponentOverlapComponent(cible->GetCapsuleComponent(), cible->GetActorLocation(), cible->GetActorRotation(), CombatCollisionQuery)) {
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "touche");
				cible->SERVEUR_meurt(this->GetActorForwardVector() * 300.f);
				listeJoueurs.Remove(cible);
				cible = nullptr;
				attaqueFinBasique();
				AIController->StopMovement();
			}
		}
	}
}

void AJeuMultiZombie::attaqueBasique() {
	Super::attaqueBasique();

	//arme->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AJeuMultiZombie::attaqueFinBasique() {
	Super::attaqueFinBasique();

	//arme->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
