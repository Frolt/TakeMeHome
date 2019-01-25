// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "LightningBolt.generated.h"

class USphereComponent;
class UParticleSystem;

/**
 * Striking target area with a lightning bolt after channeling is successful
 */
UCLASS()
class TAKEMEHOME_API ALightningBolt : public ASpellBase
{
	GENERATED_BODY()
		
public:
	ALightningBolt();
	virtual void BeginPlay() override;
	
	bool TrySetActorLocation();
	void SpellReady();

public:
	UPROPERTY(BlueprintReadWrite, Category = "Spell settings")
	USphereComponent *SphereCollision = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spell setup")
	UParticleSystem *LightningEffect = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float ChannelTime = 3.0f;

private:
	bool bWasInterrupted = false;
	FVector SpawnLocation;
};