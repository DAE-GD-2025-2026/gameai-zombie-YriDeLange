// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExploreDeLangeYri.generated.h"

class ASurvivorPawn;
class AHouse;
class AAIController;

UENUM()
enum class EExploreState : uint8
{
	Choosing,
	Moving,
	Searching
};

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTTask_ExploreDeLangeYri : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExploreDeLangeYri();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Explore")
	float ExploreRadius{ 2000.f };

	UPROPERTY(EditAnywhere, Category = "Explore")
	float MinExploreDistance{ 600.f };

	UPROPERTY(EditAnywhere, Category = "Explore")
	float AcceptanceRadius{ 80.f };

	UPROPERTY(EditAnywhere, Category = "Explore")
	float ReSearchInterval{ 60.f };

	UPROPERTY(EditAnywhere, Category = "Explore")
	float SpinSpeed{ 180.f };

private:
	bool BeginMoveToDestination(AAIController* AI, ASurvivorPawn* Survivor);
	void FaceVelocity(ASurvivorPawn* Survivor, float DeltaSeconds) const;

	EExploreState State{ EExploreState::Choosing };
	float AccumulatedSpin{ 0.f };

	UPROPERTY()
	TObjectPtr<AHouse> TargetHouse = nullptr;
};
