// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatConnection.h"
#include "Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IncomingPayload.h"

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

	if (!Socket->Connect(*ip)) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Well, shit."));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connected!"));

	Command("NICK", { nick });
	Command("USER", { username, "0", "*", username });

	MyNick = nick;
}

void FChatConnection::Send(const FString& target, const FString& text) {
	Command("PRIVMSG", { target, text });
	ReceivedMessageEvent.Broadcast(MyNick, target, text);
}

void FChatConnection::Join(const FString& channel, const FString& password) {
	if (password.Len() > 0) {
		Command("JOIN", { channel, password });
	} else {
		Command("JOIN", { channel });
	}
}

void FChatConnection::Command(FString command, const TArray<FString>& arguments) {
	uint32 i = 0;
	for (const auto& argument : arguments) {
		if (i == arguments.Num() - 1) {
			command += " :";
		} else {
			command += " ";
		}
		command += argument;
		i++;
	}

	command.ReplaceInline(TEXT("\r"), TEXT(""));
	command.ReplaceInline(TEXT("\n"), TEXT(""));

	SendPayload(command);
}

void FChatConnection::processLine() {
	auto payload = FIncomingPayload(lineBuffer);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, TEXT("<- ") + payload.PrettyPrint());

	if (payload.command == "PING") {
		Command("PONG", payload.arguments);
		return;
	} else if (payload.command == "PRIVMSG" && payload.arguments.Num() > 1) {
		const auto& from = payload.nick;
		const auto& channel = payload.arguments[0];
		const auto& message = payload.arguments[1];

		ReceivedMessageEvent.Broadcast(from, channel, message);
	}
}



void FChatConnection::Update() {
	uint32 pendingSize;
	int32 receivedSize;

	if (Socket->HasPendingData(pendingSize)) {
		if (!Socket->Recv(dataBuffer, sizeof(dataBuffer), receivedSize, ESocketReceiveFlags::None)) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("unable to receive :("));
			return;
		}

		for (int i = 0; i < receivedSize; i++) {
			if (dataBuffer[i] == '\n' || dataBuffer[i] == '\r') {
				if (lineBuffer.Len() > 0) {
					processLine();
				}
				lineBuffer = TEXT("");
			} else {
				lineBuffer.AppendChar(TCHAR(dataBuffer[i]));
			}
		}
	}
}

void FChatConnection::SendPayload(const FString& message) {

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("-> ") + message);

	FString data = message + "\r\n";
	uint8* buffer = (uint8*)TCHAR_TO_UTF8(data.GetCharArray().GetData());
	int32 size = data.Len();
	int32 sent = 0;

	if (!Socket->Send(buffer, size, sent) || sent != size) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("unable to send :("));
		return;
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