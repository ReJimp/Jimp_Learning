#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 18:24
# @Author  : Jimp
# @File    : debugger_test.py

import debugger

# test reg
# debugger = debugger.debugger()
# while True:
#     pid = input("attach pid:\n")
#     if pid.isdigit():
#         debugger.attach(int(pid))
#         list_tid = debugger.enumerate_threads()
#         for tid in list_tid:
#             thread_context = debugger.get_thread_context(tid)
#             print("线程%d".center(74, " ") % tid)
#             print("-----------------------------------寄存器----------------------------------------")
#             print("|  EAX: 0x%08x  |  EBX: 0x%08x  |  ECX: 0x%08x  |  EDX: 0x%08x  |" % (thread_context.Eax, thread_context.Ebx, thread_context.Ecx, thread_context.Edx))
#             print("".ljust(81, "-"))
#             print("|  ESI: 0x%08x  |  EDI: 0x%08x  |  EBP: 0x%08x  |  ESP: 0x%08x  |" % (thread_context.Esi, thread_context.Edi, thread_context.Ebp, thread_context.Esp))
#             print("".ljust(81, "-"))
#
#         debugger.detach()
#     else:
#         print("The pid is not a number :(")


# test bp
debugger = debugger.debugger()
pid = input("attach pid:\n")
debugger.attach(int(pid))
printf_addr = debugger.get_func_addr("msvcrt.dll", "printf")
debugger.bp_set(printf_addr)
debugger.debugger_active = True
debugger.run()
