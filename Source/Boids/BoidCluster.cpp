// (c) Jari Drufhagel.


#include "BoidCluster.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));}
#define MESH_SCALE FVector(0.1f, 0.1f, 0.1f)

// Sets default values
ABoidCluster::ABoidCluster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoidVisual = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BoidVisual"));
	BoidVisual->SetupAttachment(RootComponent);

	// This is work around for UE bug, where some components set in C++ code won't show up in Blueprint editor correctly.
	// For example the BoidVisual doesn't work correctly and only way to set a static mesh in editor is with this hack.
	BoidMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("BoidMesh"));
}

// Called when the game starts or when spawned
void ABoidCluster::BeginPlay()
{
	Super::BeginPlay();

	BoidPositions.Init(GetActorLocation(), BoidCount);
	BoidVelocities.Init(FVector(0.0f, 0.0f, 0.0f), BoidCount);
	BoidAccelerations.Init(FVector(0.0f, 0.0f, 0.0f), BoidCount);

	const FTransform Transform = FTransform(GetActorLocation());
	TArray<FTransform> Transforms;
	Transforms.Init(Transform, BoidCount);

	BoidVisual->AddInstances(Transforms, false);
	BoidVisual->SetStaticMesh(BoidMesh);

	for (uint32 i = 0; i < BoidCount; i++)
	{
		const float Angle = FMath::Rand();
		BoidVelocities[i] = FVector(FMath::Sin(Angle), FMath::Cos(Angle), FMath::Cos(0));
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

		// This is just a "temporary" hack to avoid some boids running away and never returning.
		if (FVector::Dist(BoidPositions[i], GetActorLocation()) > MaxBoidTravelDistance)
		{
			BoidPositions[i] = GetActorLocation();
			BoidVelocities[i] *= DeltaTime;
		}

		FTransform Transform;
		Transform.SetLocation(BoidPositions[i] + GetActorLocation());
		Transform.SetScale3D(MESH_SCALE);
		Transform.SetRotation(FQuat(BoidVelocities[i].Rotation() + FRotator(-45.0f))); // FIXME: Rotation doesn't work.

		BoidVisual->UpdateInstanceTransform(i, Transform, true, true);
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
		if (Dist > 0.0f && Dist <= BoidMaxNeighborDistance)
		{
			Ali += BoidVelocities[i];
			Coh += BoidPositions[i];

			NearbyCount++;
		}

		if (Dist > 0.0f && Dist <= BoidDesiredSeparation)
		{
			auto Diff = BoidPositions[CurrentBoid] - BoidPositions[i];
			Diff.Normalize();
			Sep += (Diff * Dist);
			NearbySepCount++;
		}
	}

	if (NearbySepCount > 0)
	{
		Sep /= (float)NearbySepCount;

		if (Sep.Size() > 0.0f)
		{
			Sep.Normalize();
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

			auto Desired = Coh - BoidPositions[CurrentBoid];
			Desired.Normalize();
			Desired *= MaxSpeed;

			Coh = Desired - BoidVelocities[CurrentBoid];
			Coh = Coh.GetClampedToMaxSize(MaxForce);
		}
	}

	return ((Sep * BoidSeparationWeight) + (Ali * BoidAlignmentWeight) + (Coh * BoidCohesionWeight)).GetClampedToMaxSize(MaxSpeed);
}

