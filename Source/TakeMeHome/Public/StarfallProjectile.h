// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "TakeMeHomeEnums.h"
#include "StarfallProjectile.generated.h"

class UProjectileMovementComponent;
class UPrimitiveComponent;
class USphereComponent;

/**
 * Launches a projectile in the chosen direction. Deals damage on begin overlap event
 */
UCLASS()
class TAKEMEHOME_API AStarfallProjectile : public AAbilityBase
{
	GENERATED_BODY()
	
public:	
	AStarfallProjectile();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappingComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(VisibleAnywhere, Category = "Starfall projectile")
	UProjectileMovementComponent *ProjectileMovement = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Spell setup")
	USphereComponent *SphereCollision = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spell setup")
	UParticleSystem *ProjectileContactParticle = nullptr;

	float Damage = 0.0f;
	FVector LaunchVelocity;
	TArray<AActor *> ActorsToIgnore;
	EElement ElementType;
	TSubclassOf<UDamageType> DamageType;
};
