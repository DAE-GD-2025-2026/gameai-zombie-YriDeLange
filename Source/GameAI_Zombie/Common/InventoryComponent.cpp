// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Items/ItemManager.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	Items.SetNum(5);
}

bool UInventoryComponent::GrabItem(int SlotIdx, ABaseItem* Item)
{
	if (SlotIdx >= GetInventoryCapacity() || 
		Item == nullptr ||
		Items[SlotIdx] != nullptr || 
		Items.Contains(Item))
	{
		return false;	
	}
	
	if (auto ItemManager = Cast<AItemManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemManager::StaticClass())))
	{
		ItemManager->UntrackItem(Item);
	}

	if (auto ItemStimuliSource = Item->GetComponentByClass<UAIPerceptionStimuliSourceComponent>())
	{
		ItemStimuliSource->UnregisterFromSense(UAISense_Sight::StaticClass());
		ItemStimuliSource->UnregisterFromPerceptionSystem();
	}
	Item->SetActorHiddenInGame(true);
	Item->SetActorEnableCollision(false);
	Items[SlotIdx] = Item;
	return true;
}

bool UInventoryComponent::UseItem(int SlotIdx)
{
	if (SlotIdx >= GetInventoryCapacity() ||
		Items[SlotIdx] == nullptr ||
		Items[SlotIdx]->GetValue() == 0)
	{
		return false;
	}
	
	Items[SlotIdx]->UseItem(*Cast<ASurvivorPawn>(GetOwner()));
	return true;
}

bool UInventoryComponent::RemoveItem(int SlotIdx)
{
	if (SlotIdx >= GetInventoryCapacity() || 
	Items[SlotIdx] == nullptr)
	{
		return false;
	}
	
	Items[SlotIdx]->Destroy();
	Items[SlotIdx] = nullptr;
	return true;
}


