// Fill out your copyright notice in the Description page of Project Settings.

#include "ForcePush.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"


AForcePush::AForcePush()
{
	// Enable this actor to tick
	PrimaryActorTick.bCanEverTick = true;

	// Spell settings
	ManaCost = 10.0f;
	Damage = 10.0f;
	CastTime = 0.0f;
	Cooldown = 0.0f;
	StunDuration = 3.0f;
	ElementType = EElement::E_None;
	Owner = ESpellOwner::E_Umir;
}

void AForcePush::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(BoxComponent)) { return; }

}

void AForcePush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Finds the X extent of the collision box
	MaxRange = BoxComponent->GetScaledBoxExtent().X * 2.0f;

	// Scans the collision box for actor and applies force
	PushAllOverlappingActors(MaxRange);
}

void AForcePush::PushAllOverlappingActors(float MaxRange)
{
	// Finds all overlapping primitive components
	TArray<UPrimitiveComponent *> OverlappingActors;
	BoxComponent->GetOverlappingComponents(OverlappingActors);

	// Calculates and applies force to all actors
	for (auto element : OverlappingActors)
	{
		auto SpellForward = GetActorForwardVector();
		auto OtherDir = element->GetComponentLocation() - GetActorLocation();

		// Project vector to object along player forward vector
		auto ProjDir = FVector::VectorPlaneProject(OtherDir, GetActorRightVector());
		auto Length = ProjDir.Size();
		if (Length < MaxRange)
		{
			// Force strength from -1 to 1
			auto NormalizedForceRate = ((Length / MaxRange) - 1.0f) * -1.0f;
			auto ForceVector = SpellForward * Force * NormalizedForceRate;

			// TODO push pawns aswell as physics objects
			if (element->IsSimulatingPhysics())
			{
				element->AddForce(ForceVector);
			}
		}
	}
	// Spawn particle effect
	auto SpawnRotation = GetActorRotation();
	SpawnRotation.Pitch -= 90.0f;
	auto SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemPtr, GetActorLocation(), SpawnRotation, true);

	Destroy();
}