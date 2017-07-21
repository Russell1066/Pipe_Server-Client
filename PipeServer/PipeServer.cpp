// PipeServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PipeServer.h"

static LPCWSTR PipeName = L"\\\\.\\pipe\\ReaderWriterTest";
static const int PipeBufferSize = 4096;

void WriteDataToPipe(HANDLE handle, void const* pBytes, int length)
{
	HANDLE pipeHandle = (HANDLE)handle;

	DWORD byteWrittenCount = 0;
	INT32 totalSizInBytes = length;
	if (!WriteFile(pipeHandle, &totalSizInBytes, sizeof(totalSizInBytes), &byteWrittenCount, NULL) || byteWrittenCount != sizeof(totalSizInBytes))
	{
		throw "Cannot write buffer size to pipe";
	}

	byteWrittenCount = 0;
	if (!WriteFile(pipeHandle, pBytes, totalSizInBytes, &byteWrittenCount, NULL))
	{
		throw "Cannot write blob to pipe";
	}

	FlushFileBuffers(pipeHandle);
}

byte* ReadDataFromPipe(const HANDLE &pipeHandle)
{
	DWORD readByteCount = 0;
	UINT32 totalSizInBytes = 0;
	if (!ReadFile(pipeHandle, &totalSizInBytes, sizeof(totalSizInBytes), &readByteCount, NULL) || readByteCount != sizeof(totalSizInBytes))
	{
		auto errorValue = GetLastError();
		throw "Cannot read buffer size from pipe";
		
		return nullptr;
	}

	byte* bytes = new byte[totalSizInBytes];

	if (!ReadFile(pipeHandle, bytes, totalSizInBytes, &readByteCount, NULL))
	{
		throw "Cannot read data from pipe";
	}

	return bytes;
}

int main()
{
	auto pipeHandle = CreateNamedPipeW(
		PipeName,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		PipeBufferSize,
		PipeBufferSize,
		NMPWAIT_USE_DEFAULT_WAIT,
		nullptr);

	bool shouldContinue = true;

	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		throw "Can't open handle!";
	}

	LPSTR pResponse = "This is a response";

	while (shouldContinue)
	{
		ConnectNamedPipe(pipeHandle, nullptr);

		auto bytes = ReadDataFromPipe(pipeHandle);

		free (bytes);

		WriteDataToPipe(pipeHandle, pResponse, strlen(pResponse) + 1);

		FlushFileBuffers(pipeHandle);
	}

	DisconnectNamedPipe(pipeHandle);
	CloseHandle(pipeHandle);

	return 0;
}

