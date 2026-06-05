// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "SurvivorPawn.generated.h"

class UInventoryComponent;

UCLASS()
class GAMEAI_ZOMBIE_API ASurvivorPawn : public APawn
{
	GENERATED_BODY()

public:
	ASurvivorPawn();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInventoryComponent* InventoryComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHealthComponent* HealthComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaminaComponent* StaminaComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsRunning{false};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UFloatingPawnMovement* FloatingPawnMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RunningSpeed{600.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DefaultSpeed{400.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAIPerceptionComponent* PerceptionComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAISenseConfig_Damage* DamageConfig;
	
	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	TArray<FVector> CalculatePath(const FVector& TargetLocation) const;
	void StartRunning();
	void StopRunning();
	
	bool IsRunning() const;
};
