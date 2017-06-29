// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatConnection.h"
#include "Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

FChatConnection::FChatConnection() {
	Socket = TUniquePtr<FSocket>(
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)
			->CreateSocket(NAME_Stream, TEXT("chat connection socket"), false)
	);
}

FChatConnection::~FChatConnection()
{
}


void FChatConnection::Connect(const FString& server, const FString& username, const FString& nick) {
	// auto ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	auto ip = ParseHost(server);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to connect to " + ip->ToString(true) + "."));

	if (Socket->Connect(*ip)) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connected!"));
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Well, shit."));
	}
}

void FChatConnection::processLine() {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Received line: " + lineBuffer));
}

void FChatConnection::Update() {
	uint32 pendingSize;
	int32 receivedSize;

	if (Socket->HasPendingData(pendingSize)) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Have pending data"));
		if (!Socket->Recv(dataBuffer, sizeof(dataBuffer), receivedSize, ESocketReceiveFlags::None)) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("unable to receive :("));
			return;
		}

		for (int i = 0; i < receivedSize; i++) {
			if (dataBuffer[i] == '\n') {
				processLine();
			} else {
				lineBuffer.AppendChar(TCHAR(dataBuffer[i]));
			}
		}
	}
}

TSharedRef<FInternetAddr> FChatConnection::ParseHost(const FString& host) {
	FString hostname = host;
	int32 port;

	FString portstring;
	if (host.Split(":", &hostname, &portstring, ESearchCase::IgnoreCase, ESearchDir::FromStart)) {
		port = FCString::Atoi(*portstring);
	} else {
		port = 6667;
	}

	auto ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	auto ip = ss->CreateInternetAddr();

	auto err = ss->GetHostByName(TCHAR_TO_ANSI(*hostname), *ip);

	if (err != ESocketErrors::SE_NO_ERROR) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("shit: " + FString::FromInt(err)));
	}

	ip->SetPort(port);

	return ip;
}