// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Items/ItemType.h"
#include "BTService_SelectLootDeLangeYri.generated.h"

class ASurvivorPawn;
class ABaseItem;
class UInventoryComponent;

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTService_SelectLootDeLangeYri : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectLootDeLangeYri();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LootTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HealthFractionKey;

	UPROPERTY(EditAnywhere, Category = "Loot")
	float HurtHealthThreshold{ 0.5f };

	UPROPERTY(EditAnywhere, Category = "Loot|Priority")
	float MedkitPriorityHurt{ 1000.f };
	UPROPERTY(EditAnywhere, Category = "Loot|Priority")
	float MedkitPriorityHealthy{ 200.f };
	UPROPERTY(EditAnywhere, Category = "Loot|Priority")
	float WeaponPriorityUnarmed{ 800.f };
	UPROPERTY(EditAnywhere, Category = "Loot|Priority")
	float WeaponPriorityArmed{ 100.f };
	UPROPERTY(EditAnywhere, Category = "Loot|Priority")
	float FoodPriority{ 300.f };
	UPROPERTY(EditAnywhere, Category = "Loot|Priority")
	float DistanceWeight{ 0.1f };

private:
	ABaseItem* PickBestItem(ASurvivorPawn* Survivor, float HealthFraction) const;
	static int32 FindFreeSlot(const UInventoryComponent* Inv);
	static bool HasUsableWeapon(const UInventoryComponent* Inv);
};
