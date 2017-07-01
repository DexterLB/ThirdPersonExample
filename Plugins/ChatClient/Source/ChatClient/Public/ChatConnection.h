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


	void Update();


private:
	TSharedRef<FInternetAddr> ParseHost(const FString & host);

	TUniquePtr<FSocket> Socket;

	uint8 dataBuffer[256];
	FString lineBuffer;

	void processLine();

	void SendPayload(const FString & message);
	void Command(FString command, const TArray<FString>& arguments);
};
