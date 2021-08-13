#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 16:47
# @Author  : Jimp
# @File    : debugger_defines.py

from ctypes import *

# macro like win32
BYTE        =   c_ubyte
WORD        =   c_ushort
LONG        =   c_long
DWORD       =   c_ulong
ULONG_PTR   =   c_ulong
LPBYTE      =   POINTER(c_ubyte)
LPTSTR      =   POINTER(c_char)
HANDLE      =   c_void_p
PVOID       =   c_void_p
LPVOID      =   c_void_p

# const value for CreateProcess
DEBUG_PROCESS                  =    0x00000001
CREATE_NEW_CONSOLE             =    0x00000010
STARTF_USESHOWWINDOW           =    0x00000001

# const value for OpenProcess
PROCESS_ALL_ACCESS             =    0x001F0FFF

# const value for WaitForDebugEvent
INFINITE                       =    0xFFFFFFFF

# const value for ContinueDebugEvent
DBG_CONTINUE                   =    0x00010002

# const value for OpenThread
THREAD_ALL_ACCESS              =    0x001FFFFF

# const value for CreateToolhelp32Snapshot
TH32CS_SNAPTHREAD              =    0x00000004

# const value for GetThreadContext
CONTEXT_FULL                   =    0x00010007
CONTEXT_DEBUG_REGISTERS        =    0x00010010

# const value for debug event
EXCEPTION_DEBUG_EVENT      =    0x1
CREATE_THREAD_DEBUG_EVENT  =    0x2
CREATE_PROCESS_DEBUG_EVENT =    0x3
EXIT_THREAD_DEBUG_EVENT    =    0x4
EXIT_PROCESS_DEBUG_EVENT   =    0x5
LOAD_DLL_DEBUG_EVENT       =    0x6
UNLOAD_DLL_DEBUG_EVENT     =    0x7
OUTPUT_DEBUG_STRING_EVENT  =    0x8
RIP_EVENT                  =    0x9

# const value for debug exception code
EXCEPTION_ACCESS_VIOLATION     = 0xC0000005
EXCEPTION_BREAKPOINT           = 0x80000003
EXCEPTION_GUARD_PAGE           = 0x80000001
EXCEPTION_SINGLE_STEP          = 0x80000004

# structure for CreateProcessA
class STARTUPINFO(Structure):
    _fields_ = [
        ("cb",              DWORD),
        ("lpReserved",      LPTSTR),
        ("lpDesktop",       LPTSTR),
        ("lpTitle",         LPTSTR),
        ("dwX",             DWORD),
        ("dwY",             DWORD),
        ("dwXSize",         DWORD),
        ("dwYSize",         DWORD),
        ("dwXCountChars",   DWORD),
        ("dwYCountChars",   DWORD),
        ("dwFillAttribute", DWORD),
        ("dwFlags",         DWORD),
        ("wShowWindow",     WORD),
        ("cbReserved2",     WORD),
        ("lpReserved2",     LPBYTE),
        ("hStdInput",       HANDLE),
        ("hStdOutput",      HANDLE),
        ("hStdError",       HANDLE),
    ]

class PROCESS_INFORMATION(Structure):
    _fields_ = [
        ("hProcess",        HANDLE),
        ("hThread",         HANDLE),
        ("dwProcessId",     DWORD),
        ("dwThreadId",      DWORD)
    ]

# structure for WaitForDebugEvent
class EXCEPTION_RECORD(Structure):
    pass

EXCEPTION_RECORD._fields_ = [
        ("ExceptionCode",           DWORD),
        ("ExceptionFlags",          DWORD),
        ("ExceptionRecord",         POINTER(EXCEPTION_RECORD)),
        ("ExceptionAddress",        PVOID),
        ("NumberParameters",        DWORD),
        ("ExceptionInformation",    ULONG_PTR * 15)
    ]

class EXCEPTION_RECORD(Structure):
    _fields_ = [
        ("ExceptionCode",           DWORD),
        ("ExceptionFlags",          DWORD),
        ("ExceptionRecord",         POINTER(EXCEPTION_RECORD)),
        ("ExceptionAddress",        PVOID),
        ("NumberParameters",        DWORD),
        ("ExceptionInformation",    ULONG_PTR * 15)
    ]

class EXCEPTION_DEBUG_INFO(Structure):
    _fields_ = [
        ("ExceptionRecord", EXCEPTION_RECORD),
        ("dwFirstChance",   DWORD)
    ]

class DEBUG_EVENT_UNION(Union):
    _fields_ = [
        ("Exception",           EXCEPTION_DEBUG_INFO),
        # ("CreateThread",        CREATE_THREAD_DEBUG_INFO),
        # ("CreateProcessInfo",   CREATE_PROCESS_DEBUG_INFO),
        # ("ExitThread",          EXIT_THREAD_DEBUG_INFO),
        # ("ExitProcess",         EXIT_PROCESS_DEBUG_INFO),
        # ("LoadDll",             LOAD_DLL_DEBUG_INFO),
        # ("UnloadDll",           UNLOAD_DLL_DEBUG_INFO),
        # ("DebugString",         OUTPUT_DEBUG_STRING_INFO),
        # ("RipInfo",             RIP_INFO)
    ]

class DEBUG_EVENT(Structure):
    _fields_ = [
        ("dwDebugEventCode",    DWORD),
        ("dwProcessId",         DWORD),
        ("dwThreadId",          DWORD),
        ("u",                   DEBUG_EVENT_UNION)
    ]

# structure for Thread32First
class THREADENTRY32(Structure):
    _fields_ = [
        ("dwSize",              DWORD),
        ("cntUsage",            DWORD),
        ("th32ThreadID",        DWORD),
        ("th32OwnerProcessID",  DWORD),
        ("tpBasePri",           LONG),
        ("tpDeltaPri",          LONG),
        ("dwFlags",             DWORD)
    ]

# structure for CONTEXT
class FLOATING_SAVE_AREA(Structure):
    _fields_ = [
        ("ControlWord",     DWORD),
        ("StatusWord",      DWORD),
        ("TagWord",         DWORD),
        ("ErrorOffset",     DWORD),
        ("ErrorSelector",   DWORD),
        ("DataOffset",      DWORD),
        ("DataSelector",    DWORD),
        ("RegisterArea",    BYTE * 80),
        ("Spare0",          DWORD)
    ]

class CONTEXT(Structure):
    _fields_ = [
        ("ContextFlags",        DWORD),
        ("Dr0",                 DWORD),
        ("Dr1",                 DWORD),
        ("Dr2",                 DWORD),
        ("Dr3",                 DWORD),
        ("Dr6",                 DWORD),
        ("Dr7",                 DWORD),
        ("FloatSave",           FLOATING_SAVE_AREA),
        ("SegGs",               DWORD),
        ("SegFs",               DWORD),
        ("SegEs",               DWORD),
        ("SegDs",               DWORD),
        ("Edi",                 DWORD),
        ("Esi",                 DWORD),
        ("Ebx",                 DWORD),
        ("Edx",                 DWORD),
        ("Ecx",                 DWORD),
        ("Eax",                 DWORD),
        ("Ebp",                 DWORD),
        ("Eip",                 DWORD),
        ("SegCs",               DWORD),
        ("EFlags",              DWORD),
        ("Esp",                 DWORD),
        ("SegSs",               DWORD),
        ("ExtendedRegisters",   BYTE * 512)
    ]