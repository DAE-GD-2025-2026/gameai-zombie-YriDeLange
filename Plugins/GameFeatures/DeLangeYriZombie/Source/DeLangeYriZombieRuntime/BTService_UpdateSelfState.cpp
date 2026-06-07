// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateSelfState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"

UBTService_UpdateSelfState::UBTService_UpdateSelfState()
{
	NodeName = "Update Self State";
	Interval = 0.2f;
	RandomDeviation = 0.05f;

	HealthFractionKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateSelfState, HealthFractionKey));
	StaminaFractionKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateSelfState, StaminaFractionKey));
}

void UBTService_UpdateSelfState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return;

	if (UHealthComponent* Health = Survivor->GetComponentByClass<UHealthComponent>())
	{
		const float Max = FMath::Max(1, Health->GetMaxHealth());
		BB->SetValueAsFloat(HealthFractionKey.SelectedKeyName, static_cast<float>(Health->GetHealth()) / Max);
	}

	if (UStaminaComponent* Stamina = Survivor->GetComponentByClass<UStaminaComponent>())
	{
		const float Max = FMath::Max(1.f, Stamina->GetMaxStamina());
		BB->SetValueAsFloat(StaminaFractionKey.SelectedKeyName, Stamina->GetCurrentStamina() / Max);
	}
}