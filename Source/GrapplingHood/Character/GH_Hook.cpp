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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Engine/BasicShapes/Sphere"));
	Mesh->SetStaticMesh(SphereMeshFinder.Object);
	static ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterialFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
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

	InitialLifeSpan = 0.f;
}


AGH_Hook::~AGH_Hook()
{
	InitialLifeSpan = 0.f;
}

void AGH_Hook::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
//		Destroy();
	}
}

void AGH_Hook::StopAllMovement()
{
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

