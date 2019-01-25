// Fill out your copyright notice in the Description page of Project Settings.

#include "LightningBolt.h"
#include "Components/SphereComponent.h"
#include "Public/TimerManager.h"
#include "Engine/World.h"
#include "Umir.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"


ALightningBolt::ALightningBolt()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ALightningBolt::BeginPlay()
{
	Super::BeginPlay();


	// Set actor where the player targeted
	if (TrySetActorLocation())
	{
		// Set timer for when spell is ready
		FTimerHandle TimerHandle;
		if (ChannelTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ALightningBolt::SpellReady, ChannelTime);
		}
		else
		{
			SpellReady();
		}
	}
}

bool ALightningBolt::TrySetActorLocation()
{
	// Cehck if umir exist
	auto Umir = Cast<AUmir>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Umir) { return false; }

	// Get mouse world position + direction 
	FVector MouseLocation;
	FVector MouseDirection;
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	// Find end location
	FVector EndLocation = MouseLocation + (MouseDirection.GetSafeNormal() * Umir->TargetRange);

	// Linetrace to ground
	FHitResult HitResult;
	auto bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, EndLocation, ECC_Visibility);
	if (bFoundGround)
	{
		SetActorLocation(HitResult.Location);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not find ground!"));
		return false;
	}
}

void ALightningBolt::SpellReady()
{
	if (!ensure(SphereCollision)) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Spell ready!"));

	// Finds out if Umir was interrupted
	auto Umir = Cast<AUmir>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (Umir)
	{
		if ((GetWorld()->GetTimeSeconds() - Umir->LastTimeTookDamage) > ChannelTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("Umir was NOT interrupted!"));
			bWasInterrupted = false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Umir was interrupted!"));
			bWasInterrupted = true;
		}
	}

	// Cast spell if Umir was not interrupted
	if (!bWasInterrupted)
	{
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

	Destroy();
}
