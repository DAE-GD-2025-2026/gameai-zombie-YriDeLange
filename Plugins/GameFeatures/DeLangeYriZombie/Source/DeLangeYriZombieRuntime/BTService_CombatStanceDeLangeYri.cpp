// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CombatStanceDeLangeYri.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "StudentPerceptorDeLangeYri.h"
#include "CombatStanceDeLangeYri.h"

UBTService_CombatStanceDeLangeYri::UBTService_CombatStanceDeLangeYri()
{
	NodeName = "Combat Stance";
	Interval = 0.2f;
	RandomDeviation = 0.05f;
	StanceKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CombatStanceDeLangeYri, StanceKey));
}

bool UBTService_CombatStanceDeLangeYri::HasUsableWeapon(const UInventoryComponent* Inv)
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

void UBTService_CombatStanceDeLangeYri::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return;

	UStudentPerceptorDeLangeYri* Perceptor = Survivor->GetComponentByClass<UStudentPerceptorDeLangeYri>();
	UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Perceptor || !Inv) return;

	if (UAIPerceptionComponent* Perception = Survivor->GetComponentByClass<UAIPerceptionComponent>())
	{
		TArray<AActor*> SeenNow;
		Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SeenNow);
		for (AActor* Seen : SeenNow)
		{
			if (ABaseZombie* Zombie = Cast<ABaseZombie>(Seen))
			{
				if (const UHealthComponent* H = Zombie->GetComponentByClass<UHealthComponent>())
				{
					if (H->IsDead()) continue;
				}
				Perceptor->RefreshThreatSeen(Zombie);
			}
		}
	}

	const float Now = Survivor->GetWorld()->GetTimeSeconds();
	int32 RecentThreats = 0;
	for (const FKnownThreat& Threat : Perceptor->GetKnownThreats())
	{
		if (!Threat.Zombie) continue;
		if (const UHealthComponent* H = Threat.Zombie->GetComponentByClass<UHealthComponent>())
		{
			if (H->IsDead()) continue;
		}
		if (Now - Threat.LastSeenTime <= ThreatRecency)
		{
			++RecentThreats;
		}
	}

	ECombatStance Stance;
	if (RecentThreats == 0)
	{
		Stance = ECombatStance::None;
	}
	else if (HasUsableWeapon(Inv) && RecentThreats <= MaxFightableThreats)
	{
		Stance = ECombatStance::Fight;
	}
	else
	{
		Stance = ECombatStance::Flee;
	}

	BB->SetValueAsInt(StanceKey.SelectedKeyName, static_cast<int32>(Stance));
}