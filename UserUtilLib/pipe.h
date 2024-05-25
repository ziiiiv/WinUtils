#pragma once

#include "../Common/NtApiDef.h"

#include <Windows.h>
#include <string>

#define MAX_PIPE_SERVER_SESSION 5

enum class PipeDirection
{
	kNone,
	kInbound,	//	Receive Data
	kOoutbound,	//	Send Data
	kBidrection	//	Receive / SendData
};

enum class PipeServerStatus
{
	kUnInitialized,
	kInitFailed,
	kInitialized,
	kConfiguring,
	kConfigured,
	kReady,
	kRunning,
};

class PipeServer 
{
public:
	PipeServer();
	PipeServer(const std::wstring& pipeName, PipeDirection direction, DWORD buffSize);
	~PipeServer();

	BOOL SetServerProperties(const std::wstring& pipeName, PipeDirection direction, DWORD buffSize);
	BOOL Build();
	DWORD Wait();
	DWORD Receive(VOID* buff, ULONG buffSize, ULONG* receivedSize);
	DWORD Send(const VOID* buff, ULONG buffSize, ULONG* receivedSize);

private:
	// Server Properties
	std::wstring pipeName_;
	PipeDirection direction_;
	HANDLE pipeHandle_;
	DWORD buffSize_;
	PipeServerStatus status_;

	// Using Apis
	NTWRITEFILE NtWriteFile_;
	NTREADFILE NtReadFile_;

	bool GetUsingApis_();
};

