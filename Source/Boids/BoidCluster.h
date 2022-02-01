// (c) Jari Drufhagel.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	/*
	UPROPERTY(EditAnyhwere, Category = "Boid Settings")
	float BoidSpeed;
	*/
	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidMaxDistanceFromOrigin;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidSeparation;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidAlignment;

	UPROPERTY(EditAnywhere, Category = "Boid Settings")
	float BoidCohesion;

	FVector Separation(uint32 CurrentBoid);
	FVector Alignment(uint32 CurrentBoid);
	FVector Cohesion(uint32 CurrentBoid);

	TArray<FVector> BoidPositions;
	TArray<FVector> BoidAccelerations;
	TArray<FVector> BoidVelocities;
};
