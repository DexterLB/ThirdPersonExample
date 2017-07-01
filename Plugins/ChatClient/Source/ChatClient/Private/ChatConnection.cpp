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

	if (!Socket->Connect(*ip)) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Well, shit."));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connected!"));

	Command("NICK", { "blob" });
	Command("USER", { "blob", "0", "*", "Blob" });
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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, TEXT("<- ") + lineBuffer);


	// split payload

	FString remainder;
	FString prefix;
	FString command;
	FString argumentString;

	if (
		lineBuffer.StartsWith(TEXT(":"))
		&& lineBuffer.Split(
			" ", &prefix, &remainder
		)) {

		prefix.RemoveFromStart(TEXT(":"));
	} else {
		remainder = lineBuffer;
	}

	if (!remainder.Split(" ", &command, &argumentString)) {
		command = remainder;
	}

	// split prefix

	FString nick;
	FString user;
	FString host;

	if (!prefix.Split("@", &remainder, &host)) {
		remainder = prefix;
	}

	if (!remainder.Split("!", &nick, &user)) {
		nick = remainder;
	}

	// split arguments
	auto arguments = SplitArguments(argumentString);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString(TEXT("<- ("))
		+ "nick=" + nick + ", "
		+ "user=" + user + ", "
		+ "host=" + host + ", "
		+ "command=" + command + ", "
		+ "arguments=" + prettyFormatArguments(arguments) + ")"
	);
}

TArray<FString> FChatConnection::SplitArguments(FString tosplit) {
	TArray<FString> arguments;
	if (tosplit.Len() == 0) {
		return arguments;
	}

	if (tosplit.RemoveFromStart(":")) {
		arguments.Add(tosplit);
		return arguments;
	}

	FString left;
	FString right;

	while (tosplit.Split(" ", &left, &right)) {
		arguments.Add(left);

		if (right.RemoveFromStart(":")) {
			arguments.Add(right);
			return arguments;
		}

		tosplit = right;
	}

	return arguments;
}

FString FChatConnection::prettyFormatArguments(const TArray<FString>& arguments) {
	FString s = "[";
	int32 i = 0;
	for (const FString& argument : arguments) {
		s.Append(argument);
		if (i < arguments.Num() - 1) {
			s.Append(", ");
		}
		i++;
	}
	s.Append("]");

	return s;
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