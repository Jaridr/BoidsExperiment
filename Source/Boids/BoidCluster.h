// (c) Jari Drufhagel.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"

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
	FVector SimulateBoid(uint32 CurrentBoid);

private:
	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	uint32 BoidCount;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float MaxSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float MaxForce = 0.03f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float MaxBoidTravelDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidMaxNeighborDistance = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidDesiredSeparation = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidSeparationWeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidAlignmentWeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidCohesionWeight = 1.0;

	UPROPERTY(EditAnywhere, Category = "Boid Visual")
	UStaticMesh* BoidMesh;

private:
	UInstancedStaticMeshComponent* BoidVisual;
	TArray<FVector> BoidPositions;
	TArray<FVector> BoidAccelerations;
	TArray<FVector> BoidVelocities;
};
