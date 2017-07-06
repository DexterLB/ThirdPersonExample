// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EChatMessageType : uint8
{
	Channel 	UMETA(DisplayName = "Channel"),
	Private 	UMETA(DisplayName = "Private"),
	System		UMETA(DisplayName = "System"),
	Self		UMETA(DisplayName = "Myself")
};

/**
 *  Implements a connection to an IRC server.
 */
class CHATCLIENT_API FChatConnection
{
public:
	FChatConnection();
	~FChatConnection();

	/**
	*  Call this often. It polls the connection and executes
	*  relevant actions.
	*/
	void Update();
	
	/**
	*  Connects to an IRC server.
	*  @param server Hostname to connect to
	*  @param username Server-recogniseable username
	*  @param nick Human-readable nick
	*/
	void Connect(const FString& server, const FString& username, const FString& nick);
	bool IsConnected() const;
	void Disconnect();

	/**
	*  Send a message. If it begins with a slash, it is interpreted as a
	*  command. Otherwise, it is sent to the default channel.
	*/
	void Perform(FString text);

	/**
	*  Send a message. The target can be either a channel or an username.
	*/
	void Send(const FString& text, FString target = TEXT(""));

	/**
	*  Join a channel.
	*  @param setDefault Determines whether the channel will become default after joining.
	*/
	void Join(const FString& channel, const FString& password = FString(), bool setDefault = true);
	
	/**
	*  Part (leave) a channel.
	*/
	void Part(const FString& channel);

	/**
	*  Set the default channel. Messages without target will be sent to it.
	*/
	void SetDefaultChannel(const FString& channel);




	DECLARE_EVENT_FourParams(
		FChatConnection,
		FReceivedMessageEvent, 
		const FString&, const FString&, const FString&, EChatMessageType
	);
	/**
	*  Triggered upon receiving any message.
	*/
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
