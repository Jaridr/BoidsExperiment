// (c) Jari Drufhagel.


#include "BoidCluster.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

// Sets default values
ABoidCluster::ABoidCluster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABoidCluster::BeginPlay()
{
	Super::BeginPlay();

	BoidPositions.Init(GetActorLocation(), BoidCount);
	BoidVelocities.Init(FVector(0.0f, 0.0f, 0.0f), BoidCount);
	BoidAccelerations.Init(FVector(0.0f, 0.0f, 0.0f), BoidCount);

	for (uint32 i = 0; i < BoidCount; i++)
	{
		const float Angle = FMath::Rand();
		BoidVelocities[i] = FVector(FMath::Sin(Angle), FMath::Cos(Angle), FMath::Sin(Angle));
	}
}

// Called every frame
void ABoidCluster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (uint32 i = 0; i < BoidCount; i++)
	{
		BoidAccelerations[i] = SimulateBoid(i);
		BoidVelocities[i] += BoidAccelerations[i];
		BoidPositions[i] += BoidVelocities[i];
		BoidAccelerations[i] = FVector::ZeroVector;
		DrawDebugLine(GetWorld(), BoidPositions[i], BoidPositions[i] + BoidVelocities[i] * 15.0f, FColor::Blue, false, 1.0f, 1, 1.0f);
	}
}

FVector ABoidCluster::SimulateBoid(uint32 CurrentBoid)
{
	FVector Sep = FVector::ZeroVector;
	FVector Ali = FVector::ZeroVector;
	FVector Coh = FVector::ZeroVector;

	uint32 NearbyCount = 0;
	uint32 NearbySepCount = 0;
	for (uint32 i = 0; i < BoidCount; i++)
	{
		const auto Dist = FVector::Dist(BoidPositions[CurrentBoid], BoidPositions[i]);
		if (Dist > 0.0f && Dist < BoidMaxNeighborDistance)
		{
			Ali += BoidVelocities[i];
			Coh += BoidPositions[i];
			NearbyCount++;
		}

		if (Dist > 0.0f && Dist < BoidDesiredSeparation)
		{
			auto Diff = BoidPositions[CurrentBoid] - BoidPositions[i];
			Diff.Normalize();
			Sep += (Diff / Dist);
			NearbySepCount++;
		}
	}

	if (NearbySepCount > 0)
	{
		Sep /= (float)NearbySepCount;
		Sep.Normalize();

		if (Sep.Size() > 0.0f)
		{
			Sep *= MaxSpeed;
			Sep -= BoidVelocities[CurrentBoid];
			Sep = Sep.GetClampedToMaxSize(MaxForce);
		}
	}
	
	if (NearbyCount > 0)
	{
		// Alignment
		{
			Ali /= (float)NearbyCount;
			Ali.Normalize();

			Ali *= MaxSpeed;
			Ali -= BoidVelocities[CurrentBoid];
			Ali = Ali.GetClampedToMaxSize(MaxForce);
		}

		// Coherency
		{
			Coh /= (float)NearbyCount;
			Coh.Normalize();

			auto Desired = Coh - BoidVelocities[CurrentBoid];
			Desired.Normalize();
			Desired *= MaxSpeed;

			Coh = Desired - BoidVelocities[CurrentBoid];
			Coh = Coh.GetClampedToMaxSize(MaxForce);
		}
	}

	return (Sep * BoidSeparationWeight) + (Ali * BoidAlignmentWeight) + (Coh * BoidCohesionWeight);
}

/*
FVector ABoidCluster::Separation(uint32 CurrentBoid)
{
	FVector S = FVector::ZeroVector;
	uint32 Count = 0;
	for (uint32 i = 0; i < BoidCount; i++)
	{
		const float D = FVector::Dist(BoidPositions[CurrentBoid], BoidPositions[i]);
		if (D > 0.0f && D < SEPARATION_AMOUNT)
		{
			FVector Diff = BoidPositions[CurrentBoid] - BoidPositions[i];
			Diff.Normalize(NORMALIZE_THRESHOLD);
			S += (Diff / D);
			Count++;
		}
	}

	if (Count > 0)
	{
		S /= (float)Count;
	}

	if (S.Size() > 0.0f)
	{
		S.Normalize(NORMALIZE_THRESHOLD);
		S *= MAX_SPEED;
		S -= BoidVelocities[CurrentBoid];
		S = S.GetClampedToMaxSize(MAX_FORCE);
	}

	return S;
}
*/

/*
FVector ABoidCluster::Alignment(uint32 CurrentBoid)
{
	auto S = FVector::ZeroVector;
	uint32 Count = 0;

	for (uint32 i = 0; i < BoidCount; i++)
	{
		const float D = FVector::Dist(BoidPositions[CurrentBoid], BoidPositions[i]);
		if (D > 0.0f && D < MAX_NEIGHBOR_DIST)
		{
			S += BoidVelocities[i];
			Count++;
		}
	}

	if (Count > 0)
	{
		S /= (float)Count;
		S.Normalize(NORMALIZE_THRESHOLD);
		S *= MAX_SPEED;
		S -= BoidVelocities[CurrentBoid];
		S = S.GetClampedToMaxSize(MAX_FORCE);
	}

	return S;
}
*/

/*
FVector ABoidCluster::Cohesion(uint32 CurrentBoid)
{
	auto S = FVector::ZeroVector;
	uint32 Count = 0;

	for (uint32 i = 0; i < BoidCount; i++)
	{
		const float D = FVector::Dist(BoidPositions[CurrentBoid], BoidPositions[i]);
		if (D > 0.0f && D < MAX_NEIGHBOR_DIST)
		{
			S += BoidPositions[i];
			Count++;
		}
	}

	if (Count > 0)
	{
		S /= (float)Count;
		auto Desired = S - BoidPositions[CurrentBoid];
		Desired.Normalize();
		Desired *= MAX_SPEED;

		S = Desired - BoidVelocities[CurrentBoid];
		S = S.GetClampedToMaxSize(MAX_FORCE);
	}

	return S;
}*/

