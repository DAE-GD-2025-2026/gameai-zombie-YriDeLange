#include "BTTask_FleeFromThreatDeLangeYri.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "NavigationSystem.h"
#include "NavigationData.h"

UBTTask_FleeFromThreatDeLangeYri::UBTTask_FleeFromThreatDeLangeYri()
{
	NodeName = "Flee From Threat";
	bNotifyTick = true;
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FleeFromThreatDeLangeYri, TargetLocationKey));
}

EBTNodeResult::Type UBTTask_FleeFromThreatDeLangeYri::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedPath.Reset();
	PathIndex = 0;
	return EBTNodeResult::InProgress;
}

void UBTTask_FleeFromThreatDeLangeYri::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	if (!BB->IsVectorValueSet(TargetLocationKey.SelectedKeyName))
	{
		Survivor->StopRunning();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	const FVector ThreatLocation = BB->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	const FVector MyLocation = Survivor->GetActorLocation();

	FVector AwayDir = MyLocation - ThreatLocation;
	AwayDir.Z = 0.f;
	if (AwayDir.IsNearlyZero())
	{
		AwayDir = Survivor->GetActorForwardVector();
		AwayDir.Z = 0.f;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Survivor->GetWorld());
	FNavLocation FleeNavPoint;
	bool bHaveFleePoint = false;
	if (NavSys)
	{
		const FVector RawTarget = MyLocation + AwayDir.GetSafeNormal() * FleeProjectDistance;
		bHaveFleePoint = NavSys->ProjectPointToNavigation(RawTarget, FleeNavPoint, FVector(1000.f, 1000.f, 1000.f));
	}

	Survivor->StartRunning();

	if (bHaveFleePoint)
	{
		CachedPath = Survivor->CalculatePath(FleeNavPoint.Location);
		PathIndex = (CachedPath.Num() > 1) ? 1 : 0;
	}
	else
	{
		CachedPath.Reset();
	}

	if (CachedPath.IsValidIndex(PathIndex))
	{
		FVector ToWaypoint = CachedPath[PathIndex] - MyLocation;
		ToWaypoint.Z = 0.f;
		const FVector MoveDir = ToWaypoint.GetSafeNormal();

		FVector ToThreat = ThreatLocation - MyLocation;
		ToThreat.Z = 0.f;
		if (!ToThreat.IsNearlyZero())
		{
			const FRotator NewRot = FMath::RInterpTo(Survivor->GetActorRotation(), ToThreat.Rotation(), DeltaSeconds, 8.f);
			Survivor->SetActorRotation(NewRot);
		}

		Survivor->AddMovementInput(MoveDir, 1.f);
	}
}