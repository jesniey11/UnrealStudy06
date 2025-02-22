// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
	NodeName = "Update Player Location If Seen";
}

void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(!PlayerPawn) return;
	
	if (!OwnerComp.GetAIOwner()) return;
	if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
	{
		//Set PlayerLocation
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
	}
	else
	{
		//Clear PlayerLocation
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}

}
