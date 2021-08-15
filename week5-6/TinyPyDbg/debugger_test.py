#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 18:24
# @Author  : Jimp
# @File    : debugger_test.py

from debugger import *
from debugger_defines import *
from ctypes import *

# test context
# debugger = debugger()
# while True:
# 	pid = input("attach pid:\n")
# 	debugger.attach(int(pid))
# 	list_tid = debugger.enumerate_threads()
# 	for tid in list_tid:
# 		thread_context = debugger.get_thread_context(tid)
# 		print("线程%d".center(74, " ") % tid)
# 		print("-----------------------------------寄存器----------------------------------------")
# 		print("|  EAX: 0x%08x  |  EBX: 0x%08x  |  ECX: 0x%08x  |  EDX: 0x%08x  |" % (thread_context.Eax, thread_context.Ebx, thread_context.Ecx, thread_context.Edx))
# 		print("".ljust(81, "-"))
# 		print("|  ESI: 0x%08x  |  EDI: 0x%08x  |  EBP: 0x%08x  |  ESP: 0x%08x  |" % (thread_context.Esi, thread_context.Edi, thread_context.Ebp, thread_context.Esp))
# 		print("".ljust(81, "-"))
#
# 	debugger.detach()


# test sw bp
# debugger = debugger()
# pid = input("attach pid:\n")
# debugger.attach(int(pid))
#
# printf_addr = debugger.get_func_addr("msvcrt.dll", "printf")
# if debugger.bp_set_soft(printf_addr):
# 	print("[+] bp set successfully")
# else:
# 	print("[-] bp set failed")
# 	print("[-] Error: %d" % windll.kernel32.GetLastError())
# debugger.debugger_active = True
# debugger.run()
# debugger.debugger_active = True
# debugger.run()

# test hw bp
# debugger = debugger()
# pid = input("attach pid:\n")
# debugger.attach(int(pid))
# printf_addr = debugger.get_func_addr("msvcrt.dll", "printf")
# if debugger.bp_set_hw(printf_addr, 4, HW_EXECUTE):
# 	print("[+] bp set successfully")
# else:
# 	print("[-] bp set failed")
# 	print("[-] Error: %d" % windll.kernel32.GetLastError())
# debugger.debugger_active = True
# debugger.run()
# debugger.debugger_active = True
# debugger.run()

# test mem bp
debugger = debugger()
pid = input("attach pid:\n")
debugger.attach(int(pid))
printf_addr = debugger.get_func_addr("msvcrt.dll", "printf")
if debugger.bp_set_mem(printf_addr, 0x500):
	print("[+] bp set successfully")
else:
	print("[-] bp set failed")
	print("[-] Error: %d" % windll.kernel32.GetLastError())
debugger.debugger_active = True
debugger.run()