// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "Engine/DamageEvents.h"


// Sets default values
AShotgun::AShotgun()
{
	ItemType = EItemType::Shotgun;
	Damage = 2;
	Value = FMath::RandRange(2, 4);
}

void AShotgun::UseItem(ASurvivorPawn& Survivor)
{
	if (GetValue() <= 0)
	{
		return;
	}
	
	--Value; // Ammo
	
	bool bHasHitAnyShot = false;
	for (int Shot = 0; Shot < ShotsPerAmmo; ++Shot)
	{
		auto ShotDirection = Survivor.GetActorForwardVector();
		float const RandomSprayDelta = FMath::RandRange(-MaxSprayDelta, MaxSprayDelta);
		ShotDirection = ShotDirection.ToOrientationRotator().Add(0, RandomSprayDelta, 0).Vector();
		if (APawn* HitPawn{}; Shoot(Survivor, ShotDirection, HitPawn))
		{
			HitPawn->TakeDamage(Damage, FDamageEvent{}, Survivor.GetController(), &Survivor);
			bHasHitAnyShot = true;
		}
	}
}

