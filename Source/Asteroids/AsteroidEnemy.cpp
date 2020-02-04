// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidEnemy.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SpaceShip.h"
#include "SpaceshipProjectile.h"

// Sets default values
AAsteroidEnemy::AAsteroidEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructile Component"));
	DestructibleComponent->SetupAttachment(RootComponent);
	DestructibleComponent->SetNotifyRigidBodyCollision(true);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = ProjectileMovement->InitialSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bAutoActivate = true;


	IsDestroyed = false;

}

void AAsteroidEnemy::Damage(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, __FUNCTION__);
}


// Called when the game starts or when spawned
void AAsteroidEnemy::BeginPlay()
{
	Super::BeginPlay();

	DestructibleComponent->OnComponentHit.AddDynamic(this, &AAsteroidEnemy::Damage);
	// OnActorHit.AddDynamic(this, &AAsteroidEnemy::OnAsteroidEnemyHit);
}

void AAsteroidEnemy::OnAsteroidEnemyHit(AActor* Self, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	
	Self->Destroy();

}

//// Called every frame
void AAsteroidEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

