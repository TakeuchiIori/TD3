#pragma once
#include <Windows.h>
#include <string>

static void Logger(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}