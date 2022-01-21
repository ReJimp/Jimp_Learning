#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 16:47
# @Author  : Jimp
# @File    : debugger.py
import time

from debugger_defines import *

kernel32 = windll.kernel32

class debugger():
    def __init__(self):
        self.h_process              =   None
        self.pid                    =   None
        self.debugger_active        =   False
        self.h_thread               =   None
        self.context                =   None
        self.exception              =   None
        self.exception_address      =   None
        self.breakpoints            =   {}
        self.first_breakpoint       =   True
        self.hardware_breakpoints   =   {}
        self.guarded_pages          =   []
        self.memory_breakpoints     =   {}

        # get the page size
        system_info = SYSTEM_INFO()
        kernel32.GetSystemInfo(byref(system_info))
        self.page_size = system_info.dwPageSize


    def load(self, path_to_exe):
        startupinfo = STARTUPINFO()
        process_information = PROCESS_INFORMATION()

        creation_flag = DEBUG_PROCESS
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
            print("[-] Lauch process error: 0x%08x" % kernel32.GetLastError())

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

    def get_debug_event(self):
        debug_event     = DEBUG_EVENT()
        continue_status = DBG_CONTINUE

        if kernel32.WaitForDebugEvent(byref(debug_event), INFINITE):
            self.h_thread = self.open_thread(debug_event.dwThreadId)
            self.context = self.get_thread_context(self.h_thread)

            if debug_event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT:
                self.exception = debug_event.u.Exception.ExceptionRecord.ExceptionCode
                self.exception_address = debug_event.u.Exception.ExceptionRecord.ExceptionAddress

                if self.exception == EXCEPTION_ACCESS_VIOLATION:
                    print("[+] Event_Code: %d Thread_Id: %d" % (debug_event.dwDebugEventCode, debug_event.dwThreadId))
                    print("[+] Access Violation Detected :)")
                elif self.exception == EXCEPTION_BREAKPOINT:
                    print("[+] Event_Code: %d Thread_Id: %d" % (debug_event.dwDebugEventCode, debug_event.dwThreadId))
                    continue_status = self.exception_handler_breakpoint()
                elif self.exception == EXCEPTION_GUARD_PAGE:
                    print("[+] Event_Code: %d Thread_Id: %d" % (debug_event.dwDebugEventCode, debug_event.dwThreadId))
                    print("[+] Guard Page Access Detected :)")
                elif self.exception == EXCEPTION_SINGLE_STEP:
                    print("[+] Event_Code: %d Thread_Id: %d" % (debug_event.dwDebugEventCode, debug_event.dwThreadId))
                    continue_status = self.exception_handler_single_step()

            kernel32.ContinueDebugEvent(
                debug_event.dwProcessId,
                debug_event.dwThreadId,
                continue_status
            )

    # software breakpoint
    def exception_handler_breakpoint(self):
        self.debugger_active = False
        if self.first_breakpoint == True:
            print("[+] First hit the breakpoint :)")
        else:
            print("[+] Hit the software breakpoint defined by user :)")
        self.first_breakpoint = False
        return DBG_CONTINUE

    def bp_set_soft(self, address):
        if not self.breakpoints.get(address):
            try:
                # backup original byte
                origin_byte = self.read_process_memory(address, 1)
                self.write_process_memory(address, "\xCC")
                self.breakpoints[address] = origin_byte
            except:
                return False
        return True

    # hareware breakpoint
    def exception_handler_single_step(self):
        self.debugger_active = False
        continue_status = DBG_CONTINUE

        print("[+] Hit the hardware breakpoint defined by user :)")

        if (self.context.Dr6 & 0x00000001) and self.hardware_breakpoints.get(0):
            bp_index = 0
        elif (self.context.Dr6 & 0x00000002) and self.hardware_breakpoints.get(1):
            bp_index = 1
        elif (self.context.Dr6 & 0x00000004) and self.hardware_breakpoints.get(2):
            bp_index = 2
        elif (self.context.Dr6 & 0x00000008) and self.hardware_breakpoints.get(3):
            bp_index = 3
        else:
            continue_status = DBG_EXCEPTION_NOT_HANDLED

        if self.bp_del_hw(bp_index):
            print("[+] Hardware breakpoint remove :)")

        return continue_status

    def bp_set_hw(self, address, length, condition):
        # check the length of bp
        if length not in (1, 2, 4):
            return False
        else:
            length -= 1

        if condition not in (HW_ACCESS, HW_WRITE, HW_EXECUTE):
            return False

        if not self.hardware_breakpoints.get(0):
            available = 0
        elif not self.hardware_breakpoints.get(1):
            available = 1
        elif not self.hardware_breakpoints.get(2):
            available = 2
        elif not self.hardware_breakpoints.get(3):
            available = 3
        else:
            return False

        print("available: %d" % available)

        for tid in self.enumerate_threads():
            context = self.get_thread_context(thread_id=tid)
            context.Dr7 |= (3 << (available * 2))
            context.Dr7 |= (condition << ((available * 4) + 16))
            context.Dr7 |= (length << ((available * 4) + 18))

            if available == 0:
                context.Dr0 = address
            elif available == 1:
                context.Dr1 = address
            elif available == 2:
                context.Dr2 = address
            elif available == 3:
                context.Dr3 = address

            self.hardware_breakpoints.update({available: (hex(address), length + 1, condition)})
            print(self.hardware_breakpoints)

            h_thread = self.open_thread(tid)
            if h_thread is None:
                print("[-] Error: %d" % kernel32.GetLastError())
                return False

            # error: 5
            if not kernel32.SetThreadContext(h_thread, byref(context)):
                print("[-] Error: %d" % kernel32.GetLastError())

        return True

    def bp_del_hw(self, bp_index):
        for tid in self.enumerate_threads():
            context = self.get_thread_context(thread_id=tid)
            context.Dr7 &= ~(3 << (bp_index * 2))
            context.Dr7 &= ~(3 << ((bp_index * 4) + 16))
            context.Dr7 &= ~(3 << ((bp_index * 4) + 18))

            if bp_index == 0:
                context.Dr0 = 0x00000000
            elif bp_index == 1:
                context.Dr1 = 0x00000000
            elif bp_index == 2:
                context.Dr2 = 0x00000000
            elif bp_index == 3:
                context.Dr3 = 0x00000000

            h_thread = self.open_thread(tid)
            if h_thread is None:
                print("[-] Error: %d" % kernel32.GetLastError())
                return False

            if not kernel32.SetThreadContext(h_thread, byref(context)):
                print("[-] Error: %d" % kernel32.GetLastError())

            del self.hardware_breakpoints[bp_index]

        return True

    # memory breakpoint
    def bp_set_mem(self, address, size):
        mem_basic_info = MEMORY_BASIC_INFORMATION()

        if not kernel32.VirtualQueryEx(self.h_process,
                                   address,
                                   byref(mem_basic_info),
                                   sizeof(mem_basic_info)):
            print("[-] Error: %d" % kernel32.GetLastError())
            return False

        current_page = mem_basic_info.BaseAddress
        while current_page <= address + size:
            self.guarded_pages.append(hex(current_page))
            old_protection = c_ulong(0)

            if not kernel32.VirtualProtectEx(self.h_process,
                                             current_page,
                                             size,
                                             mem_basic_info.Protect | PAGE_GUARD,
                                             byref(old_protection)):
                return False

            current_page += self.page_size
            self.memory_breakpoints[hex(address)] = (hex(address), size, mem_basic_info)

        print("guarded_pages: ")
        print(self.guarded_pages)
        print("memory_breakpoints")
        print(self.memory_breakpoints)
        return True

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

    def get_func_addr(self, dll, func):
        handle = kernel32.GetModuleHandleA(dll)
        if handle is None:
            print("[-] Get the handle error: %d" % kernel32.GetLastError())

        address = kernel32.GetProcAddress(handle, func)
        if address is None:
            print("[-] Get the function address error: %d" % kernel32.GetLastError())

        print("[+] get the target function address: 0x%08x" % address)

        kernel32.CloseHandle(handle)
        return address

    def detach(self):
        if kernel32.DebugActiveProcessStop(self.pid):
            print("[+] Finished debugging :)")
            return True
        else:
            print("[-] There was an error :(")
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
        if kernel32.GetThreadContext(h_thread, byref(context)):
            kernel32.CloseHandle(h_thread)
            return context
        else:
            return False