// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_LootItem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UBTTask_LootItem::UBTTask_LootItem()
{
	NodeName = "Loot Item";
	bNotifyTick = true;
	bCreateNodeInstance = true;
	LootTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LootItem, LootTargetKey), ABaseItem::StaticClass());
}

static int32 FindFreeSlot(const UInventoryComponent* Inv)
{
	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] == nullptr) return i;
	}
	return -1;
}

EBTNodeResult::Type UBTTask_LootItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return EBTNodeResult::Failed;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return EBTNodeResult::Failed;

	TargetItem = Cast<ABaseItem>(BB->GetValueAsObject(LootTargetKey.SelectedKeyName));
	if (!TargetItem || TargetItem->IsHidden() || TargetItem->GetValue() <= 0)
	{
		return EBTNodeResult::Failed;
	}

	UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>();
	const float Acceptance = Inv ? Inv->GetPickupRange() : 100.f;

	if (AI->MoveToActor(TargetItem, Acceptance) == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	Survivor->StopRunning();
	return EBTNodeResult::InProgress;
}

void UBTTask_LootItem::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	if (!TargetItem || TargetItem->IsHidden() || TargetItem->GetValue() <= 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector Vel = Survivor->GetVelocity();
	Vel.Z = 0.f;
	if (!Vel.IsNearlyZero())
	{
		const FRotator NewRot = FMath::RInterpTo(Survivor->GetActorRotation(), Vel.Rotation(), DeltaSeconds, 6.f);
		Survivor->SetActorRotation(NewRot);
	}

	if (AI->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>();
		const int32 Slot = Inv ? FindFreeSlot(Inv) : -1;
		const bool bGrabbed = (Slot != -1) && Inv->GrabItem(Slot, TargetItem);
		FinishLatentTask(OwnerComp, bGrabbed ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UBTTask_LootItem::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AI = OwnerComp.GetAIOwner())
	{
		AI->StopMovement();
	}
	return EBTNodeResult::Aborted;
}