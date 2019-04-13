// Fill out your copyright notice in the Description page of Project Settings.

#include "GH_Character.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkinnedMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Public/Engine.h"
#include "Containers/UnrealString.h"
#include "Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include <GenericPlatformMath.h>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGH_Character

// Sets default values
AGH_Character::AGH_Character()
{
	SetActorTickEnabled(true);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	Camera->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetOnlyOwnerSee(true);
	BodyMesh->SetupAttachment(Camera);
	BodyMesh->bCastDynamicShadow = false;
	BodyMesh->CastShadow = false;
	BodyMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	BodyMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMesh->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	GunMesh->SetupAttachment(RootComponent);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	RopeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Cylinder")).Object;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

// Called when the game starts or when spawned
void AGH_Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	GunMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Create hook tip
	if (HookClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			HookInstance = World->SpawnActor<AGH_Hook>(HookClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			check(HookInstance != nullptr && "Spawning of Hook class failed");
			HookInstance->StopAllMovement();
			HookInstance->AttachToComponent(GunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Muzzle");
		}
	}

	// create rope mesh
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		// spawn the projectile at the muzzle
		Rope = World->SpawnActor<AStaticMeshActor>();
		Rope->SetMobility(EComponentMobility::Movable);
		Rope->SetActorEnableCollision(false);
		Rope->FindComponentByClass<UStaticMeshComponent>()->SetStaticMesh(RopeMesh);
		Rope->FindComponentByClass<UStaticMeshComponent>()->SetVisibility(false);
		Rope->SetActorLocation(FVector::ZeroVector);

		PhysicsConstraints.SetNum(2);

		// spawn the physics constraints
		PhysicsConstraints[0] = World->SpawnActor<APhysicsConstraintActor>();
		PhysicsConstraints[1] = World->SpawnActor<APhysicsConstraintActor>();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGH_Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGH_Character::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGH_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGH_Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGH_Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGH_Character::LookUpAtRate);
}

void AGH_Character::Tick(float DeltaSeconds)
{
	if ((!RopeLocked && HookInstance->GetState() == AGH_Hook::State::HOOKED) || HookInstance->GetState() == AGH_Hook::State::RETRACTING || HookInstance->GetState() == AGH_Hook::State::FIRING)
		UpdateRope();

	if (!RopeLocked && HookInstance->GetState() == AGH_Hook::State::HOOKED && GetCharacterMovement()->Velocity.Z < -10.f)
		LockRope();

	if (RopeLocked)
	{
		SwingCharacter(DeltaSeconds);
	}

	if (HookInstance->GetState() == AGH_Hook::State::RETRACTING)
	{
		HookInstance->Retract(GunMesh->GetSocketByName("Muzzle")->GetSocketLocation(GunMesh), DeltaSeconds);
		if(HookInstance->GetState() == AGH_Hook::State::DOCKED)
		{
			HookInstance->AttachToComponent(GunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Muzzle");
			Rope->SetActorLocation(FVector::ZeroVector);
			Rope->FindComponentByClass<UStaticMeshComponent>()->SetVisibility(false);
		}
	}
}

void AGH_Character::OnFire()
{
	if (HookInstance->GetState() == AGH_Hook::State::DOCKED)
	{
		HookInstance->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HookInstance->Fire(Camera->GetForwardVector());

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		Rope->FindComponentByClass<UStaticMeshComponent>()->SetVisibility(true);
	}
	else if (HookInstance->GetState() != AGH_Hook::State::RETRACTING)
	{
		UnlockRope();
		HookInstance->Retract(GetMuzzleWorldLocation(), 0.f);
	}
}

void AGH_Character::UpdateRope()
{
	if (Rope != nullptr)
	{
		FVector CharLocation = GetMuzzleWorldLocation();
		FVector HookLocation = HookInstance->GetActorLocation();
		FVector CharToHookVector = HookLocation - CharLocation;

		Rope->SetActorLocation(CharLocation + (CharToHookVector / 2));

		Rope->SetActorRotation(FRotationMatrix::MakeFromZ(CharToHookVector).Rotator());

		Rope->SetActorScale3D(FVector(0.04f, 0.04f, CharToHookVector.Size() / 100.f));
	}
}

void AGH_Character::LockRope()
{
	UpdateRope();
	//Rope->FindComponentByClass<UStaticMeshComponent>()->SetSimulatePhysics(true);

	//APhysicsConstraintActor* PC_Char = PhysicsConstraints[0];
	//APhysicsConstraintActor* PC_Hook = PhysicsConstraints[1];

	//PC_Char->SetActorLocation(GetMuzzleWorldLocation());
	//PC_Char->FindComponentByClass<UPhysicsConstraintComponent>()->ConstraintActor1 = this;
	//PC_Char->FindComponentByClass<UPhysicsConstraintComponent>()->ComponentName1.ComponentName = FName("GunMesh");
	//PC_Char->FindComponentByClass<UPhysicsConstraintComponent>()->ConstraintActor2 = Rope;
	//PC_Char->FindComponentByClass<UPhysicsConstraintComponent>()->ComponentName1.ComponentName = FName("StaticMeshComponent");
	//PC_Char->FindComponentByClass<UPhysicsConstraintComponent>()->SetDisableCollision(true);


	//PC_Hook->SetActorLocation(HookInstance->GetActorLocation());
	//PC_Hook->FindComponentByClass<UPhysicsConstraintComponent>()->ConstraintActor1 = HookInstance;
	//PC_Hook->FindComponentByClass<UPhysicsConstraintComponent>()->ComponentName1.ComponentName = FName("SphereCollider");
	//PC_Hook->FindComponentByClass<UPhysicsConstraintComponent>()->ConstraintActor2 = Rope;
	//PC_Hook->FindComponentByClass<UPhysicsConstraintComponent>()->ComponentName1.ComponentName = FName("StaticMeshComponent");
	//PC_Hook->FindComponentByClass<UPhysicsConstraintComponent>()->SetDisableCollision(true);

	FVector diffVec = HookInstance->GetActorLocation() - GetMuzzleWorldLocation();
	SwingRopeLength = diffVec.Size();
	SwingAngle = FMath::Acos(FVector::DotProduct(diffVec, FVector(0.f, 0.f, -1.f)) / SwingRopeLength);

	RopeLocked = true;
}

#pragma optimize("", off)
void AGH_Character::SwingCharacter(float DeltaSeconds)
{
	FVector location = FVector::ZeroVector;

    const float angleAccel = (-9.81f / SwingRopeLength) * FMath::Sin(SwingAngle);
    SwingAngleVelocity += angleAccel * DeltaSeconds;
    SwingAngle += SwingAngleVelocity * DeltaSeconds;

	FVector location2D = FVector::ZeroVector;
	location2D.X += sin(SwingAngle) * SwingRopeLength;
	location2D.Z += cos(SwingAngle) * SwingRopeLength;

	DrawDebugLine(
		GetWorld(),
		HookInstance->GetActorLocation(),
		HookInstance->GetActorLocation() + location2D,
		FColor(255, 0, 0),
		false, -1, 0,
		12.333
	);

	FRotationMatrix::MakeFromY(FVector::CrossProduct(HookInstance->GetActorLocation() - GetMuzzleWorldLocation(), FVector(0.f, 1.f, 0.f)) ).Rotator().RotateVector(location2D);

	location += HookInstance->GetActorLocation();

	//SetActorLocation(location - GetMuzzleWorldLocation());

	UpdateRope();
}
#pragma optimize("", on)

void AGH_Character::UnlockRope()
{
	for (APhysicsConstraintActor* physicsConstraint : PhysicsConstraints)
	{
		APhysicsConstraintActor* PC_Char = PhysicsConstraints[0];
		APhysicsConstraintActor* PC_Hook = PhysicsConstraints[1];

		PC_Char->SetActorLocation(GetMuzzleWorldLocation());
		PC_Char->FindComponentByClass<UPhysicsConstraintComponent>()->ConstraintActor1 = nullptr;

		PC_Hook->SetActorLocation(HookInstance->GetActorLocation());
		PC_Hook->FindComponentByClass<UPhysicsConstraintComponent>()->ConstraintActor2 = nullptr;

		RopeLocked = false;
	}
}

void AGH_Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGH_Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGH_Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGH_Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}