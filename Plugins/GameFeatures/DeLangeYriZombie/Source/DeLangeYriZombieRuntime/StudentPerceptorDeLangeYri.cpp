// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptorDeLangeYri.h"

UStudentPerceptorDeLangeYri::UStudentPerceptorDeLangeYri()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStudentPerceptorDeLangeYri::BeginPlay()
{
	Super::BeginPlay();

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptorDeLangeYri::OnPerceptionUpdated);
	}
}

void UStudentPerceptorDeLangeYri::RefreshThreatSeen(ABaseZombie* Zombie)
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

void UStudentPerceptorDeLangeYri::MarkHouseSearched(AHouse* House)
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

void UStudentPerceptorDeLangeYri::RememberThreat(ABaseZombie* Zombie, const FVector& Location)
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

void UStudentPerceptorDeLangeYri::RememberItem(ABaseItem* Item, const FVector& Location)
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

void UStudentPerceptorDeLangeYri::RememberHouse(AHouse* House)
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

void UStudentPerceptorDeLangeYri::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
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