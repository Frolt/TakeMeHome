// Fill out your copyright notice in the Description page of Project Settings.

#include "Tornado.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Controller.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"
#include "TimerManager.h"
#include "BaseCharacter.h"
#include "Umir.h"


ATornado::ATornado()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATornado::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(CapsuleCollision)) return;
	if (!ensure(AbilityOwner)) return;

	// Spell settings
	ActorsToIgnore.Add(AbilityOwner);

	// Setup OnOverlap event
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ATornado::OnOverlap);

	// Set duration timer
	FTimerHandle DurationTimer;
	GetWorldTimerManager().SetTimer(DurationTimer, this, &ATornado::DurationExpired, Duration);
}

void ATornado::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanMove)
	{
		MoveAlongGround(DeltaTime);
	}
}

void ATornado::MoveAlongGround(float DeltaTime)
{
	// Line trace to find the ground beneath the actor
	auto StartPos = GetActorLocation();
	StartPos.Z += TraceHeight;
	auto EndPos = StartPos + (1000.0f * FVector(0.0f, 0.0f, -1.0f));
	FHitResult HitResult;
	auto bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility);

	// Set actor location to ground
	if (bFoundGround)
	{
		GroundLocation = HitResult.Location;
		SetActorLocation(GroundLocation + GetActorForwardVector() * Speed * DeltaTime);
	}
	else
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
		DurationExpired();
	}
}

void ATornado::OnOverlap(UPrimitiveComponent* OverlappingComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ABaseCharacter>())
	{
		if (!ActorsToIgnore.Contains(OtherActor))
		{
			ActorsToIgnore.Add(OtherActor);
			OtherActor->TakeDamage(Damage, FDamageEvent(DamageType), AbilityOwner->GetController(), this);
			LiftPawn(Cast<ABaseCharacter>(OtherActor), LiftHeight, StunDuration, SpinRate, OtherActor->GetActorLocation());
		}
	}
}

void ATornado::DurationExpired()
{
	if (!ensure(CapsuleCollision)) return;

	CapsuleCollision->SetGenerateOverlapEvents(false);
	auto Particle = FindComponentByClass<UParticleSystemComponent>();
	Particle->SecondsBeforeInactive = 10.0f;
	Particle->CustomTimeDilation = 2.0f;
	Particle->Deactivate();
	bCanMove = false;

	FTimerHandle DestructionTimer;
	GetWorldTimerManager().SetTimer(DestructionTimer, this, &ATornado::DestroyActor, 2.0f);
}

void ATornado::DestroyActor()
{
	Destroy();
}
