// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"

//비교
#include "KillEmAllGameMode.h"
#include "SimpleShooterGameModeBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Animation 기본 총 숨기기
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

	//모든 Weapon 스폰
	for (TSubclassOf<AGun> Weapons : WeaponList)
	{
		//액터 생성
		Weapon.Add(GetWorld()->SpawnActor<AGun>(Weapons));

		//Character에 Attach 후 일단 숨기기
		Weapon.Last()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
		Weapon.Last()->SetActorHiddenInGame(true);
	}

	//초기 Weapon 설정
	Weapon[ActiveWeaponIdx]->SetActorHiddenInGame(false);
	Weapon[ActiveWeaponIdx]->SetOwner(this);

	//캐릭터 Health 세팅
	Health = MaxHealth;
}

// Enhanced Input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//InputMappingContext 매핑
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	//InputAction 바인딩
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Shoot);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Reload);

		EnhancedInputComponent->BindAction(SwitchWeaponAction1, ETriggerEvent::Triggered, this, &AShooterCharacter::ChangeWeaponIdx1);
		EnhancedInputComponent->BindAction(SwitchWeaponAction2, ETriggerEvent::Triggered, this, &AShooterCharacter::ChangeWeaponIdx2);
		EnhancedInputComponent->BindAction(SwitchWeaponAction3, ETriggerEvent::Triggered, this, &AShooterCharacter::ChangeWeaponIdx3);
		EnhancedInputComponent->BindAction(SwitchWeaponAction4, ETriggerEvent::Triggered, this, &AShooterCharacter::ChangeWeaponIdx4);
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterCharacter::Move(const FInputActionValue& Value)
{
	if (Controller)
	{
		//입력값
		FVector2D MoveVector = Value.Get<FVector2D>();

		//캐릭터가 보고 있는 방향
		const FRotator PlayerRotation(0, Controller->GetControlRotation().Yaw, 0);

		//이동할 방향
		const FVector ForwardDirection = FRotationMatrix(PlayerRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(PlayerRotation).GetUnitAxis(EAxis::Y);

		//적용
		AddMovementInput(ForwardDirection, MoveVector.Y);
		AddMovementInput(RightDirection, MoveVector.X);
	}
}

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	if (Controller)
	{
		//입력값(보는 방향)
		FVector2D LookVector = Value.Get<FVector2D>();

		//적용
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void AShooterCharacter::Reload()
{
	Weapon[ActiveWeaponIdx]->ReloadAmmo();
}

void AShooterCharacter::Shoot()
{
	Weapon[ActiveWeaponIdx]->PullTrigger();
}

void AShooterCharacter::ChangeWeapon(int32 Index)
{
	//null 체크 필요
	if (!Weapon.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("KEY%d: NO Weapon"), Index);
		return;
	}

	//현재 착용중인 Weapon 비활성화
	Weapon[ActiveWeaponIdx]->SetActorHiddenInGame(true);
	Weapon[ActiveWeaponIdx]->SetOwner(nullptr);

	ActiveWeaponIdx = Index;

	//인덱스 중인 Weapon만 활성화
	Weapon[ActiveWeaponIdx]->SetActorHiddenInGame(false);
	Weapon[ActiveWeaponIdx]->SetOwner(this);
}

void AShooterCharacter::ChangeWeaponIdx1()
{
	ChangeWeapon(0);
}

void AShooterCharacter::ChangeWeaponIdx2()
{
	ChangeWeapon(1);
}

void AShooterCharacter::ChangeWeaponIdx3()
{
	ChangeWeapon(2);
}

void AShooterCharacter::ChangeWeaponIdx4()
{
	ChangeWeapon(3);
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	if (IsDead()) 
	{
		AKillEmAllGameMode* GameMode = GetWorld()->GetAuthGameMode<AKillEmAllGameMode>();
		//ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();

		if (GameMode)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}
