// Fill out your copyright notice in the Description page of Project Settings.


#include "House.h"


// Sets default values
AHouse::AHouse()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHouse::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHouse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FHouseBounds AHouse::GetBounds() const
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	return {Origin, Extent};
}
