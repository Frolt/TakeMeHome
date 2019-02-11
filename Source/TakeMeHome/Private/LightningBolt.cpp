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
	SpellOwner = ECharacterType::CT_Umir;

	// Set timer for when spell is ready
	FTimerHandle TimerHandle;
	if (CastTime > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ALightningBolt::SpellReady, CastTime);
	}
}

void ALightningBolt::SpellReady()
{
	if (!ensure(SphereCollision)) { return; }

	// Finds out if Umir was interrupted
	auto Umir = Cast<AUmir>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (Umir)
	{
		if ((GetWorld()->GetTimeSeconds() - Umir->LastTimeTookDamage) > CastTime)
		{
			// Cast spell if Umir was not interrupted
			// Get all actors hit
			TArray<AActor *> ActorsHit;
			SphereCollision->GetOverlappingActors(ActorsHit);

			// TODO damage all enemies hit
			for (auto element : ActorsHit)
			{
			}

			// Spawn particle effect
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LightningEffect, GetActorLocation());
		}
	}

	Destroy();
}
