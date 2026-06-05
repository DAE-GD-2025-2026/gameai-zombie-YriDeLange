// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

#include "Engine/DamageEvents.h"

APistol::APistol()
{
	ItemType = EItemType::Pistol;
	Damage = 5;
	Value = FMath::RandRange(4, 10);
}

void APistol::UseItem(ASurvivorPawn& Survivor)
{
	if (GetValue() <= 0)
	{
		return;
	}
	
	--Value; // Ammo
	if (APawn* HitPawn{}; Shoot(Survivor, Survivor.GetActorForwardVector(), HitPawn))
	{
		HitPawn->TakeDamage(Damage, FDamageEvent{}, Survivor.GetController(), &Survivor);
	}
}

