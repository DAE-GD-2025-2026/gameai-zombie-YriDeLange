// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombies/BaseZombie.h"
#include "GameFramework/Pawn.h"

static const FName TargetEnemyKey{ "TargetEnemy" };
static const FName TargetEnemyLocationKey{ "TargetEnemyLocation" };

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

UBlackboardComponent* UStudentPerceptor::GetBlackboard() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
		if (AAIController* AI = Cast<AAIController>(Pawn->GetController()))
			return AI->GetBlackboardComponent();
	return nullptr;
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("Perception fired for %s | Sensed=%d"),
		*GetNameSafe(Actor), Stimulus.WasSuccessfullySensed());

	UBlackboardComponent* BB = GetBlackboard();
	if (!BB)
	{
		UE_LOG(LogTemp, Error, TEXT("  -> Blackboard is NULL"));
		return;
	}
	if (!Actor) return;

	if (Actor->IsA<ABaseZombie>())
	{
		UE_LOG(LogTemp, Warning, TEXT("  -> It's a zombie, writing keys"));
		if (Stimulus.WasSuccessfullySensed())
		{
			BB->SetValueAsObject(TargetEnemyKey, Actor);
			BB->SetValueAsVector(TargetEnemyLocationKey, Stimulus.StimulusLocation);
		}
		else
		{
			BB->ClearValue(TargetEnemyKey);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  -> Not a zombie (class: %s)"), *Actor->GetClass()->GetName());
	}
}