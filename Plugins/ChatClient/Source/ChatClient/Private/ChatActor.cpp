// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatActor.h"


// Sets default values
AChatActor::AChatActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChatActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChatActor::Connect()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to connect."));
}

void AChatActor::Send(const FString & text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to send from plugin: ") + text);
	OnReceiveMessage.Broadcast("Me", text);
}
