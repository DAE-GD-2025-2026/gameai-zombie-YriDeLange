// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FleeFromThreat.generated.h"

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTTask_FleeFromThreat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FleeFromThreat();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Flee")
	float SafeDistance{ 1200.f };

	UPROPERTY(EditAnywhere, Category = "Flee")
	float FleeProjectDistance{ 1500.f };

	UPROPERTY(EditAnywhere, Category = "Flee")
	float WaypointAcceptance{ 100.f };

private:
	TArray<FVector> CachedPath;
	int32 PathIndex{ 0 };
};
