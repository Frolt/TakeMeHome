// Fill out your copyright notice in the Description page of Project Settings.

#include "LightningBolt.h"
#include "Components/SphereComponent.h"
#include "Public/TimerManager.h"
#include "Engine/World.h"
#include "Umir.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"


ALightningBolt::ALightningBolt()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ALightningBolt::BeginPlay()
{
	Super::BeginPlay();

	// Spell settings
	auto *LightningBolt = Cast<UTakeMeHomeGameInstance>(GetGameInstance())->OffensiveSpells.Find(EOffensiveSpell::OS_Lightning_Bolt);
	Damage = LightningBolt->Damage;
	CastTime = LightningBolt->CastTime;
	StunDuration = LightningBolt->StunDuration;
	SphereCollision->SetSphereRadius(LightningBolt->DecalRadius);

	// Subscribe to casting status
	if (AbilityOwner)
	{
		AbilityOwner->OnCastingStatusChange.AddDynamic(this, &ALightningBolt::CastingStatusChanged);
	}
}

void ALightningBolt::CastingStatusChanged(bool bSucceeded)
{
	if (!ensure(SphereCollision)) return;

	if (bSucceeded)
	{
		// Get all overlapping characters
		TArray<AActor *> ActorsHit;
		SphereCollision->GetOverlappingActors(ActorsHit, TSubclassOf<ABaseCharacter>());

		for (auto element : ActorsHit)
		{
			element->TakeDamage(Damage, FDamageEvent(), AbilityOwner->GetController(), this);
		}

		// Spawn particle effect
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LightningEffect, GetActorLocation());
	}
	Destroy();
}
