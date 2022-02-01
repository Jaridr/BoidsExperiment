// (c) Jari Drufhagel.


#include "BoidCluster.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}

#define MAX_NEIGHBOR_DIST 500.0f
#define MAX_FORCE 0.03f
#define MAX_SPEED 2.0f
#define SEPARATION_AMOUNT 250.0f
#define NORMALIZE_THRESHOLD 1.1f

// Sets default values
ABoidCluster::ABoidCluster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UBoxComponent* BoidVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoidVolume"));
	BoidVolume->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoidCluster::BeginPlay()
{
	Super::BeginPlay();

	BoidPositions.Init(GetActorLocation(), BoidCount);
	BoidVelocities.Init(FVector(0.0f, 0.0f, 0.0f), BoidCount);
	BoidAccelerations.Init(FVector(0.0f, 0.0f, 0.0f), BoidCount);

	FVector Start = FVector::ZeroVector;
	for (uint32 i = 0; i < BoidCount; i++)
	{
		Start += FVector(0.2f, 0.2f, 0.2f);
		BoidPositions[i] += Start * 10.0f;
		BoidVelocities[i] = FVector(FMath::Sin(Start.X), FMath::Cos(Start.Y), FMath::Sin(Start.Z));
		//DEBUGMESSAGE("Boid: %u, %f %f %f", i, BoidVelocities[i].X, BoidVelocities[i].Y, BoidVelocities[i].Z);
	}

	DEBUGMESSAGE("Actor %f %f %f", GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
}

// Called every frame
void ABoidCluster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (uint32 i = 0; i < BoidCount; i++)
	{
		BoidAccelerations[i] += Separation(i) * BoidSeparation;
		BoidAccelerations[i] += Alignment(i) * BoidAlignment;
		BoidAccelerations[i] += Cohesion(i) * BoidCohesion;
	}

	for (uint32 i = 0; i < BoidCount; i++) {
		BoidVelocities[i] += BoidAccelerations[i];
		//BoidVelocities[i] = BoidVelocities[i].GetClampedToMaxSize(MAX_SPEED);
		BoidPositions[i] += BoidVelocities[i];
		/*
		if (FMath::Abs(BoidPositions[i].X) > (GetActorLocation().X + 50.0f) || FMath::Abs(BoidPositions[i].Y) > (GetActorLocation().Y + 50.0f) || FMath::Abs(BoidPositions[i].Z) > (GetActorLocation().Z + 50.0f))
		{
			BoidPositions[i] = GetActorLocation();
		}
		*/
		if (FMath::Abs(FVector::Dist(GetActorLocation(), BoidPositions[i])) > BoidMaxDistanceFromOrigin)
		{
			BoidPositions[i] = GetActorLocation();
		}
		BoidAccelerations[i] = FVector::ZeroVector;
	}

	for (uint32 i = 0; i < BoidCount; i++)
	{
		DrawDebugLine(GetWorld(), BoidPositions[i], BoidPositions[i] + BoidVelocities[i], FColor::Blue, false, 1.0f, 1, 1.0f);
		if (i == 0)
		{
			//DEBUGMESSAGE("Boid: %u, Pos: { %f %f %f , Vel: { %f, %f, %f }", i, BoidPositions[i].X, BoidPositions[i].Y, BoidPositions[i].Z, BoidVelocities[i].X, BoidVelocities[i].Y, BoidVelocities[i].Z);
		}
	}
}

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
		/*
		S.Normalize(NORMALIZE_THRESHOLD);

		auto NewDirection = S - BoidPositions[CurrentBoid];
		NewDirection.Normalize(NORMALIZE_THRESHOLD);
		NewDirection *= MAX_SPEED;

		S = NewDirection - BoidVelocities[CurrentBoid];
		*/
	}

	return S.GetClampedToMaxSize(MAX_FORCE);
}

