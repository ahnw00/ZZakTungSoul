// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/GeometryCollectionComponentBase.h"

void UGeometryCollectionComponentBase::NotifyBreak(const FChaosBreakEvent& Event)
{
	Super::NotifyBreak(Event);

	Event.Component->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Event.Component->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		[this]()
		{
			GetOwner()->Destroy();
		},
		1.0f,
		false
	);
}
