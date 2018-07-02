/*
 * MassEffectModder
 *
 * Copyright (C) 2017-2018 Pawel Kolodziejski <aquadran at users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#else
#error not supported system!
#endif

#define GIGABYTES (1024 * 1024 * 1024)
#define ALIGN_GIGABYTES (1024 * 1024 * 1023)

int DetectAmountMemoryGB()
{
    int amountGB = 0;
#if defined(_WIN32)
    MEMORYSTATUSEX memoryStatusEx;
    memoryStatusEx.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memoryStatusEx);
    amountGB = static_cast<int>((memoryStatusEx.ullTotalPhys + ALIGN_GIGABYTES) / GIGABYTES);
#elif defined(__APPLE__)
    uint64_t physMem;
    size_t paramLen = sizeof(physMem);
    sysctlbyname("hw.memsize", &physMem, &paramLen, nullptr, 0);
    amountGB = static_cast<int>((physMem + ALIGN_GIGABYTES) / GIGABYTES);
#elif defined(__linux__)
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    amountGB = static_cast<int>((static_cast<uint64_t>(sysInfo.totalram) * sysInfo.mem_unit + ALIGN_GIGABYTES) / GIGABYTES);
#endif

    return amountGB;
}

#define MAX_MSG_SIZE 4000

void ConsolePrint(const char *message)
{
    if (message == nullptr)
        CRASH_MSG("ConsolePrint: NULL param!");

    ::puts(message);
}

void ConsolePrint(const QString &message)
{
    ::puts(message.toStdString().c_str());
}

__attribute__ ((__format__ (printf, 1, 2)))
void ConsolePrintf(const char *format, ...)
{
    va_list arguments;
    char message[MAX_MSG_SIZE];

    if (format == nullptr)
        CRASH_MSG("ConsolePrintf: NULL param!");

    va_start(arguments, format);
    vsnprintf(message, MAX_MSG_SIZE - 1, format, arguments);
    va_end(arguments);

    ::printf("%s", message);
}
