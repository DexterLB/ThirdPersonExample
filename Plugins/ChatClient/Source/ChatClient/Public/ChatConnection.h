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

private:
	TSharedRef<FInternetAddr> ParseHost(const FString & host);
};
