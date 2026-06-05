// Fill out your copyright notice in the Description page of Project Settings.


#include "Medkit.h"

#include "Common/HealthComponent.h"


// Sets default values
AMedkit::AMedkit()
{
	ItemType = EItemType::Medkit;
	Value = FMath::RandRange(1, 5);
}

void AMedkit::UseItem(ASurvivorPawn& Survivor)
{
	Survivor.GetComponentByClass<UHealthComponent>()->HealDamage(GetValue());
	Value = 0;
}


