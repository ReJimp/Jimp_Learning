#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 16:47
# @Author  : Jimp
# @File    : debugger_defines.py

from ctypes import *

# macro like win32
WORD        = c_ushort
DWORD       = c_ulong
ULONG_PTR   = c_ulong
LPBYTE      = POINTER(c_ubyte)
LPTSTR      = POINTER(c_char)
HANDLE      = c_void_p
PVOID       = c_void_p
LPVOID      = c_void_p

# const value
DEBUG_PROCESS           =   0x00000001
CREATE_NEW_CONSOLE      =   0x00000010
STARTF_USESHOWWINDOW    =   0x00000001
PROCESS_ALL_ACCESS      =   0x001F0FFF
INFINITE                =   0xFFFFFFFF
DBG_CONTINUE            =   0x00010002

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

EXCEPTION_RECORD._fields = [
        ("ExceptionCode", DWORD),
        ("ExceptionFlags", DWORD),
        ("ExceptionRecord", POINTER(EXCEPTION_RECORD)),
        ("ExceptionAddress", PVOID),
        ("NumberParameters", DWORD),
        ("ExceptionInformation", ULONG_PTR * 15)
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
