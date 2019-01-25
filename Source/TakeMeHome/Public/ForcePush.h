// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "ForcePush.generated.h"

class UBoxComponent;
class UParticleSystem;

/**
 * Spell that pushes away objects/enemies
 */
UCLASS()
class TAKEMEHOME_API AForcePush : public ASpellBase
{
	GENERATED_BODY()

public:
	AForcePush();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void PushAllOverlappingActors(float MaxRange);

public:
	UPROPERTY(BlueprintReadWrite, Category = "Spell setup")
	UBoxComponent *BoxCollision = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spell setup")
	UParticleSystem *ForcePushEffect;

private:
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float Force = 100000000.0f;
	float MaxRange = 0.0f;
};
