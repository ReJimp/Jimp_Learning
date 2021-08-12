#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/8/12 18:24
# @Author  : Jimp
# @File    : my_test.py

import debugger

debugger = debugger.debugger()
pid = input("please enter the pid of process which you want to attach:\n")
debugger.attach(int(pid))
debugger.detach()