// Fill out your copyright notice in the Description page of Project Settings.

#include "Tornado.h"
#include "Engine/World.h"

ATornado::ATornado()
{
	// Spell settings
	ManaCost = 10.0f;
	Damage = 10.0f;
	CastTime = 0.0f;
	Cooldown = 0.0f;
	TimeBeforeDestruction = 3.0f;
	ElementType = EElement::E_None;
	Owner = ESpellOwner::E_Umir;
}

void ATornado::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(TimeBeforeDestruction);
}

void ATornado::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveAlongGround(DeltaTime);
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
		SetActorLocation(HitResult.Location + GetActorForwardVector() * Speed * DeltaTime);
	}
	else
	{
		Destroy();
	}
}
