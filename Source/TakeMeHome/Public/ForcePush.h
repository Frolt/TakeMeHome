// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveSpellBase.h"
#include "ForcePush.generated.h"

class UBoxComponent;
class UParticleSystemComponent;

/**
 * Spell that pushes away objects/enemies
 */
UCLASS()
class TAKEMEHOME_API AForcePush : public AOffensiveSpellBase
{
	GENERATED_BODY()

public:
	AForcePush();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void InterruptEvent(bool bWasInterrupted);
	void PushOverlappingActors();

public:
	UPROPERTY(BlueprintReadWrite, Category = "Spell setup")
	UBoxComponent *BoxCollision = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Spell setup")
	UParticleSystemComponent *ForcePushEffect;

private:
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float PushSpeed = 500.0f;
};
