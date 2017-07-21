// PipeClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


static LPCWSTR pipeName = L"\\\\.\\pipe\\ReaderWriterTest";
static const int PipeBufferSize = 4096;

byte* ReadDataFromPipe(const HANDLE &pipeHandle)
{
	DWORD readByteCount = 0;
	UINT32 totalSizInBytes = 0;
	if (!ReadFile(pipeHandle, &totalSizInBytes, sizeof(totalSizInBytes), &readByteCount, NULL) || readByteCount != sizeof(totalSizInBytes))
	{
		throw "Cannot read buffer size from pipe";
	}

	byte* bytes = new byte[totalSizInBytes];

	if (!ReadFile(pipeHandle, bytes, totalSizInBytes, &readByteCount, NULL))
	{
		throw "Cannot read data from pipe";
	}

	return bytes;
}

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

int main()
{
	int waitAttemptsLeft = 10;

	HANDLE pipeHandle = INVALID_HANDLE_VALUE;

	while (waitAttemptsLeft-- > 0)
	{
		pipeHandle = CreateFileW(pipeName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (pipeHandle != INVALID_HANDLE_VALUE)
		{
			break;
		}

		Sleep(1000);
	}

	if (pipeHandle == INVALID_HANDLE_VALUE)
	{
		throw "Failed to connect to SystemConfigurator pipe...";
	}

	LPSTR pSend = "this is some send data";
	WriteDataToPipe(pipeHandle, pSend, strlen(pSend) + 1);
	auto bytes = ReadDataFromPipe(pipeHandle);
	free(bytes);

	Sleep(2000);

	CloseHandle(pipeHandle);

	return 0;
}

