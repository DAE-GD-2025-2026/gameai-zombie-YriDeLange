#include "BTTask_FleeFromThreat.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"

UBTTask_FleeFromThreat::UBTTask_FleeFromThreat()
{
	NodeName = "Flee From Threat";
	bNotifyTick = true;
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FleeFromThreat, TargetLocationKey));
}

EBTNodeResult::Type UBTTask_FleeFromThreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedPath.Reset();
	PathIndex = 0;
	return EBTNodeResult::InProgress;
}

void UBTTask_FleeFromThreat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	const FVector ThreatLocation = BB->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	const FVector MyLocation = Survivor->GetActorLocation();

	FVector AwayDir = MyLocation - ThreatLocation;
	AwayDir.Z = 0.f;

	if (AwayDir.Size() >= SafeDistance)
	{
		BB->ClearValue(TargetLocationKey.SelectedKeyName);
		Survivor->StopRunning();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	const FVector FleeTarget = MyLocation + AwayDir.GetSafeNormal() * FleeProjectDistance;
	CachedPath = Survivor->CalculatePath(FleeTarget);
	PathIndex = (CachedPath.Num() > 1) ? 1 : 0;

	Survivor->StartRunning();

	if (CachedPath.IsValidIndex(PathIndex))
	{
		FVector ToWaypoint = CachedPath[PathIndex] - MyLocation;
		ToWaypoint.Z = 0.f;
		const FVector MoveDir = ToWaypoint.GetSafeNormal();

		const FRotator NewRot = FMath::RInterpTo(Survivor->GetActorRotation(), MoveDir.Rotation(), DeltaSeconds, 8.f);
		Survivor->SetActorRotation(NewRot);

		Survivor->AddMovementInput(MoveDir, 1.f);
	}
	else
	{
		Survivor->AddMovementInput(AwayDir.GetSafeNormal(), 1.f);
	}
}