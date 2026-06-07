// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FightTargetDeLangeYri.generated.h"

class ABaseZombie;

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTTask_FightTargetDeLangeYri : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FightTargetDeLangeYri();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetEnemyKey;

	UPROPERTY(EditAnywhere, Category = "Fight")
	float TurnSpeed{ 10.f };

	UPROPERTY(EditAnywhere, Category = "Fight")
	float FireAngleTolerance{ 8.f };

	UPROPERTY(EditAnywhere, Category = "Fight")
	float FireCooldown{ 0.5f };

private:
	float LastFireTime{ -1000.f };
};