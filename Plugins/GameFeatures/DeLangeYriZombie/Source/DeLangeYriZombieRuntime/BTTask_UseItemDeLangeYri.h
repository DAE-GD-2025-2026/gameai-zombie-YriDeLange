// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Items/ItemType.h"
#include "BTTask_UseItemDeLangeYri.generated.h"

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTTask_UseItemDeLangeYri : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_UseItemDeLangeYri();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Use Item")
	EItemType ItemTypeToUse{ EItemType::Medkit };
};
