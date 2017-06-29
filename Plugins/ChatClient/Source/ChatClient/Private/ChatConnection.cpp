// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatConnection.h"
#include "Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FChatConnection::FChatConnection()
{
}

FChatConnection::~FChatConnection()
{
}

void FChatConnection::Connect(const FString& server, const FString& username, const FString& nick) {
	auto ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	auto ip = ss->CreateInternetAddr();

	auto err = ss->GetHostByName("xkcd.com:1234", *ip);

	if (err != ESocketErrors::SE_NO_ERROR) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("shit: " + FString::FromInt(err)));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to connect to " + ip->ToString(true) + "."));

}