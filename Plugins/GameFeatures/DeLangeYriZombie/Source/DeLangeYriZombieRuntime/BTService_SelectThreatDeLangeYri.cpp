#include "BTService_SelectThreatDeLangeYri.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "StudentPerceptorDeLangeYri.h"
#include "Zombies/BaseZombie.h"

UBTService_SelectThreatDeLangeYri::UBTService_SelectThreatDeLangeYri()
{
	NodeName = "Select Threat";
	Interval = 0.2f;
	RandomDeviation = 0.05f;

	TargetEnemyKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SelectThreatDeLangeYri, TargetEnemyKey), ABaseZombie::StaticClass());
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SelectThreatDeLangeYri, TargetLocationKey));
}

void UBTService_SelectThreatDeLangeYri::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) return;

	UStudentPerceptorDeLangeYri* Perceptor = Survivor->GetComponentByClass<UStudentPerceptorDeLangeYri>();
	if (!Perceptor) return;

	const FVector MyLocation = Survivor->GetActorLocation();
	const float Now = Survivor->GetWorld()->GetTimeSeconds();

	const FKnownThreat* Best = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (const FKnownThreat& Threat : Perceptor->GetKnownThreats())
	{
		if (!Threat.Zombie) continue;
		if (Now - Threat.LastSeenTime > ForgetTime) continue;

		const float Score = FVector::Dist(MyLocation, Threat.LastSeenLocation);
		if (Score < BestScore)
		{
			BestScore = Score;
			Best = &Threat;
		}
	}

	if (Best)
	{
		BB->SetValueAsObject(TargetEnemyKey.SelectedKeyName, Best->Zombie);
		BB->SetValueAsVector(TargetLocationKey.SelectedKeyName, Best->LastSeenLocation);
	}
	else
	{
		BB->ClearValue(TargetEnemyKey.SelectedKeyName);
		BB->ClearValue(TargetLocationKey.SelectedKeyName);
	}
}