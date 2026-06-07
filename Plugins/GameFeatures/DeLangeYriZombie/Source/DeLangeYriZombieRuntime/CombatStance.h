#pragma once

#include "CoreMinimal.h"
#include "CombatStance.generated.h"

UENUM(BlueprintType)
enum class ECombatStance : uint8
{
	None  UMETA(DisplayName = "None"),
	Flee  UMETA(DisplayName = "Flee"),
	Fight UMETA(DisplayName = "Fight")
};