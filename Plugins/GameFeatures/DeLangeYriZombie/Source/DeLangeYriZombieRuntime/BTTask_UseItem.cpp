// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UseItem.h"
#include "AIController.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UBTTask_UseItem::UBTTask_UseItem()
{
	NodeName = "Use Item";
}

EBTNodeResult::Type UBTTask_UseItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return EBTNodeResult::Failed;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return EBTNodeResult::Failed;

	UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Inv) return EBTNodeResult::Failed;

	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] && Items[i]->GetItemType() == ItemTypeToUse && Items[i]->GetValue() > 0)
		{
			if (Inv->UseItem(i))
			{
				if (Items[i] && Items[i]->GetValue() == 0)
				{
					Inv->RemoveItem(i);
				}
				return EBTNodeResult::Succeeded;
			}
			return EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::Failed;
}