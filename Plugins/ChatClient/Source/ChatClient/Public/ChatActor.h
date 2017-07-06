// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChatConnection.h"
#include "ChatActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FReceivedMessageEvent, FString, From, FString, Channel, FString, Text, EChatMessageType, type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConnectedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnectedEvent);

UCLASS(BlueprintType, Blueprintable)
class CHATCLIENT_API AChatActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChatActor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Connect();
	
	UFUNCTION(BlueprintCallable)
	void Send(const FString& text, const FString& channel = TEXT(""));

	UFUNCTION(BlueprintCallable)
	void Perform(const FString& text);

	UPROPERTY(EditAnywhere)
	FString Server;

	UPROPERTY(EditAnywhere)
	FString Username;

	UPROPERTY(EditAnywhere)
	FString Nick;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Commands to execute on connect"))
	TArray<FString> ExecuteOnConnect;
	
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "Message received"))
	FReceivedMessageEvent OnReceiveMessage;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "Connected"))
	FConnectedEvent OnConnected;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "Disconnected"))
	FDisconnectedEvent OnDisconnected;
private:
	FChatConnection connection;

	UFUNCTION()
	void DispatchReceiveMessage(const FString& from, const FString& channel, const FString& message, EChatMessageType type);

	UFUNCTION()
	void DispatchConnected();

	UFUNCTION()
	void DispatchDisconnected();
};
