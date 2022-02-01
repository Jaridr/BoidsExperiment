// (c) Jari Drufhagel.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "BoidPrimitiveComponent.generated.h"

/**
 * 
 */
UCLASS()
class BOIDS_API UBoidPrimitiveComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UStaticMeshComponent* BoidMesh;
};
