// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Explore.h"
#include "AIController.h"
#include "Survivor/SurvivorPawn.h"
#include "StudentPerceptor.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_Explore::UBTTask_Explore()
{
	NodeName = "Explore";
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Explore::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	State = EExploreState::Choosing;
	AccumulatedSpin = 0.f;
	TargetHouse = nullptr;
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Explore::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AI = OwnerComp.GetAIOwner())
	{
		AI->StopMovement();
	}
	return EBTNodeResult::Aborted;
}

bool UBTTask_Explore::BeginMoveToDestination(AAIController* AI, ASurvivorPawn* Survivor)
{
	TargetHouse = nullptr;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Survivor->GetWorld());
	if (!NavSys) return false;

	const FVector MyLocation = Survivor->GetActorLocation();

	if (UStudentPerceptor* Perceptor = Survivor->GetComponentByClass<UStudentPerceptor>())
	{
		const float Now = Survivor->GetWorld()->GetTimeSeconds();
		const FKnownHouse* Best = nullptr;
		float BestDist = TNumericLimits<float>::Max();

		for (const FKnownHouse& Known : Perceptor->GetKnownHouses())
		{
			if (!Known.House) continue;
			const bool bNeedsSearch = !Known.bEverSearched || (Now - Known.LastSearchedTime >= ReSearchInterval);
			if (!bNeedsSearch) continue;

			const float Dist = FVector::Dist(MyLocation, Known.Location);
			if (Dist < BestDist)
			{
				BestDist = Dist;
				Best = &Known;
			}
		}

		if (Best)
		{
			if (AI->MoveToLocation(Best->Location, AcceptanceRadius) != EPathFollowingRequestResult::Failed)
			{
				TargetHouse = Best->House;
				return true;
			}
		}
	}

	for (int32 Attempt = 0; Attempt < 5; ++Attempt)
	{
		FNavLocation RandomPoint;
		if (NavSys->GetRandomReachablePointInRadius(MyLocation, ExploreRadius, RandomPoint))
		{
			if (FVector::Dist(MyLocation, RandomPoint.Location) >= MinExploreDistance)
			{
				if (AI->MoveToLocation(RandomPoint.Location, AcceptanceRadius) != EPathFollowingRequestResult::Failed)
				{
					return true;
				}
			}
		}
	}

	return false;
}

void UBTTask_Explore::FaceVelocity(ASurvivorPawn* Survivor, float DeltaSeconds) const
{
	FVector Vel = Survivor->GetVelocity();
	Vel.Z = 0.f;
	if (!Vel.IsNearlyZero())
	{
		const FRotator NewRot = FMath::RInterpTo(Survivor->GetActorRotation(), Vel.Rotation(), DeltaSeconds, 6.f);
		Survivor->SetActorRotation(NewRot);
	}
}

void UBTTask_Explore::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return;

	Survivor->StopRunning();

	switch (State)
	{
	case EExploreState::Choosing:
		if (BeginMoveToDestination(AI, Survivor))
		{
			State = EExploreState::Moving;
		}
		break;

	case EExploreState::Moving:
		FaceVelocity(Survivor, DeltaSeconds);
		if (AI->GetMoveStatus() == EPathFollowingStatus::Idle)
		{
			if (TargetHouse)
			{
				AccumulatedSpin = 0.f;
				State = EExploreState::Searching;
			}
			else
			{
				State = EExploreState::Choosing;
			}
		}
		break;

	case EExploreState::Searching:
	{
		const float SpinThisTick = SpinSpeed * DeltaSeconds;
		AccumulatedSpin += SpinThisTick;
		Survivor->AddActorWorldRotation(FRotator(0.f, SpinThisTick, 0.f));

		if (AccumulatedSpin >= 360.f)
		{
			if (UStudentPerceptor* Perceptor = Survivor->GetComponentByClass<UStudentPerceptor>())
			{
				Perceptor->MarkHouseSearched(TargetHouse);
			}
			TargetHouse = nullptr;
			State = EExploreState::Choosing;
		}
	}
	break;
	}
}