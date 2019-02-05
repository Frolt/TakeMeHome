// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "Tornado.generated.h"

class UCapsuleComponent;
class UPrimitiveComponent;

/**
 * Tornado spell
 */
UCLASS()
class TAKEMEHOME_API ATornado : public ASpellBase
{
	GENERATED_BODY()
	
public:
	ATornado();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappingComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void PutDown(UPrimitiveComponent *OtherComponent, bool bHadPhysics);
	UFUNCTION(BlueprintImplementableEvent, Category = "Tornado")
	void SpinActor(AActor *ActorToSpin, float LiftHeight, float LiftDuration);
	void MoveAlongGround(float DeltaTime);
	void DurationExpired();
	void DestroyActor();

public:
	UPROPERTY(BlueprintReadWrite, Category = "Spell setup")
	UCapsuleComponent *CapsuleCollision = nullptr;

private:
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float Duration = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float TraceHeight = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float Speed = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float LiftHeight = 500.0f;
	bool bCanMove = true;
};
