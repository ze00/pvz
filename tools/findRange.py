#!/data/data/com.termux/files/usr/bin/python
#-*- coding: utf-8 -*-
"""
File    : findRange.py
Project :
Author  : ze00
Email   : zerozakiGeek@gmail.com
Date    : 2018-01-19
"""
import sys
import re
try:
    fhandle = open("/proc/{}/maps".format(sys.argv[1]))
    addr = int(sys.argv[2],16)
except IndexError:
    print("provide me 2 argument(pid and address you want to query)")
    exit(-1)
pat = re.compile(r"\A([0-9a-fA-F]{8})-([0-9a-fA-F]{8})")
for i in fhandle:
    res = pat.match(i)
    if res:
        start,end = [x for x in res.groups()]
        if int(start,16) <= addr and int(end,16) >= addr:
            print(i)
