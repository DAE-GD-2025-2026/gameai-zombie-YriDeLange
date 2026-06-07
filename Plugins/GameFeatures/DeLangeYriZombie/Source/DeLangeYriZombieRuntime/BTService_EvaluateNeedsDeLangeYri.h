// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Items/ItemType.h"
#include "BTService_EvaluateNeedsDeLangeYri.generated.h"

class UInventoryComponent;

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTService_EvaluateNeedsDeLangeYri : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_EvaluateNeedsDeLangeYri();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HealthFractionKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StaminaFractionKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldHealKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldEatKey;

	UPROPERTY(EditAnywhere, Category = "Needs")
	float HealHealthThreshold{ 0.3f };
	UPROPERTY(EditAnywhere, Category = "Needs")
	float EatStaminaThreshold{ 0.3f };

private:
	static bool InventoryHasUsable(const UInventoryComponent* Inv, EItemType Type);
};
