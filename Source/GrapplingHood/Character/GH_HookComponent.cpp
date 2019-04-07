// Fill out your copyright notice in the Description page of Project Settings.

#include "GH_HookComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UGH_HookComponent::UGH_HookComponent()
{
	// Create a CameraComponent	
	//SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	//SphereCollider->SetRelativeLocation(FVector::ZeroVector);
	//SphereCollider->SetupAttachment(this);
}


// Called when the game starts
void UGH_HookComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGH_HookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

