// Fill out your copyright notice in the Description page of Project Settings.

#include "IncomingPayload.h"

FIncomingPayload::FIncomingPayload(const FString& line) {
	Parse(line);
}

void FIncomingPayload::Parse(const FString& line) {
	// split payload

	FString remainder;
	FString argumentString;

	if (
		line.StartsWith(TEXT(":"))
		&& line.Split(
			" ", &prefix, &remainder
		)) {

		prefix.RemoveFromStart(TEXT(":"));
	}
	else {
		remainder = line;
	}

	if (!remainder.Split(" ", &command, &argumentString)) {
		command = remainder;
	}

	// split prefix

	if (!prefix.Split("@", &remainder, &host)) {
		remainder = prefix;
	}

	if (!remainder.Split("!", &nick, &user)) {
		nick = remainder;
	}

	// split arguments
	arguments = SplitArguments(argumentString);
}


TArray<FString> FIncomingPayload::SplitArguments(FString tosplit) {
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

FString FIncomingPayload::PrettyPrint() {
	return FString(FString(TEXT("("))
		+ "nick=" + nick + ", "
		+ "user=" + user + ", "
		+ "host=" + host + ", "
		+ "command=" + command + ", "
		+ "arguments=" + PrettyFormatArguments(arguments) + ")"
	);
}

FString FIncomingPayload::PrettyFormatArguments(const TArray<FString>& arguments) {
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

FIncomingPayload::~FIncomingPayload()
{
}
