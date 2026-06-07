// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CombatStance.generated.h"

class UInventoryComponent;

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTService_CombatStance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CombatStance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StanceKey;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ThreatRecency{ 5.f };

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 MaxFightableThreats{ 1 };

private:
	static bool HasUsableWeapon(const UInventoryComponent* Inv);
};
