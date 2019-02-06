// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionBar.h"


UActionBar::UActionBar()
{
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UActionBar::BeginPlay()
{
	Super::BeginPlay();


}


// Called every frame
void UActionBar::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

