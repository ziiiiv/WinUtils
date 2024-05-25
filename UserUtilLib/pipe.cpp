#include "pipe.h"


PipeServer::PipeServer() :
	pipeName_(L""),
	pipeHandle_(INVALID_HANDLE_VALUE),
	direction_(PipeDirection::kNone),
	buffSize_(0)
{
	if (GetUsingApis_() == false)
	{
		status_ = PipeServerStatus::kInitFailed;
	}
	else
	{
		status_ = PipeServerStatus::kConfiguring;
	}
}

 
PipeServer::PipeServer(
	const std::wstring& pipeName,
	PipeDirection direction,
	DWORD buffSize):
	pipeHandle_(INVALID_HANDLE_VALUE)
{
	if (GetUsingApis_() == false)
	{
		status_ = PipeServerStatus::kInitFailed;
	}
	else
	{
		status_ = PipeServerStatus::kConfiguring;
	}

	if (status_ == PipeServerStatus::kConfiguring)
	{

		SetServerProperties(pipeName, direction, buffSize);

		status_ = PipeServerStatus::kConfigured;
	}

}

PipeServer::~PipeServer()
{
	if (pipeHandle_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pipeHandle_);
	}
}


BOOL PipeServer::SetServerProperties(
	const std::wstring& pipeName,
	PipeDirection direction,
	DWORD buffSize)
{
	if (status_ != PipeServerStatus::kConfiguring)
	{
		return FALSE;
	}
	pipeName_ = pipeName;
	direction_ = direction;
	buffSize_ = buffSize;


	status_ = PipeServerStatus::kConfigured;

	return TRUE;

}

BOOL PipeServer::Build()
{

	UNICODE_STRING pipeName;
	DWORD direction = 0;
	OBJECT_ATTRIBUTES objectAttributes;

	// Check Server Status
	if (status_ != PipeServerStatus::kConfigured)
	{
		return FALSE;
	}


	// Initialize pipe name
	RtlInitUnicodeString(&pipeName, pipeName_.c_str());

	InitializeObjectAttributes(&objectAttributes, &pipeName, OBJ_CASE_INSENSITIVE, NULL, NULL);


	// Set Pipe Driection
	if (direction_ == PipeDirection::kInbound)
	{
		direction = GENERIC_READ;
	}
	else if (direction_ == PipeDirection::kOoutbound)
	{
		direction = GENERIC_WRITE;
	}
	else if (direction_ == PipeDirection::kBidrection)
	{
		direction = GENERIC_READ | GENERIC_WRITE;
	}

	// Create Pipe
	pipeHandle_ = CreateNamedPipeW(
		pipeName_.c_str(),
		direction,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		MAX_PIPE_SERVER_SESSION,
		buffSize_,
		buffSize_,
		0,
		NULL);


	if (pipeHandle_ == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	status_ = PipeServerStatus::kReady;


	return TRUE;
}


DWORD PipeServer::Wait()
{
	BOOL result = FALSE;
	DWORD error = 0;


	if (pipeHandle_ == INVALID_HANDLE_VALUE)
	{
		return ERROR_INVALID_HANDLE;
	}

	if (status_ == PipeServerStatus::kReady)
	{
		status_ = PipeServerStatus::kRunning;
	}
	else if(status_ != PipeServerStatus::kRunning)
	{
		return ERROR_INVALID_PARAMETER;
	}


	result = ConnectNamedPipe(pipeHandle_, NULL);

	if (result == FALSE)
	{
		error = GetLastError();
		if (error != ERROR_PIPE_CONNECTED)
		{
			status_ = PipeServerStatus::kReady;
			return error;
		}
	}


	return ERROR_SUCCESS;
}


DWORD PipeServer::Receive(
	VOID* buff, 
	ULONG buffSize,
	ULONG* receivedSize)
{

	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;


	if (status_ != PipeServerStatus::kRunning)
	{
		return ERROR_ACCESS_DENIED;
	}

	status = NtReadFile_(
		pipeHandle_,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		buff,
		buffSize,
		NULL,
		NULL);

	if (NT_SUCCESS(status))
	{
		return ERROR_OPERATION_ABORTED;
	}

	*receivedSize = static_cast<ULONG>(ioStatusBlock.Information);

	return ERROR_SUCCESS;
}


DWORD PipeServer::Send(
	const VOID* buff,
	ULONG buffSize,
	ULONG* receivedSize)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;


	if (status_ != PipeServerStatus::kRunning)
	{
		return ERROR_ACCESS_DENIED;
	}

	status = NtWriteFile_(
		pipeHandle_,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		const_cast<PVOID>(buff),
		buffSize,
		NULL,
		NULL);

	if (NT_SUCCESS(status))
	{
		return ERROR_OPERATION_ABORTED;
	}

	*receivedSize = static_cast<ULONG>(ioStatusBlock.Information);

	return ERROR_SUCCESS;
}

bool PipeServer::GetUsingApis_()
{
	HMODULE ntdll = GetModuleHandleA("ntdll.dll");

	if (ntdll != NULL)
	{
		NtWriteFile_ = reinterpret_cast<NTWRITEFILE>(GetProcAddress(ntdll, "NtWriteFile"));
		NtReadFile_ = reinterpret_cast<NTREADFILE>(GetProcAddress(ntdll, "NtReadFile"));
	}

	if ( NtWriteFile_ == NULL
		|| NtReadFile_ == NULL)
	{
		return false;
	}
	return true;

}
