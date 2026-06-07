#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SelectThreat.generated.h"

UCLASS()
class DELANGEYRIZOMBIERUNTIME_API UBTService_SelectThreat : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectThreat();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetEnemyKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Threat")
	float ForgetTime{ 5.f };
};