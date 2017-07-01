// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChatConnection.h"
#include "ChatActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedMessageEvent, FString, From, FString, Channel, FString, Text);

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

private:
	FChatConnection connection;

	UFUNCTION()
	void DispatchReceiveMessage(const FString& from, const FString& channel, const FString& message);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Connect();
	
	UFUNCTION(BlueprintCallable)
	void Send(const FString& text);

	UPROPERTY(EditAnywhere)
	FString Server;

	UPROPERTY(EditAnywhere)
	FString Username;

	UPROPERTY(EditAnywhere)
	FString Nick;
	
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "Message received"))
	FReceivedMessageEvent OnReceiveMessage;
};
