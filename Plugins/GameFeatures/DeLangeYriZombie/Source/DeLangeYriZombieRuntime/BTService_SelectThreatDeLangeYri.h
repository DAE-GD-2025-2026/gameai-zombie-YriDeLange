#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SelectThreatDeLangeYri.generated.h"

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTService_SelectThreatDeLangeYri : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectThreatDeLangeYri();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetEnemyKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Threat")
	float ForgetTime{ 5.f };
};