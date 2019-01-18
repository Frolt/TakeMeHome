// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Umir.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TAKEMEHOME_API AUmir : public ACharacter
{
	GENERATED_BODY()

public:
	AUmir();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float NormalizedRate);
	void MoveRight(float NormalizedRate);
	void LookRight(float NormalizedRate);
	void LookUp(float NormalizedRate);
	void Zoom(float NormalizedRate);

// 	void LookRightAtRate(float NormalizedRate);
// 	void LookUpAtRate(float NormalizedRate);

	/// Getters
	USpringArmComponent *GetCameraBoom() const;
	UCameraComponent *GetFollowCamera() const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent *CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent *FollowCamera = nullptr;

// 	// Base turn rate, in deg/sec. Other scaling may affect final turn rate
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
// 		float BaseTurnRate = 45.0f;;
// 	// Base look up/down rate, in deg/sec. Other scaling may affect final rate
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
// 		float BaseLookUpRate = 45.0f;;
	
private:
	bool bCanRotateCamera = false;		// not using atm
	bool bCanRotateCharacter = false;	// not using atm
	float MaxZoom = 1000.0f;
	float MinZoom = 300.0f;
	float ZoomStrength = 50.0f;
};
