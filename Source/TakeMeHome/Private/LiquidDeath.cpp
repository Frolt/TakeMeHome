// Fill out your copyright notice in the Description page of Project Settings.

#include "LiquidDeath.h"
#include "Components/SphereComponent.h"
#include "Public/TimerManager.h"
#include "Engine/World.h"
#include "Umir.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"
#include "Particles/ParticleSystemComponent.h"


ALiquidDeath::ALiquidDeath()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ALiquidDeath::BeginPlay()
{
	Super::BeginPlay();

	// Spell settings
	if (!ensure(SphereCollision)) return;
	SphereCollision->SetSphereRadius(DecalRadius);

	// Subscribe to casting status
	if (AbilityOwner)
	{
		AbilityOwner->OnCastingStatusChange.AddDynamic(this, &ALiquidDeath::CastingStatusChanged);
	}

	// Set actor rotation perpendicular to ground normal
	SetRotationBasedOnGround();
}

void ALiquidDeath::CastingStatusChanged(bool bSucceeded)
{
	if (!ensure(SphereCollision)) return;
	if (!ensure(BurstParticle)) return;

	if (bSucceeded)
	{
		// Get all overlapping characters
		TArray<AActor *> ActorsHit;
		SphereCollision->GetOverlappingActors(ActorsHit, TSubclassOf<ABaseCharacter>());

		for (auto element : ActorsHit)
		{
			if (element == AbilityOwner) continue;
			element->TakeDamage(Damage, FDamageEvent(DamageType), AbilityOwner->GetController(), this);
		}

		// Spawn particle effect
		auto SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BurstParticle, GetActorLocation() + (GetActorUpVector() * WaterBallHeight));
		SpawnedParticle->SetRelativeScale3D(FVector(4.0f));
	}
	Destroy();
}

void ALiquidDeath::SetRotationBasedOnGround()
{
	FVector ActorLoc = GetActorLocation();
	FHitResult HitResult;

	// Trace A
	FVector TraceAStart = ActorLoc + FVector(10.0f, 10.0f, 200.0f);
	FVector TraceAEnd = TraceAStart + FVector(0.0f, 0.0f, -400);
	FVector TraceAHit;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceAStart, TraceAEnd, ECC_Visibility))
	{
		TraceAHit = HitResult.Location;
	}
	else
	{
		TraceAHit = TraceAEnd;
	}

	// Trace B
	FVector TraceBStart = ActorLoc + FVector(-10.0f, 10.0f, 200.0f);
	FVector TraceBEnd = TraceBStart + FVector(0.0f, 0.0f, -400);
	FVector TraceBHit;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceBStart, TraceBEnd, ECC_Visibility))
	{
		TraceBHit = HitResult.Location;
	}
	else
	{
		TraceBHit = TraceBEnd;
	}

	// Trace C
	FVector TraceCStart = ActorLoc + FVector(-10.0f, -10.0f, 200.0f);
	FVector TraceCEnd = TraceCStart + FVector(0.0f, 0.0f, -400);
	FVector TraceCHit;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceCStart, TraceCEnd, ECC_Visibility))
	{
		TraceCHit = HitResult.Location;
	}
	else
	{
		TraceCHit = TraceCEnd;
	}

	// Trace D
	FVector TraceDStart = ActorLoc + FVector(10.0f, -10.0f, 200.0f);
	FVector TraceDEnd = TraceDStart + FVector(0.0f, 0.0f, -400);
	FVector TraceDHit;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceDStart, TraceDEnd, ECC_Visibility))
	{
		TraceDHit = HitResult.Location;
	}
	else
	{
		TraceDHit = TraceDEnd;
	}

	FVector ADir = TraceAHit - ActorLoc;
	FVector BDir = TraceAHit - ActorLoc;
	FVector CDir = TraceAHit - ActorLoc;
	FVector DDir = TraceAHit - ActorLoc;

	FVector ABNormal = FVector::CrossProduct(TraceAHit, TraceBHit);
	FVector BCNormal = FVector::CrossProduct(TraceBHit, TraceCHit);
	FVector CDNormal = FVector::CrossProduct(TraceCHit, TraceDHit);
	FVector DANormal = FVector::CrossProduct(TraceDHit, TraceAHit);

	FVector MeanNormal = (ABNormal + BCNormal + CDNormal + DANormal) / 4.0f;
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromZ(MeanNormal);

	SetActorRotation(NewRotation);
}

