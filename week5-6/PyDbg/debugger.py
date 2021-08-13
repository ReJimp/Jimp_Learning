#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 16:47
# @Author  : Jimp
# @File    : debugger.py

from debugger_defines import *

kernel32 = windll.kernel32
cdll.LoadLibrary("msvcrt.dll")

class debugger():
    def __init__(self):
        self.h_process          =   None
        self.pid                =   None
        self.debugger_active    =   False
        self.h_thread           =   None
        self.context            =   None
        self.exception          =   None
        self.exception_address  =   None
        self.breakpoints        =   {}

    def load(self, path_to_exe):
        creation_flag = DEBUG_PROCESS

        startupinfo = STARTUPINFO()
        process_information = PROCESS_INFORMATION()

        startupinfo.dwFlags = STARTF_USESHOWWINDOW
        startupinfo.wShowWindow = 0x0

        startupinfo.cb = sizeof(startupinfo)

        if kernel32.CreateProcessW(path_to_exe,
                                   None,
                                   None,
                                   None,
                                   None,
                                   creation_flag,
                                   None,
                                   None,
                                   byref(startupinfo),
                                   byref(process_information)):

            print("[+] Successfully lauched the process :)")
            print("[+] PID: %d" % process_information.dwProcessId)
        else:
            print("[-] Error: 0x%08x" % kernel32.GetLastError())

        self.h_process = self.open_process(process_information.dwProcessId)

    def open_process(self, pid):
        h_process = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
        return h_process

    def attach(self, pid):
        self.h_process = self.open_process(pid)

        # try to attach the target process until we get all access of the target process
        if kernel32.DebugActiveProcess(pid):
            self.debugger_active = True
            self.pid             = int(pid)
            self.run()
        else:
            print("[-] Unable to attach the target process :(")

    def run(self):
        while self.debugger_active == True:
            self.get_debug_event()

    # ****************************************************************

    def get_debug_event(self):
        debug_event     = DEBUG_EVENT()
        continue_status = DBG_CONTINUE

        if kernel32.WaitForDebugEvent(byref(debug_event), INFINITE):
            self.h_thread = self.open_thread(debug_event.dwThreadId)
            self.context = self.get_thread_context(self.h_thread)
            print("[+] Event_Code: %d Thread_Id: %d" % (debug_event.dwDebugEventCode, debug_event.dwThreadId))

            if debug_event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT:
                self.exception = debug_event.u.Exception.ExceptionRecord.ExceptionCode
                self.exception_address = debug_event.u.Exception.ExceptionRecord.ExceptionAddress

                if self.exception == EXCEPTION_ACCESS_VIOLATION:
                    print("[+] Access Violation Detected")
                elif self.exception == EXCEPTION_BREAKPOINT:
                    continue_status = self.exception_handler_breakpoint()
                    self.debugger_active = False
                elif self.exception == EXCEPTION_GUARD_PAGE:
                    print("[+] Guard Page Access Detected")
                elif self.exception == EXCEPTION_SINGLE_STEP:
                    print("[+] Single Stepping")

            kernel32.ContinueDebugEvent(
                debug_event.dwProcessId,
                debug_event.dwThreadId,
                continue_status
            )


    def exception_handler_breakpoint(self):
        print("[+] Inside the breakpoint handler")
        if self.exception_address is not None:
            print("[+] Exception_Address: 0x%08x" % self.exception_address)

        return DBG_CONTINUE

    def read_process_memory(self, address, length):
        data        =    ""
        read_buf    =    create_string_buffer(length)
        count       =    c_ulong(0)

        if not kernel32.ReadProcessMemory(self.h_process,
                                          address,
                                          read_buf,
                                          length,
                                          byref(count)):
            return False
        else:
            data += read_buf.raw
            return data

    def write_process_memory(self, address, data):
        length = len(data)
        count  = c_ulong(0)

        c_data = c_char_p(data[count.value : ])

        if not kernel32.WriteProcessMemory(self.h_process,
                                           address,
                                           c_data,
                                           length,
                                           byref(count)):
            return False
        else:
            return True

    def bp_set(self, address):
        print("address:" + hex(address))
        if not self.breakpoints.get(address):
            try:
                # backup original byte
                origin_byte = self.read_process_memory(address, 1)
                self.write_process_memory(address, "\xCC")
                self.breakpoints[address] = origin_byte
            except:
                return False
        return True

    def get_func_addr(self, dll, func):
        handle = kernel32.GetModuleHandleW(dll)
        print("[+] handle: %d" % handle)
        if handle is None:
            print("[-] Error: %d" % kernel32.GetLastError())

        # problem
        address = kernel32.GetProcAddress(handle, func)
        if address is None:
            print("[-] Error: %d" % kernel32.GetLastError())

        kernel32.CloseHandle(handle)
        return address

    # ****************************************************************

    def detach(self):
        if kernel32.DebugActiveProcessStop(self.pid):
            print("[+] Finished debugging :)")
            return True
        else:
            print("[-] There was an error")
            return False

    def open_thread(self, thread_id):
        h_thread = kernel32.OpenThread(THREAD_ALL_ACCESS, None, thread_id)

        if h_thread is not None:
            return h_thread
        else:
            print("[-] Unable to obtain a valid thread handle :(")
            return False

    def enumerate_threads(self):
        thread_entry = THREADENTRY32()
        thread_list = []

        snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, self.pid)

        if snapshot is not None:
            thread_entry.dwSize = sizeof(THREADENTRY32)
            status = kernel32.Thread32First(snapshot, byref(thread_entry))

            while status:
                if thread_entry.th32OwnerProcessID == self.pid:
                    thread_list.append(thread_entry.th32ThreadID)
                status = kernel32.Thread32Next(snapshot, byref(thread_entry))

            kernel32.CloseHandle(snapshot)
            return thread_list

        else:
            return False

    def get_thread_context(self, thread_id = None, h_thread = None):
        context = CONTEXT()
        context.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS
        if h_thread is None:
            h_thread = self.open_thread(thread_id)
        if kernel32.Wow64GetThreadContext(h_thread, byref(context)):
            kernel32.CloseHandle(h_thread)
            return context
        else:
            return False