// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create defultSubobject is a factory function that creates a new objects
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleRadius(15.f);	//Arrow operator because this is a pointer
	Capsule->SetCapsuleHalfHeight(20.f);  

	SetRootComponent(Capsule);	// another way to do RootComponent = Capsule


	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent()); // Attach the mesh to the root component 

	// Setup SpringArm for camera as pointer variable of USpringArmComponent
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	//Creating Camera
	ViewCamera = CreateDefaultSubobject< UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);


	AutoPossessPlayer = EAutoReceiveInput::Player0; // Creating Autoposses for Player0

}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// Setting pointer variable of type LocalPlayerSubsystem
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			// connecting enhanced Input Subsystem to mapping context
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}

	}

}

void ABird::MoveForward(float Value)
{
	// testing by printing the value to the output log
	UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value);
}

void ABird::Move(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	//UE_LOG(LogTemp, Warning, TEXT("Triggered"));
	
	if (Controller && (DirectionValue != 0.f))
	{
		UE_LOG(LogTemp, Warning, TEXT("Triggered"));
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, DirectionValue);
	}

}

// Look takes in two dimensional Vector
void ABird::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController())
	{
		// adding Yaw value of mouse input to FVector2D output variable
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}



}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// NEW Way
	// CastChecked will crush the program if it fails to cast, this - pointer to class we are in
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Binds Move function with input action pointer variable that is set in UE
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		// Same kind of binding for Look action
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}






	// OLD Way
	// this - pointer to class we are in, & - symbol for adress of operator
	//PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABird::MoveForward); // mapping the key



}

