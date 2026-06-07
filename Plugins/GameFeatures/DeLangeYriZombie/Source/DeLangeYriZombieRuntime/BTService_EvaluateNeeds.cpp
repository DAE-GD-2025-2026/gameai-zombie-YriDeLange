// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_EvaluateNeeds.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UBTService_EvaluateNeeds::UBTService_EvaluateNeeds()
{
	NodeName = "Evaluate Needs";
	Interval = 0.2f;
	RandomDeviation = 0.05f;

	HealthFractionKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EvaluateNeeds, HealthFractionKey));
	StaminaFractionKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EvaluateNeeds, StaminaFractionKey));
	ShouldHealKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EvaluateNeeds, ShouldHealKey));
	ShouldEatKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_EvaluateNeeds, ShouldEatKey));
}

bool UBTService_EvaluateNeeds::InventoryHasUsable(const UInventoryComponent* Inv, EItemType Type)
{
	for (const ABaseItem* Item : Inv->GetInventory())
	{
		if (Item && Item->GetItemType() == Type && Item->GetValue() > 0)
		{
			return true;
		}
	}
	return false;
}

void UBTService_EvaluateNeeds::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return;

	UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Inv) return;

	const float Health = BB->GetValueAsFloat(HealthFractionKey.SelectedKeyName);
	const float Stamina = BB->GetValueAsFloat(StaminaFractionKey.SelectedKeyName);

	const bool bShouldHeal = (Health < HealHealthThreshold) && InventoryHasUsable(Inv, EItemType::Medkit);
	const bool bShouldEat = (Stamina < EatStaminaThreshold) && InventoryHasUsable(Inv, EItemType::Food);

	BB->SetValueAsBool(ShouldHealKey.SelectedKeyName, bShouldHeal);
	BB->SetValueAsBool(ShouldEatKey.SelectedKeyName, bShouldEat);
}