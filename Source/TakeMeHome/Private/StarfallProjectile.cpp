// Fill out your copyright notice in the Description page of Project Settings.

#include "StarfallProjectile.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.h"


AStarfallProjectile::AStarfallProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;
}

void AStarfallProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Setup
	if (!ensure(SphereCollision)) return;
	ActorsToIgnore.Add(AbilityOwner);

	// Setup onOverlap event
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AStarfallProjectile::OnOverlap);

	// Launch projectile
	ProjectileMovement->SetVelocityInLocalSpace(LaunchVelocity);
	ProjectileMovement->Activate();
}

void AStarfallProjectile::OnOverlap(UPrimitiveComponent* OverlappingComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ensure(ProjectileContactParticle)) return;

	// Spawn particle effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileContactParticle, GetActorLocation(), FRotator::ZeroRotator, true);

	// Apply damage to enemies hit
	if (OtherActor->IsA<ABaseCharacter>())
	{
		if (!ActorsToIgnore.Contains(OtherActor))
		{
			ActorsToIgnore.Add(OtherActor);
			OtherActor->TakeDamage(Damage, FDamageEvent(DamageType), AbilityOwner->GetController(), this);
		}
		return;
	}
	
	Destroy();
}
