// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "House.generated.h"

USTRUCT(BlueprintType)
struct GAMEAI_ZOMBIE_API FHouseBounds
{
	GENERATED_BODY()
	
	FVector Origin;
	FVector Extent;
};

UCLASS()
class GAMEAI_ZOMBIE_API AHouse : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHouse();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FHouseBounds GetBounds() const;
};
