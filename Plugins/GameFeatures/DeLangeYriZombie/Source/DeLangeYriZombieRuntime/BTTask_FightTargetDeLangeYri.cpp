// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FightTargetDeLangeYri.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Zombies/BaseZombie.h"

UBTTask_FightTargetDeLangeYri::UBTTask_FightTargetDeLangeYri()
{
	NodeName = "Fight Target";
	bNotifyTick = true;
	bCreateNodeInstance = true;
	TargetEnemyKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FightTargetDeLangeYri, TargetEnemyKey), ABaseZombie::StaticClass());
}

EBTNodeResult::Type UBTTask_FightTargetDeLangeYri::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_FightTargetDeLangeYri::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AI->GetPawn());
	if (!Survivor) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	ABaseZombie* Target = Cast<ABaseZombie>(BB->GetValueAsObject(TargetEnemyKey.SelectedKeyName));
	if (!Target) { FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); return; }

	FVector ToTarget = Target->GetActorLocation() - Survivor->GetActorLocation();
	ToTarget.Z = 0.f;
	if (ToTarget.IsNearlyZero()) return;

	const FRotator DesiredRot = ToTarget.Rotation();
	const FRotator NewRot = FMath::RInterpTo(Survivor->GetActorRotation(), DesiredRot, DeltaSeconds, TurnSpeed);
	Survivor->SetActorRotation(NewRot);

	Survivor->StopRunning();

	const float YawError = FMath::Abs(FMath::FindDeltaAngleDegrees(NewRot.Yaw, DesiredRot.Yaw));
	if (YawError <= FireAngleTolerance)
	{
		const float Now = Survivor->GetWorld()->GetTimeSeconds();
		if (Now - LastFireTime >= FireCooldown)
		{
			if (UInventoryComponent* Inv = Survivor->GetComponentByClass<UInventoryComponent>())
			{
				const TArray<ABaseItem*>& Items = Inv->GetInventory();
				for (int32 i = 0; i < Items.Num(); ++i)
				{
					if (Items[i])
					{
						const EItemType T = Items[i]->GetItemType();
						if ((T == EItemType::Shotgun || T == EItemType::Pistol) && Items[i]->GetValue() > 0)
						{
							Inv->UseItem(i);
							LastFireTime = Now;

							if (Items[i] && Items[i]->GetValue() == 0)
							{
								Inv->RemoveItem(i);
							}
							break;
						}
					}
				}
			}
		}
	}
}