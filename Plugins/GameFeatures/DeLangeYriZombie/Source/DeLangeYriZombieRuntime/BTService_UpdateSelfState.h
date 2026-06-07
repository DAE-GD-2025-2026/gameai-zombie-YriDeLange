// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateSelfState.generated.h"

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTService_UpdateSelfState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateSelfState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HealthFractionKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StaminaFractionKey;
};
