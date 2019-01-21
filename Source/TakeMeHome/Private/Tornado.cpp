// Fill out your copyright notice in the Description page of Project Settings.

#include "Tornado.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"


ATornado::ATornado()
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

void ATornado::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(CapsuleComponent)) { return; }
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ATornado::OnOverlap);

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
		SetActorLocation(HitResult.Location + GetActorForwardVector() * Speed * DeltaTime);
	}
	else
	{
		DurationExpired();
	}
}

void ATornado::OnOverlap(UPrimitiveComponent* OverlappingComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Lifts OtherActor into the air
	auto NewPos = OtherActor->GetActorLocation();
	NewPos.Z = GetActorLocation().Z + LiftHeight;
	//OtherActor->SetActorLocation(NewPos);
	OtherComp->SetEnableGravity(false);
	bool bHadPhysics = OtherComp->IsSimulatingPhysics();
	OtherComp->SetSimulatePhysics(false);
	OtherComp->SetPhysicsLinearVelocity(FVector::ZeroVector);

	// Timer to turn off gravity after StunDuration
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;
	TimerDel.BindUFunction(this, FName("PutDown"), OtherComp, bHadPhysics);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, StunDuration, false);

	// Do movement with blueprint timeline
	SpinActor(OtherActor, NewPos.Z, StunDuration);
}

void ATornado::DurationExpired()
{
	CapsuleComponent->SetGenerateOverlapEvents(false);
	auto Particle = FindComponentByClass<UParticleSystemComponent>();
	Particle->SecondsBeforeInactive = 10.0f;
	Particle->CustomTimeDilation = 2.0f;
	Particle->Deactivate();
	
	bCanMove = false;

	FTimerHandle DestructionTimer;
	GetWorldTimerManager().SetTimer(DestructionTimer, this, &ATornado::DestroyActor, StunDuration + 1.0f);
}

void ATornado::DestroyActor()
{
	Destroy();
}

void ATornado::PutDown(UPrimitiveComponent *OtherComponent, bool bHadPhysics)
{
	if (ensure(OtherComponent))
	{
		if (bHadPhysics)
		{
			OtherComponent->SetSimulatePhysics(true);
		}
		OtherComponent->SetEnableGravity(true);
	}
}
