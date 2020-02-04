// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SpaceshipProjectile.generated.h"

class ASpaceShip;
/**
 * 
 */
UCLASS()
class ASTEROIDS_API ASpaceshipProjectile : public AProjectile
{
	GENERATED_BODY()

protected:
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
		void OnSpaceshipProjectileHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

public:
	ASpaceshipProjectile();

	void SetSpaceShip(TWeakObjectPtr<ASpaceShip> spaceShip);
	TWeakObjectPtr<ASpaceShip> GetSpaceShip();

	// Executed when component destroyed.

	UFUNCTION()
		virtual void OnDestroyGameController(AActor* SelfActor);

	/** Returns ProjectileMesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }


protected:
	UPROPERTY()
		TWeakObjectPtr<ASpaceShip> SpaceShip;

	UPROPERTY(Category = Projectile, EditAnywhere)
		float OriginalProjectileSpeed;

	virtual void BeginPlay() override;
	
};