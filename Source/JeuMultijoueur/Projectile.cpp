// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "Projectile.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	visuel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Representation"));
	RootComponent = visuel;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (BoxVisualAsset.Succeeded())
	{
		visuel->SetStaticMesh(BoxVisualAsset.Object);
		visuel->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visuel->SetWorldScale3D(FVector(0.05f));
	}

	bReplicates = true;
	bReplicateMovement = true;
	
	InitialLifeSpan = 500.f;

}

void AProjectile::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, visuel);

}
