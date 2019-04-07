// Fill out your copyright notice in the Description page of Project Settings.

#include "GH_Hook.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AGH_Hook::AGH_Hook()
{
	// Use a sphere as a simple collision representation
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereCollider->InitSphereRadius(5.0f);
	SphereCollider->BodyInstance.SetCollisionProfileName("Projectile");
	SphereCollider->OnComponentHit.AddDynamic(this, &AGH_Hook::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	SphereCollider->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	SphereCollider->CanCharacterStepUpOn = ECB_No;

	SphereCollider->SetEnableGravity(false);

	// Set as root component
	RootComponent = SphereCollider;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere"));
	Mesh->SetStaticMesh(SphereMeshFinder.Object);
	ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterialFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	UMaterialInterface* Material = UMaterialInstanceDynamic::Create(SphereMaterialFinder.Object, SphereMaterialFinder.Object);
	Mesh->SetMaterial(0, Material);
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Mesh->SetRelativeLocation(FVector::ZeroVector);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("SphereCollider"));
	ProjectileMovement->UpdatedComponent = SphereCollider;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	HookState = DOCKED;

	InitialLifeSpan = 0.f;
}


AGH_Hook::~AGH_Hook()
{
}

void AGH_Hook::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if (HookState == FIRING && (OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		ProjectileMovement->Deactivate();
		HookState = HOOKED;
	}
}

void AGH_Hook::StopAllMovement()
{
	ProjectileMovement->Deactivate();
}

void AGH_Hook::Fire(FVector direction)
{
	if (HookState == DOCKED)
	{
		ProjectileMovement->Activate();
		ProjectileMovement->SetVelocityInLocalSpace(GetTransform().InverseTransformVector((direction != FVector::ZeroVector ? direction : GetActorForwardVector()) * FireSpeed));
		HookState = FIRING;
	}
}

void AGH_Hook::Retract(FVector destination, float deltaTime)
{
	if (HookState == FIRING || HookState == HOOKED)
	{
		StopAllMovement();
		HookState = RETRACTING;
	}

	if (HookState == RETRACTING)
	{
		FVector DeltaToDestination = destination - GetActorLocation();
		float distanceThisFrame = RetractSpeed * deltaTime;

		if ((distanceThisFrame * distanceThisFrame) > DeltaToDestination.SizeSquared())
		{
			SetActorLocation(destination);
			HookState = DOCKED;
			return;
		}
		else if(deltaTime != 0.f)
		{
			float ratioDistance = distanceThisFrame / DeltaToDestination.Size();
			SetActorLocation(GetActorLocation() + DeltaToDestination * ratioDistance);
		}
	}
}

