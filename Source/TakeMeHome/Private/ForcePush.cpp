// Fill out your copyright notice in the Description page of Project Settings.

#include "ForcePush.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"
#include "TimerManager.h"
#include "BaseCharacter.h"
#include "Umir.h"


AForcePush::AForcePush()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AForcePush::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe to casting status
	AbilityOwner->OnCastingStatusChange.AddDynamic(this, &AForcePush::InterruptEvent);
}

void AForcePush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Push actors inside the collision box
	PushOverlappingActors();
}

void AForcePush::PushOverlappingActors()
{
	if (!ensure(BoxCollision)) return;

	// Finds all overlapping primitive components
	TArray<AActor *> OverlappingActors;
	BoxCollision->GetOverlappingActors(OverlappingActors, TSubclassOf<AActor>());

	// Calculates and applies force to all actors
	for (auto element : OverlappingActors)
	{
		if (element == AbilityOwner) continue;

		if (element->IsRootComponentMovable())
		{
			auto SpellForward = GetActorForwardVector();
			element->AddActorWorldOffset(SpellForward * PushSpeed * GetWorld()->GetDeltaSeconds());
			if (element->IsA<ABaseCharacter>())
			{
				Cast<ABaseCharacter>(element)->Stun(StunDuration);
			}
		}
	}
}

void AForcePush::InterruptEvent(bool bWasInterrupted)
{
	Destroy();
}
