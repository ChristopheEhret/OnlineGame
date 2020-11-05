// Fill out your copyright notice in the Description page of Project Settings.

#include "JeuMultijoueur.h"
#include "ArmeCac.h"


// Sets default values
AArmeCac::AArmeCac()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	collision->AttachTo(RootComponent);
	collision->InitBoxExtent(FVector(20.f));
	collision->SetRelativeLocation(FVector(0.f));
}

// Called when the game starts or when spawned
void AArmeCac::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArmeCac::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

