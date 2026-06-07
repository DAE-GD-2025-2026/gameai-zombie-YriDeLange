// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptor.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

void UStudentPerceptor::RefreshThreatSeen(ABaseZombie* Zombie)
{
	if (!Zombie) return;
	const float Now = GetWorld()->GetTimeSeconds();
	for (FKnownThreat& Threat : KnownThreats)
	{
		if (Threat.Zombie == Zombie)
		{
			Threat.LastSeenLocation = Zombie->GetActorLocation();
			Threat.LastSeenTime = Now;
			return;
		}
	}
	RememberThreat(Zombie, Zombie->GetActorLocation());
}

void UStudentPerceptor::MarkHouseSearched(AHouse* House)
{
	for (FKnownHouse& Known : KnownHouses)
	{
		if (Known.House == House)
		{
			Known.bEverSearched = true;
			Known.LastSearchedTime = GetWorld()->GetTimeSeconds();
			return;
		}
	}
}

void UStudentPerceptor::RememberThreat(ABaseZombie* Zombie, const FVector& Location)
{
	const float Now = GetWorld()->GetTimeSeconds();

	for (FKnownThreat& Threat : KnownThreats)
	{
		if (Threat.Zombie == Zombie)
		{
			Threat.LastSeenLocation = Location;
			Threat.LastSeenTime = Now;
			return;
		}
	}

	FKnownThreat NewThreat;
	NewThreat.Zombie = Zombie;
	NewThreat.LastSeenLocation = Location;
	NewThreat.LastSeenTime = Now;
	KnownThreats.Add(NewThreat);
}

void UStudentPerceptor::RememberItem(ABaseItem* Item, const FVector& Location)
{
	for (FKnownItem& Known : KnownItems)
	{
		if (Known.Item == Item)
		{
			Known.Location = Location;
			return;
		}
	}

	FKnownItem NewItem;
	NewItem.Item = Item;
	NewItem.Type = Item->GetItemType();
	NewItem.Location = Location;
	KnownItems.Add(NewItem);
}

void UStudentPerceptor::RememberHouse(AHouse* House)
{
	for (const FKnownHouse& Known : KnownHouses)
	{
		if (Known.House == House)
		{
			return;
		}
	}

	FKnownHouse NewHouse;
	NewHouse.House = House;
	NewHouse.Location = House->GetBounds().Origin;
	KnownHouses.Add(NewHouse);
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	if (!Stimulus.WasSuccessfullySensed())
	{
		return;
	}

	if (ABaseZombie* Zombie = Cast<ABaseZombie>(Actor))
	{
		RememberThreat(Zombie, Stimulus.StimulusLocation);
	}
	else if (ABaseItem* Item = Cast<ABaseItem>(Actor))
	{
		RememberItem(Item, Stimulus.StimulusLocation);
	}
	else if (AHouse* House = Cast<AHouse>(Actor))
	{
		RememberHouse(House);
	}
}