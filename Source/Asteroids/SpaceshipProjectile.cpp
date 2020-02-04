// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipProjectile.h"
#include "SpaceShip.h"
#include "AsteroidEnemy.h"
#include "Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

void ASpaceshipProjectile::OnSpaceshipProjectileHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AAsteroidEnemy* Enemy = Cast<AAsteroidEnemy>(OtherActor))
	{
		ASpaceShip* PawnShip = Cast<ASpaceShip>(UGameplayStatics::GetPlayerPawn(this, 0));
		if (!Enemy->IsDestroyed) {
			PawnShip->UpdateCurrentProgress(.2f);
			if (FMath::IsNearlyEqual(PawnShip->GetCurrentProgress(), 1.f))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Finish shooting. You win!");
				PawnShip->Destroy();
			}
		}
		Enemy->IsDestroyed = true;
	}
}

ASpaceshipProjectile::ASpaceshipProjectile()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Projectile/Meshes/SpaceshipProjectile.SpaceshipProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;


	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bAutoActivate = false;
	//ProjectileMovement->InitialSpeed = 5000.f;
	//ProjectileMovement->MaxSpeed = ProjectileMovement->InitialSpeed;

	OriginalProjectileSpeed = 15000.f;

	// Die after 0.4 seconds by default
	InitialLifeSpan = 0.4f;
	// Events.
	OnDestroyed.AddDynamic(this, &ASpaceshipProjectile::OnDestroyGameController);
	OnActorHit.AddDynamic(this, &ASpaceshipProjectile::OnSpaceshipProjectileHit);
}

void ASpaceshipProjectile::SetSpaceShip(TWeakObjectPtr<ASpaceShip> spaceShip)
{
	this->SpaceShip = spaceShip;
}

TWeakObjectPtr<ASpaceShip> ASpaceshipProjectile::GetSpaceShip()
{
	return SpaceShip;
}

void ASpaceshipProjectile::OnDestroyGameController(AActor* SelfActor)
{
}

void ASpaceshipProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ASpaceShip* ship = Cast<ASpaceShip>(GetOwner());

	if (ship != nullptr) {

		// absolute speed, projectile speed = spaceship speed + original projectile speed
		ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * (ship->GetCurrentForwardSpeed() + OriginalProjectileSpeed));
		ProjectileMovement->Activate();
	}
}



