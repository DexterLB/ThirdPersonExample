// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatClient.h"


// Sets default values
AChatClient::AChatClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChatClient::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChatClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChatClient::Connect()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to connect."));
}

void AChatClient::Send(const FString & text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to send: ") + text);
	OnReceiveMessage.Broadcast("Me", text);
}
