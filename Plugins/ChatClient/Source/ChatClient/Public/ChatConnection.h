// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EChatMessageType : uint8
{
	Channel 	UMETA(DisplayName = "Channel"),
	Private 	UMETA(DisplayName = "Private"),
	System		UMETA(DisplayName = "System"),
	Self		UMETA(DisplayName = "Myself")
};

/**
 * 
 */
class CHATCLIENT_API FChatConnection
{
public:
	FChatConnection();
	~FChatConnection();

	void Update();

	void Connect(const FString& server, const FString& username, const FString& nick);
	bool IsConnected() const;
	void Disconnect();

	void Perform(FString text);

	void Send(const FString& text, FString target = TEXT(""));
	void Join(const FString& channel, const FString& password = FString(), bool setDefault = true);
	void Part(const FString& channel);

	void SetDefaultChannel(const FString& channel);





	DECLARE_EVENT_FourParams(
		FChatConnection,
		FReceivedMessageEvent, 
		const FString&, const FString&, const FString&, EChatMessageType
	);
	FReceivedMessageEvent& OnReceivedMessage() { return ReceivedMessageEvent; };

	DECLARE_EVENT(
		FChatConnection,
		FConnectedEvent
	);
	FConnectedEvent& OnConnected() { return ConnectedEvent; };

	DECLARE_EVENT(
		FChatConnection,
		FDisconnectedEvent
	);
	FDisconnectedEvent& OnDisconnected() { return DisconnectedEvent; };
private:
	TSharedRef<FInternetAddr> ParseHost(const FString & host);

	TUniquePtr<FSocket> Socket;

	uint8 dataBuffer[256];
	FString lineBuffer;

	void processLine();

	TArray<FString> SplitArguments(FString tosplit);

	FString prettyFormatArguments(const TArray<FString>& arguments);

	void SendPayload(const FString & message);
	void Command(FString command, const TArray<FString>& arguments);

	FReceivedMessageEvent ReceivedMessageEvent;
	FConnectedEvent ConnectedEvent;
	FDisconnectedEvent DisconnectedEvent;

	FString MyNick;
	FString DefaultChannel;

	bool Connected = false;
	bool SocketConnected();
	void UpdateConnectedState();
};

DECLARE_LOG_CATEGORY_EXTERN(ChatClient, Log, All);
