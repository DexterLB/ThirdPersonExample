// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatConnection.h"
#include "Engine.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IncomingPayload.h"

DEFINE_LOG_CATEGORY(ChatClient);

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
	auto ip = ParseHost(server);

	UE_LOG(ChatClient, Display, TEXT("Want to connect to %s."), *ip->ToString(true));

	if (!Socket->Connect(*ip)) {
		UE_LOG(ChatClient, Error, TEXT("Unable to connect to %s."), *ip->ToString(true));
		return;
	}

	UE_LOG(ChatClient, Display, TEXT("Connected to %s!"), *ip->ToString(true));

	Command("NICK", { nick });
	Command("USER", { username, "0", "*", username });


	MyNick = nick;
}

void FChatConnection::Send(const FString& text, FString target) {
	if (target.Len() == 0) {
		if (DefaultChannel.Len() == 0) {
			UE_LOG(ChatClient, Error, TEXT("Trying to send message without default channel."));
			return;
		}
		target = DefaultChannel;
	}

	Command("PRIVMSG", { target, text });
	ReceivedMessageEvent.Broadcast(MyNick, target, text, EChatMessageType::Self);
}

void FChatConnection::Perform(FString text) {
	if (!text.RemoveFromStart("/")) {
		Send(text);
		return;
	}

	FString command;
	FString argument;

	text.Split(" ", &command, &argument);

	if (command == "join") {
		Join(argument);
	} else {
		UE_LOG(ChatClient, Error, TEXT("Unknown command: %s."), *command)
	}
}

void FChatConnection::Join(const FString& channel, const FString& password, bool setDefault) {
	if (password.Len() > 0) {
		Command("JOIN", { channel, password });
	} else {
		Command("JOIN", { channel });
	}

	if (setDefault) {
		SetDefaultChannel(channel);
	}
}

void FChatConnection::Part(const FString& channel) {
	Command("PART", { channel });
	if (DefaultChannel == channel) {
		SetDefaultChannel("");
	}
}

void FChatConnection::Disconnect() {
	Command("QUIT", {});
	SetDefaultChannel("");
	MyNick = "";
}

void FChatConnection::SetDefaultChannel(const FString & channel)
{
	DefaultChannel = channel;
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

	UE_LOG(ChatClient, Verbose, TEXT("<- %s"), *payload.PrettyPrint());

	if (payload.command == "PING") {
		Command("PONG", payload.arguments);
		return;
	} else if (payload.command == "PRIVMSG" && payload.arguments.Num() > 1) {
		const FString& from = payload.nick;
		const FString& channel = payload.arguments[0];
		const FString& message = payload.arguments[1];

		if (channel.Len() == 0 || channel == MyNick) {
			ReceivedMessageEvent.Broadcast(from, "<private>", message, EChatMessageType::Private);
		} else {
			ReceivedMessageEvent.Broadcast(from, channel, message, EChatMessageType::Channel);
		}
	}
	else {
		if (payload.arguments.Num() > 0) {
			ReceivedMessageEvent.Broadcast(payload.nick, "<server>", payload.arguments.Last(), EChatMessageType::System);
		}
	}
}



void FChatConnection::Update() {
	UpdateConnectedState();

	if (!Connected) {
		return;
	}

	uint32 pendingSize;
	int32 receivedSize;
	

	if (Socket->HasPendingData(pendingSize)) {
		if (!Socket->Recv(dataBuffer, sizeof(dataBuffer), receivedSize, ESocketReceiveFlags::None)) {
			UE_LOG(ChatClient, Error, TEXT("Unable to receive data."));
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

void FChatConnection::UpdateConnectedState() {
	if (SocketConnected()) {
		if (!Connected) {
			Connected = true;
			UE_LOG(ChatClient, Display, TEXT("Connection established."));
			ConnectedEvent.Broadcast();
		}
	} else {
		if (Connected) {
			Connected = false;
			UE_LOG(ChatClient, Display, TEXT("Connection lost."));
			DisconnectedEvent.Broadcast();
		}
	}
}

bool FChatConnection::SocketConnected() {
	return (Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected);
}

bool FChatConnection::IsConnected() const {
	return Connected;
}

void FChatConnection::SendPayload(const FString& message) {
	if (!SocketConnected()) {
		UE_LOG(ChatClient, Error, TEXT("Socket not connected."));
		return;
	}

	UE_LOG(ChatClient, Verbose, TEXT("-> %s"), *message);

	FString data = message + "\r\n";
	uint8* buffer = (uint8*)TCHAR_TO_UTF8(data.GetCharArray().GetData());
	int32 size = data.Len();
	int32 sent = 0;

	if (!Socket->Send(buffer, size, sent) || sent != size) {
		UE_LOG(ChatClient, Error, TEXT("Unable to send data."));
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
		UE_LOG(ChatClient, Error, TEXT("Unable to perform DNS lookup."));
	}

	ip->SetPort(port);

	return ip;
}