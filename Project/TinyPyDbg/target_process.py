#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
# @Time    : 2021/8/13 20:45
# @Author  : Jimp
# @File    : target_process.py
import time
from ctypes import *

libc = cdll.msvcrt
printf = libc.printf

while True:
	printf("hello\n".encode("ASCII"))
	time.sleep(5)