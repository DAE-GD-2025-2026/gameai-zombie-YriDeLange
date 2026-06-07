// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SelectLoot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "StudentPerceptor.h"

UBTService_SelectLoot::UBTService_SelectLoot()
{
	NodeName = "Select Loot";
	Interval = 0.2f;
	RandomDeviation = 0.05f;

	LootTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SelectLoot, LootTargetKey), ABaseItem::StaticClass());
	HealthFractionKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SelectLoot, HealthFractionKey));
}

int32 UBTService_SelectLoot::FindFreeSlot(const UInventoryComponent* Inv)
{
	const TArray<ABaseItem*>& Items = Inv->GetInventory();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] == nullptr) return i;
	}
	return -1;
}

bool UBTService_SelectLoot::HasUsableWeapon(const UInventoryComponent* Inv)
{
	for (const ABaseItem* Item : Inv->GetInventory())
	{
		if (!Item) continue;
		const EItemType T = Item->GetItemType();
		if ((T == EItemType::Shotgun || T == EItemType::Pistol) && Item->GetValue() > 0)
		{
			return true;
		}
	}
	return false;
}

ABaseItem* UBTService_SelectLoot::PickBestItem(ASurvivorPawn* Survivor, float HealthFraction) const
{
	UStudentPerceptor* Perceptor = Survivor->GetComponentByClass<UStudentPerceptor>();
	UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Perceptor || !Inv) return nullptr;

	if (FindFreeSlot(Inv) == -1) return nullptr;

	const bool bHurt = HealthFraction < HurtHealthThreshold;
	const bool bUnarmed = !HasUsableWeapon(Inv);
	const FVector MyLoc = Survivor->GetActorLocation();

	ABaseItem* Best = nullptr;
	float BestScore = -TNumericLimits<float>::Max();

	for (const FKnownItem& Known : Perceptor->GetKnownItems())
	{
		ABaseItem* Item = Known.Item;
		if (!Item || Item->IsHidden() || Item->GetValue() <= 0) continue;

		const EItemType Type = Item->GetItemType();
		float Priority;
		switch (Type)
		{
		case EItemType::Medkit:  Priority = bHurt ? MedkitPriorityHurt : MedkitPriorityHealthy; break;
		case EItemType::Shotgun:
		case EItemType::Pistol:  Priority = bUnarmed ? WeaponPriorityUnarmed : WeaponPriorityArmed; break;
		case EItemType::Food:    Priority = FoodPriority; break;
		default: continue;
		}

		const float Dist = FVector::Dist(MyLoc, Item->GetActorLocation());
		const float Score = Priority - Dist * DistanceWeight;
		if (Score > BestScore)
		{
			BestScore = Score;
			Best = Item;
		}
	}

	return Best;
}

void UBTService_SelectLoot::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return;

	const float HealthFraction = BB->GetValueAsFloat(HealthFractionKey.SelectedKeyName);

	if (ABaseItem* Best = PickBestItem(Survivor, HealthFraction))
	{
		BB->SetValueAsObject(LootTargetKey.SelectedKeyName, Best);
	}
	else
	{
		BB->ClearValue(LootTargetKey.SelectedKeyName);
	}
}