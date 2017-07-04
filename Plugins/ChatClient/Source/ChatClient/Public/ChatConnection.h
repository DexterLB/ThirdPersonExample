// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CHATCLIENT_API FChatConnection
{
public:
	FChatConnection();
	~FChatConnection();

	void Connect(const FString& server, const FString& username, const FString& nick);

	void Send(const FString& text, FString target = TEXT(""));

	void Join(const FString& channel, const FString& password = FString(), bool setDefault = true);

	void SetDefaultChannel(const FString& channel);

	void Update();

	DECLARE_EVENT_ThreeParams(FChatConnection, FReceivedMessageEvent, const FString&, const FString&, const FString&);
	FReceivedMessageEvent& OnReceivedMessage() { return ReceivedMessageEvent; };

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

	FString MyNick;
	FString DefaultChannel;
};

DECLARE_LOG_CATEGORY_EXTERN(ChatClient, Log, All);
