// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "JeuMultiCharacter.h"

// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "message");
// GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, message);
AJeuMultiCharacter::AJeuMultiCharacter()
{

	//Initialisation des modèles
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> meshFPSFinder(TEXT("/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms"));
	if (meshFPSFinder.Succeeded())
	{
		MeshFPS = meshFPSFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> meshTPSFinder(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin"));
	if (meshTPSFinder.Succeeded())
	{
		MeshTPS = meshTPSFinder.Object;
	}

	//Initialisation des AnimBlueprint
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> ABPT(TEXT("AnimBlueprint'/Game/Blueprints/AnimBlueprintJoueur.AnimBlueprintJoueur'"));
	if (ABPT.Succeeded())
	{
		animBPTPS = ABPT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> ABPF(TEXT("AnimBlueprint'/Game/Blueprints/FirstPersonBP.FirstPersonBP'"));
	if (ABPF.Succeeded())
	{
		animBPFPS = ABPF.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> mater(TEXT("Material'/Game/Images/M_impact.M_impact'"));
	if (mater.Succeeded()) {
		decalMaterial = mater.Object;

	}

	//Positionnement du Mesh en TPS
	this->GetMesh()->SetSkeletalMesh(MeshTPS);
	this->GetMesh()->SetWorldRotation(FRotator(0.f, -90.f, 0.f));
	this->GetMesh()->SetRelativeLocation(posMeshTPS);

	//Empeche le component de modifier le pathfiding des IA
	this->GetCapsuleComponent()->SetCanEverAffectNavigation(false);

	//Initialisation la vision du joueur
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	//Initialisation le mouvement du joueur
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 1.f;

	//Création du bras de caméra
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = distanceCamera;
	CameraBoom->SetRelativeLocation(positionCamera);
	CameraBoom->bUsePawnControlRotation = false;

	//Création de la caméra
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;	//Recupèration des inputs du joueur
	
	//bReplicates = true;
	//bReplicateMovement = true;
}

//Inputs
void AJeuMultiCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AJeuMultiCharacter::sauter);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AJeuMultiCharacter::allerToutDroit);
	PlayerInputComponent->BindAxis("MoveRight", this, &AJeuMultiCharacter::allerADroite);

	PlayerInputComponent->BindAxis("Turn", this, &AJeuMultiCharacter::tournerVueDroite);
	PlayerInputComponent->BindAxis("LookUp", this, &AJeuMultiCharacter::tournerVueHaut);

	PlayerInputComponent->BindAction("Tirer", IE_Pressed, this, &AJeuMultiCharacter::tirer);
	PlayerInputComponent->BindAction("Tirer", IE_Released, this, &AJeuMultiCharacter::tirerDown);

	PlayerInputComponent->BindAction("AfficherTableauScore", IE_Pressed, this, &AJeuMultiCharacter::afficheTableauScore);
	PlayerInputComponent->BindAction("AfficherTableauScore", IE_Released, this, &AJeuMultiCharacter::cacheTableauScore);

	PlayerInputComponent->BindAction("Respawner", IE_Pressed, this, &AJeuMultiCharacter::respawner);

}

//Initialisation des variables répliquées
void AJeuMultiCharacter::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJeuMultiCharacter, directionAnim);
	DOREPLIFETIME(AJeuMultiCharacter, tire);
	DOREPLIFETIME(AJeuMultiCharacter, rotationCamera);
	DOREPLIFETIME(AJeuMultiCharacter, idMulti);
	DOREPLIFETIME(AJeuMultiCharacter, respawn);
}

void AJeuMultiCharacter::BeginPlay() {
	Super::BeginPlay();

	//Affecte le bon Mesh au joueur selon si c'est celui contôlé par l'utilisateur ou non 
	if (Role == ROLE_AutonomousProxy) {

		//si c'est celui controllé par l'utilisateur, on affecte un modèle pour la 1ere personne
		this->GetMesh()->SetSkeletalMesh(MeshFPS);
		this->GetMesh()->AttachTo(CameraBoom);
		this->GetMesh()->SetRelativeLocation(posMeshFPS);
		this->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		this->GetMesh()->SetAnimInstanceClass(animBPFPS->GeneratedClass);
		this->GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	}
	else {

		//sinon on laisse le modèle actuel, càd celui de la 3eme personne
		this->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		this->GetMesh()->SetAnimInstanceClass(animBPTPS->GeneratedClass);
	}	

}

void AJeuMultiCharacter::Tick(float DeltaSeconds)
{

	//Envoie au serveur la rotation de la caméra à chaque Tick
	if (Role == ROLE_AutonomousProxy) {
		updateRotationCamera(CameraBoom->GetComponentRotation());
	}
}

//Tourner la vue à droite
void AJeuMultiCharacter::tournerVueDroite(float AxisValue) {
	this->AddControllerYawInput(AxisValue);
}

//Tourner la vue en haut
void AJeuMultiCharacter::tournerVueHaut(float AxisValue) {
	FRotator newRotation = CameraBoom->GetComponentRotation();
	newRotation.Pitch = FMath::Clamp(newRotation.Pitch + AxisValue * 2.f, -89.f, 89.f); //Blocage aux deux extrémité, càd à 90° et à -90°
	CameraBoom->SetWorldRotation(newRotation);
}

void AJeuMultiCharacter::allerToutDroit(float AxisValue)
{
	if (!mort) {
		//Mouvement du joueur tout droit
		this->GetMovementComponent()->AddInputVector(vitesse * this->GetRootComponent()->GetForwardVector() * AxisValue * GetWorld()->GetDeltaSeconds());

		//Change l'animation en cours si besoin 
		if (AxisValue > 0) {
			if (directionAnim == ANIM_IDLE)
				setDirectionAnim(ANIM_GO_FORW);
		}
		else if (AxisValue < 0) {
			if (directionAnim == ANIM_IDLE)
				setDirectionAnim(ANIM_GO_BACKW);
		}
		else {
			if (directionAnim == ANIM_GO_BACKW || directionAnim == ANIM_GO_FORW)
				setDirectionAnim(ANIM_IDLE);
		}
	}
}

void AJeuMultiCharacter::allerADroite(float AxisValue)
{	
	if (!mort) {
		//Mouvement du joueur vers la droite
		this->GetMovementComponent()->AddInputVector(vitesse * this->GetRootComponent()->GetRightVector() * AxisValue * GetWorld()->GetDeltaSeconds());

		//Change l'animation en cours si besoin 
		if (AxisValue > 0) {
			if (directionAnim == ANIM_IDLE)
				setDirectionAnim(ANIM_GO_RIGHT);
		}
		else if (AxisValue < 0) {
			if (directionAnim == ANIM_IDLE)
				setDirectionAnim(ANIM_GO_LEFT);
		}
		else {
			if (directionAnim == ANIM_GO_RIGHT || directionAnim == ANIM_GO_LEFT)
				setDirectionAnim(ANIM_IDLE);
		}
	}
}

void AJeuMultiCharacter::sauter() {
	if (!mort)
		this->Jump();
}

//Envoie au serveur l'animation qui doit être jouée par le modèle
void AJeuMultiCharacter::setDirectionAnim(int nDir)
{
	if (!mort) {
		directionAnim = nDir;

		if (Role < ROLE_Authority)
		{
			SERVEUR_setDirectionAnim(nDir);
		}
	}
}

//Partie serveur de la fonction
void AJeuMultiCharacter::SERVEUR_setDirectionAnim_Implementation(int nDir)
{
	setDirectionAnim(nDir);
}

bool AJeuMultiCharacter::SERVEUR_setDirectionAnim_Validate(int nDir)
{
	return true;
}


//Le joueur souhaite tirer
void AJeuMultiCharacter::tirer() {

	setTire(true);

	setDirectionAnim(ANIM_SHOOT);

}

//Le joueur ne souhaite plus
void AJeuMultiCharacter::tirerDown() {

	setDirectionAnim(ANIM_IDLE);
}

//Renvoie si le joueur souhaite tirer
bool AJeuMultiCharacter::getTire()
{
	return tire;
}


//Envoie au serveur que le joueur souhaite tirer
void AJeuMultiCharacter::setTire(bool nT)
{
	if (!mort) {
		tire = nT;

		if (Role < ROLE_Authority)
		{
			SERVEUR_setTire(nT);
		}
	}
}

//Partie serveur de la fonction
void AJeuMultiCharacter::SERVEUR_setTire_Implementation(bool nT)
{
	setTire(nT);
}

bool AJeuMultiCharacter::SERVEUR_setTire_Validate(bool nT)
{
	return true;
}

void AJeuMultiCharacter::afficheTableauScore() {
	afficherTableauScore = true;
}

void AJeuMultiCharacter::cacheTableauScore() {
	afficherTableauScore = false;
}

//Envoie au serveur la rotation acteulle de la caméra
void AJeuMultiCharacter::updateRotationCamera(FRotator nRC)
{
	rotationCamera = nRC;

	if (Role < ROLE_Authority)
	{
		SERVEUR_updateRotationCamera(nRC);
	}
}

//Partie serveur de la fonction
void AJeuMultiCharacter::SERVEUR_updateRotationCamera_Implementation(FRotator nRC)
{
	updateRotationCamera(nRC);
}

bool AJeuMultiCharacter::SERVEUR_updateRotationCamera_Validate(FRotator nRC)
{
		return true;
}

//Connection au serveur
void AJeuMultiCharacter::Connect(FString servAdresse, FString pseudo) {

	//verifie si le pseudo choisi n'est ni trop court, ni trop long
	if (pseudo.Len() < 2) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Le pseudo est trop court (Min 2 caracteres)");
		return;
	}
	else if (pseudo.Len() > 20) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Le pseudo est trop long (Max 20 caracteres)");
		return;
	}

	if (!servAdresse.IsEmpty()) {
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController) {
			PlayerController->ClientTravel(servAdresse + ":7777?Pseudo=" + pseudo, ETravelType::TRAVEL_Absolute); //Connection à l'adresse servAdresse avec le pseudo en option
		}
	}
}

void AJeuMultiCharacter::SERVEUR_meurt_Implementation(FVector force) {
	this->GetMesh()->SetSkeletalMesh(MeshTPS);
	this->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	this->GetMesh()->SetAnimInstanceClass(animBPTPS->GeneratedClass);
	this->directionAnim = ANIM_IDLE;
	this->GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("root"), true);
	this->GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("root"), true);
	this->GetMesh()->AddImpulse(force * 2, FName("spine_02"));
	this->GetMesh()->SetCollisionProfileName(FName("Ragdoll"));

	this->mort = true;
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AJeuMultiCharacter::SERVEUR_meurt_Validate(FVector force) {
	return true;
}

void AJeuMultiCharacter::respawner() {
	if (mort)
		setRespawn(true);
}

void AJeuMultiCharacter::setRespawn(bool nR) {
	if (mort) {
		respawn = nR;

		if (Role < ROLE_Authority)
		{
			SERVEUR_setRespawn(nR);
		}
	}
}

void AJeuMultiCharacter::SERVEUR_setRespawn_Implementation(bool nR) {
	setRespawn(nR);
}

bool AJeuMultiCharacter::SERVEUR_setRespawn_Validate(bool nR) {
	return true;
}

void AJeuMultiCharacter::SERVEUR_creerImpactBalle_Implementation(AActor* acteur, FVector pos, FRotator rot) {
	UGameplayStatics::SpawnDecalAtLocation(acteur, decalMaterial, FVector(3.f, 20.f, 20.f), pos, rot, 30.f);
}

bool AJeuMultiCharacter::SERVEUR_creerImpactBalle_Validate(AActor* acteur, FVector pos, FRotator rot) {
	return true;
}