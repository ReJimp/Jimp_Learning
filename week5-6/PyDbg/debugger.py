#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 16:47
# @Author  : Jimp
# @File    : debugger.py

from ctypes import *
from debugger_defines import *

kernel32 = windll.kernel32

class debugger():
    def __init__(self):
        self.h_process       = None
        self.pid             = None
        self.debugger_active = False

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

            print("[+] We have successfully lauched the process!")
            print("[+] PID: %d" % process_information.dwProcessId)
        else:
            print("[-] Error: 0x%08x" % kernel32.GetLastError())

        self.h_process = self.open_process(process_information.dwProcessId)

    def open_process(self, pid):
        h_process = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
        return h_process

    def attach(self, pid):
        self.h_process = self.open_process(pid)

        # try to attach the target process
        if kernel32.DebugActiveProcess(pid):
            self.debugger_active = True
            self.pid             = int(pid)
            self.run()
        else:
            print("[-] Unable to attach the target process")

    def run(self):
        while self.debugger_active == True:
            self.get_debug_event()

    def get_debug_event(self):
        debug_event = DEBUG_EVENT()
        continue_status = DBG_CONTINUE

        if kernel32.WaitForDebugEvent(byref(debug_event), INFINITE):
            input("press a key to continue...\n")
            self.debugger_active = False
            kernel32.ContinueDebugEvent(
                debug_event.dwProcessId,
                debug_event.dwThreadId,
                continue_status
            )

    def detach(self):
        if kernel32.DebugActiveProcessStop(self.pid):
            print("[+] Finished debugging...")
            return True
        else:
            print("There was an error")
            return False

