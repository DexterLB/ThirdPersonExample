// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FIncomingPayload
{
public:
	FString prefix;
	FString nick;
	FString user;
	FString host;
	FString command;
	TArray<FString> arguments;

	FIncomingPayload() {};
	FIncomingPayload(const FString& line);
	~FIncomingPayload();

	void Parse(const FString& line);
	
	FString PrettyPrint();
private:
	TArray<FString> SplitArguments(FString tosplit);
	FString PrettyFormatArguments(const TArray<FString>& arguments);
};
