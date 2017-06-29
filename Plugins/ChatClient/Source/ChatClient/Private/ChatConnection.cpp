// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatConnection.h"
#include "Engine.h"

FChatConnection::FChatConnection()
{
}

FChatConnection::~FChatConnection()
{
}

void FChatConnection::Connect(const FString& server) {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Want to connect to " + server + "."));
}