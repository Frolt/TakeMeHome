// Fill out your copyright notice in the Description page of Project Settings.

#include "StarfallProjectile.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


AStarfallProjectile::AStarfallProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;
}

void AStarfallProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(SphereCollision)) { return; }

	// Setup onOverlap event
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AStarfallProjectile::OnOverlap);

	// Launch projectile
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Force);
	ProjectileMovement->Activate();
}

void AStarfallProjectile::OnOverlap(UPrimitiveComponent* OverlappingComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OVERLAPPED: %s"), *OtherActor->GetName());

	// Spawn particle effect
	if (ProjectileContactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileContactParticle, GetActorLocation(), FRotator::ZeroRotator, true);
	}

	Destroy();
}
