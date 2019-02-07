// Fill out your copyright notice in the Description page of Project Settings.

#include "ForcePush.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"
#include "TimerManager.h"


AForcePush::AForcePush()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AForcePush::BeginPlay()
{
	Super::BeginPlay();

	// Spell settings
	auto *ForcePush = Cast<UTakeMeHomeGameInstance>(GetGameInstance())->OffensiveSpells.Find(EOffensiveSpell::E_Force_Push);
	Damage = ForcePush->Damage;
	CastTime = ForcePush->CastTime;
	StunDuration = ForcePush->StunDuration;
	SpellOwner = ECharacterType::E_Umir;
}

void AForcePush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ensure(BoxCollision)) { return; }

	// Finds the X extent of the collision box
	MaxRange = BoxCollision->GetScaledBoxExtent().X * 2.0f;

	// Scans the collision box for actor and applies force
	PushAllOverlappingActors(MaxRange);
}

void AForcePush::PushAllOverlappingActors(float MaxRange)
{
	if (!ensure(BoxCollision)) { return; }

	// Finds all overlapping primitive components
	TArray<UPrimitiveComponent *> OverlappingActors;
	BoxCollision->GetOverlappingComponents(OverlappingActors);

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
	auto SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ForcePushEffect, GetActorLocation(), SpawnRotation, true);

	Destroy();
}