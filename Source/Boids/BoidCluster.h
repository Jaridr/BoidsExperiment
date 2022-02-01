// (c) Jari Drufhagel.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidPrimitiveComponent.h"

#include "BoidCluster.generated.h"

UCLASS()
class BOIDS_API ABoidCluster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidCluster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	uint32 BoidCount;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float MaxSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float MaxForce = 0.03f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidMaxNeighborDistance = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidDesiredSeparation = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidSeparationWeight;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidAlignmentWeight;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidCohesionWeight;

	FVector SimulateBoid(uint32 CurrentBoid);

	TArray<FVector> BoidPositions;
	TArray<FVector> BoidAccelerations;
	TArray<FVector> BoidVelocities;
};
