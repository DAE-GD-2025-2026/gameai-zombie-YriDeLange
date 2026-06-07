// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Zombies/BaseZombie.h"
#include "Items/BaseItem.h"
#include "Village/House/House.h"
#include "StudentPerceptor.generated.h"

USTRUCT()
struct FKnownThreat
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABaseZombie> Zombie = nullptr;

	FVector LastSeenLocation = FVector::ZeroVector;
	float LastSeenTime = 0.f;
};

USTRUCT()
struct FKnownItem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABaseItem> Item = nullptr;

	EItemType Type = EItemType::Garbage;
	FVector Location = FVector::ZeroVector;
};

USTRUCT()
struct FKnownHouse
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AHouse> House = nullptr;

	FVector Location = FVector::ZeroVector;
	bool bEverSearched = false;
	float LastSearchedTime = 0.f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DELANGEYRIZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()

public:
	UStudentPerceptor();

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	const TArray<FKnownThreat>& GetKnownThreats() const { return KnownThreats; }
	const TArray<FKnownItem>& GetKnownItems() const { return KnownItems; }
	const TArray<FKnownHouse>& GetKnownHouses() const { return KnownHouses; }

	void RefreshThreatSeen(ABaseZombie* Zombie);
	void MarkHouseSearched(AHouse* House);

protected:
	void RememberThreat(ABaseZombie* Zombie, const FVector& Location);
	void RememberItem(ABaseItem* Item, const FVector& Location);
	void RememberHouse(AHouse* House);

	UPROPERTY()
	TArray<FKnownThreat> KnownThreats;

	UPROPERTY()
	TArray<FKnownItem> KnownItems;

	UPROPERTY()
	TArray<FKnownHouse> KnownHouses;
};