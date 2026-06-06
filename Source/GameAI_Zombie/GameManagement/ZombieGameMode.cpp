// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieGameMode.h"

#include "Items/ItemManager.h"
#include "Kismet/GameplayStatics.h"

AZombieGameMode::AZombieGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZombieGameMode::BeginPlay()
{
	Super::BeginPlay();
	Survivor = Cast<ASurvivorPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ASurvivorPawn::StaticClass()));
	
	SpawnVillages();
	TArray SpawnHouses = SpawnVillageAt(FVector::ZeroVector);
	for (AActor* House : SpawnHouses)
	{
		FBox HouseBox = House->GetComponentsBoundingBox(true);
		if (HouseBox.IsInside(Survivor->GetActorLocation()))
		{
			House->Destroy();
		}
	}
	
	auto ItemManager = Cast<AItemManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemManager::StaticClass()));
	ItemManager->RegisterHouses();
	
}

void AZombieGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TimeSinceLastSpawn += DeltaTime;
	if (IsValid(Survivor) && TimeSinceLastSpawn >= GetCurrentZombieSpawnInterval())
	{
		SpawnZombieAround(Survivor->GetActorLocation());
		TimeSinceLastSpawn = 0;
	}
	
	if (ShouldPurgeZonesSpawn())
	{
		TimeSinceLastPurgeSpawn += DeltaTime;
		if (TimeSinceLastPurgeSpawn >= FMath::Lerp(MaxPurgeInterval, MinPurgeInterval, GetCurrentPurgeAlpha()))
		{
			SpawnPurgeAround(Survivor->GetActorLocation());
			TimeSinceLastPurgeSpawn = 0;
		}
	}
}

void AZombieGameMode::SpawnVillages() const
{
	TArray<FVector> VillageCenters{};
	int FailedAttempts{};
	
	int const VillagesToSpawn = FMath::RandRange(MinVillagesSpawned, MaxVillagesSpawned);
	for (int VillageIdx = 0; VillageIdx < VillagesToSpawn; ++VillageIdx)
	{
		if (FailedAttempts > 10)
		{
			continue;
		}
		
		float const Yaw = FMath::RandRange(0.0f, 360.0f);
		FVector VillageLocation = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, Yaw)).Vector() * VillageSpawnRadius;
		
		// if no other village is too close
		if (IsVillageSpacingOk(VillageLocation, VillageCenters))
		{
			VillageCenters.Add(VillageLocation);
			SpawnVillageAt(VillageLocation);
		}
		else
		{
			++FailedAttempts;
			--VillageIdx;
		}
	}
}

TArray<AActor*> AZombieGameMode::SpawnVillageAt(FVector const& Location) const
{
	TArray<AActor*> Houses{};
	int const HousesToSpawn{FMath::RandRange(MinHousesPerVillage, MaxHousesPerVillage)};
	
	for (int HouseIdx = 0; HouseIdx < HousesToSpawn; ++HouseIdx)
	{
		auto const & ChosenTemplate = HouseTemplates[FMath::RandRange(0, HouseTemplates.Num() - 1)];
		float const Yaw = FMath::RandRange(0.0f, 360.0f);
		FVector const HouseOffset = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, Yaw)).Vector() * InitialHouseOffSetRadius;
		
		FVector const HouseLocation{Location + HouseOffset};
		FActorSpawnParameters SpawnParams{};
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FRotator HouseRotation{};
		HouseRotation.Yaw = FMath::RandRange(0, 3) * 90.0f;
		Houses.Add(GetWorld()->SpawnActor(ChosenTemplate, &HouseLocation, &HouseRotation, SpawnParams));
	}
	
	SpreadHouses(Houses);
	return Houses;
}

void AZombieGameMode::SpreadHouses(TArray<AActor*> const& Houses) const
{
    if (Houses.Num() < 2) return;

    // ---------- 1. capture initial bounds ----------
    TMap<AActor*, FHouseBounds> BoundsMap;
    for (AActor* House : Houses)
    {
        FVector Origin, Extent;
        House->GetActorBounds(true, Origin, Extent);
        BoundsMap.Add(House, {Origin, Extent});
    }

    // ---------- 2. separate iteratively (horizontal only) ----------
    const int32 MaxIterations = 100;

    for (int32 Iter = 0; Iter < MaxIterations; ++Iter)
    {
        TMap<AActor*, FVector> Displacements; // XY only; Z remains 0
        bool bAnyOverlap = false;

        for (int32 i = 0; i < Houses.Num(); ++i)
        {
            AActor* A = Houses[i];
            const FHouseBounds& BoundsA = BoundsMap[A];

            for (int32 j = i + 1; j < Houses.Num(); ++j)
            {
                AActor* B = Houses[j];
                const FHouseBounds& BoundsB = BoundsMap[B];

                // Only consider horizontal axes (X and Y)
                FVector Delta = BoundsA.Origin - BoundsB.Origin;
                FVector ExtentsSum = BoundsA.Extent + BoundsB.Extent;
                ExtentsSum.X += HouseSeparation;
                ExtentsSum.Y += HouseSeparation;
                // Ignore Z entirely

                const float AbsDeltaX = FMath::Abs(Delta.X);
                const float AbsDeltaY = FMath::Abs(Delta.Y);

                if (AbsDeltaX < ExtentsSum.X && AbsDeltaY < ExtentsSum.Y)
                {
                    bAnyOverlap = true;

                    const float OverlapX = ExtentsSum.X - AbsDeltaX;
                    const float OverlapY = ExtentsSum.Y - AbsDeltaY;

                    // Push along the axis with the smallest overlap (X or Y)
                    if (OverlapX <= OverlapY)
                    {
                        float Push = OverlapX * 0.5f;
                        if (Delta.X > 0.0f)   // A is to the right of B
                        {
                            Displacements.FindOrAdd(A).X += Push;
                            Displacements.FindOrAdd(B).X -= Push;
                        }
                        else
                        {
                            Displacements.FindOrAdd(A).X -= Push;
                            Displacements.FindOrAdd(B).X += Push;
                        }
                    }
                    else
                    {
                        float Push = OverlapY * 0.5f;
                        if (Delta.Y > 0.0f)   // A is further in positive Y
                        {
                            Displacements.FindOrAdd(A).Y += Push;
                            Displacements.FindOrAdd(B).Y -= Push;
                        }
                        else
                        {
                            Displacements.FindOrAdd(A).Y -= Push;
                            Displacements.FindOrAdd(B).Y += Push;
                        }
                    }
                }
            }
        }

        if (!bAnyOverlap) break;

        // Apply the accumulated horizontal pushes (Z stays exactly the same)
        for (const auto& Pair : Displacements)
        {
            AActor* Actor = Pair.Key;
            FVector NewLocation = Actor->GetActorLocation() + Pair.Value;
            NewLocation.Z = Actor->GetActorLocation().Z;   // safety: enforce unchanged Z
            Actor->SetActorLocation(NewLocation);

            // Keep bounds map in sync (only X and Y moved)
            BoundsMap[Actor].Origin.X += Pair.Value.X;
            BoundsMap[Actor].Origin.Y += Pair.Value.Y;
        }
    }
}

bool AZombieGameMode::IsVillageSpacingOk(FVector const& ProposedLocation, TArray<FVector> const& ExistingVillages) const
{
	for (FVector const & Village : ExistingVillages)
	{
		if (FVector::Distance(ProposedLocation, Village) < MinVillageSpacing)
		{
			return false;
		}
	}
	return true;
}

void AZombieGameMode::SpawnZombieAround(const FVector& SurvivorLocation)
{
	auto const & ZombieType = ZombieTypes[FMath::RandRange(0, ZombieTypes.Num() - 1)];
	float const Yaw = FMath::RandRange(0.0f, 360.0f);
	FVector const Offset = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, Yaw)).Vector() * ZombieSpawnRadius;
	FVector SpawnLocation = SurvivorLocation + Offset;
	
	auto Zombie = Cast<ABaseZombie>(GetWorld()->SpawnActor(ZombieType, &SpawnLocation));
	Zombies.Add(Zombie);
}

float AZombieGameMode::GetCurrentZombieSpawnInterval() const
{
	auto const Alpha = FMath::Clamp(GetGameTimeSinceCreation() / SpawnIntervalLerpDuration, 0.0f, 1.0f);
	return FMath::Lerp(MaxZombieSpawnInterval, MinZombieSpawnInterval, Alpha);
}

void AZombieGameMode::SpawnPurgeAround(const FVector& SurvivorLocation)
{
	float const Yaw = FMath::RandRange(0.0f, 360.0f);
	FVector const Offset = FRotator::MakeFromEuler(FVector(0.0f, 0.0f, Yaw)).Vector() * FMath::RandRange(MinPurgeSpawnDistance, MaxPurgeSpawnDistance);
	FVector SpawnLocation = SurvivorLocation + Offset;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.CustomPreSpawnInitalization = [&](AActor* Actor)
	{
		if (APurgeZone* PurgeZone = Cast<APurgeZone>(Actor))
		{
			PurgeZone->Diameter = FMath::Lerp(MaxPurgeDiameter, MinPurgeDiameter, GetCurrentPurgeAlpha());
			PurgeZone->TimeTillPurge = FMath::Lerp(MaxPurgeDuration, MinPurgeDuration, GetCurrentPurgeAlpha());
		}
	};

	GetWorld()->SpawnActor(PurgeClass, &SpawnLocation, &FRotator::ZeroRotator, SpawnParams);
}

float AZombieGameMode::GetCurrentPurgeAlpha() const
{
	return FMath::Clamp(GetGameTimeSinceCreation() - PurgeGracePeriod / PurgeLerpDuration, 0.0f, 1.0f);
}

bool AZombieGameMode::ShouldPurgeZonesSpawn() const
{
	return GetGameTimeSinceCreation() > PurgeGracePeriod;
}
