// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatActor.h"
#include "Engine.h"

// Sets default values
AChatActor::AChatActor()
{
	PrimaryActorTick.bCanEverTick = true;

	connection.OnReceivedMessage().AddUObject(this, &AChatActor::DispatchReceiveMessage);
	connection.OnConnected().AddUObject(this, &AChatActor::DispatchConnected);
	connection.OnDisconnected().AddUObject(this, &AChatActor::DispatchDisconnected);
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
	connection.Update();
}

void AChatActor::Connect()
{
	connection.Connect(Server, Username, Nick);
}

void AChatActor::Send(const FString& text, const FString& channel)
{
	connection.Send(text, channel);
}

void AChatActor::Perform(const FString& text) {
	connection.Perform(text);
}

void AChatActor::DispatchReceiveMessage(const FString& from, const FString& channel, const FString& message, EChatMessageType type) {
	OnReceiveMessage.Broadcast(from, channel, message, type);
}

void AChatActor::DispatchConnected() {
	for (const FString& command : ExecuteOnConnect) {
		connection.Perform(command);
	}
	OnConnected.Broadcast();
}

void AChatActor::DispatchDisconnected() {
	OnDisconnected.Broadcast();
}