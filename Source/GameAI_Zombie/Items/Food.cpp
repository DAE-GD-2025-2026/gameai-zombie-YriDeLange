// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"

#include "Common/StaminaComponent.h"


// Sets default values
AFood::AFood()
{
	ItemType = EItemType::Food;
	Value = FMath::RandRange(1, 5);
}

void AFood::UseItem(ASurvivorPawn& Survivor)
{
	Survivor.GetComponentByClass<UStaminaComponent>()->AddStamina(GetValue());
	Value = 0;
}

