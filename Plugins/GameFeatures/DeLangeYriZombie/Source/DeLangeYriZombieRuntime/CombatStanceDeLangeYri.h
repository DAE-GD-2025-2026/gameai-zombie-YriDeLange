#pragma once

#include "CoreMinimal.h"
#include "CombatStanceDeLangeYri.generated.h"

UENUM(BlueprintType)
enum class ECombatStance : uint8
{
	None  UMETA(DisplayName = "None"),
	Flee  UMETA(DisplayName = "Flee"),
	Fight UMETA(DisplayName = "Fight")
};