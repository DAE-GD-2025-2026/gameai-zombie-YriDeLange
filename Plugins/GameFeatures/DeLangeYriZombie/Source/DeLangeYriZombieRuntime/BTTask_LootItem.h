// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LootItem.generated.h"

class ABaseItem;

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTTask_LootItem : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_LootItem();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LootTargetKey;

private:
	UPROPERTY()
	TObjectPtr<ABaseItem> TargetItem = nullptr;
};
