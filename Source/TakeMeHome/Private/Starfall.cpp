// Fill out your copyright notice in the Description page of Project Settings.

#include "Starfall.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Public/TimerManager.h"


AStarfall::AStarfall()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AStarfall::BeginPlay()
{
	Super::BeginPlay();

	auto InitLocation = GetActorLocation();
	auto ForwardVector = GetActorForwardVector();
	InitLocation.Z += SpawnHeight;
	SpawnLocations.SetNum(NumOfProjectiles);

	for (int i = 0; i < NumOfProjectiles; i++)
	{
		SpawnLocations[i] = InitLocation + (SpawnDelta * ForwardVector * i);
	}

	FTimerDelegate TimerDelegate;
	FTimerHandle TimerHandle;
	//TimerDelegate.BindUFunction(this, FName("SpawnProjectile"), i);
	//GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, SpawnInterval, false);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AStarfall::SpawnProjectile, SpawnTimeInterval, true, FirstSpawnDelay);
}

void AStarfall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AStarfall::SpawnProjectile()
{
	auto SpawnedActor = GetWorld()->SpawnActor<AStaticMeshActor>(TestBP, SpawnLocations[Index++], GetActorRotation());
	if (Index == SpawnLocations.Num())
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
		Destroy();
	}
}
