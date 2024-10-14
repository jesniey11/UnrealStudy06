// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"

//비교
#include "KillEmAllGameMode.h"
#include "SimpleShooterGameModeBase.h"

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

	/*
	//Gun 액터 생성
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);

	//Character에 총 Attach
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	*/

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

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);

	//Enhanced Input으로 바꾸면 개선됨...
	PlayerInputComponent->BindAction(TEXT("WeaponChangeNumber1"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ChangeWeaponIdx1);
	PlayerInputComponent->BindAction(TEXT("WeaponChangeNumber2"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ChangeWeaponIdx2);
	PlayerInputComponent->BindAction(TEXT("WeaponChangeNumber3"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ChangeWeaponIdx3);
	PlayerInputComponent->BindAction(TEXT("WeaponChangeNumber4"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ChangeWeaponIdx4);
	
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

void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
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

void AShooterCharacter::ChangeWeapon(int32 Index)
{
	//null 체크 필요
	if (!Weapon.IsValidIndex(Index)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("KEY%d: NO Weapon"), Index); 
		return;
	}

	//현재 착용중인 Weapon 비활성화
	UE_LOG(LogTemp, Warning, TEXT("비활 Active Idx: %d"), ActiveWeaponIdx);
	Weapon[ActiveWeaponIdx]->SetActorHiddenInGame(true);
	Weapon[ActiveWeaponIdx]->SetOwner(nullptr);
	
	ActiveWeaponIdx = Index;

	//인덱스 중인 Weapon만 활성화
	UE_LOG(LogTemp, Warning, TEXT("활성화 Active Idx: %d"), ActiveWeaponIdx);
	Weapon[ActiveWeaponIdx]->SetActorHiddenInGame(false);
	Weapon[ActiveWeaponIdx]->SetOwner(this);
}

void AShooterCharacter::Shoot()
{
	//Gun->PullTrigger();
	Weapon[ActiveWeaponIdx]->PullTrigger();
}
