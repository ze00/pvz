#!/data/data/com.termux/files/usr/bin/python
#-*- coding: utf-8 -*-
"""
File    : zombiesList.py
Project : PVZ
Author  : ze00
Email   : zerozakiGeek@gmail.com
Date    : 2018-01-18
"""
import sys
if len(sys.argv[1:]) != 2:
    print("need 2 argument")
    sys.exit(-1)
dat = open(sys.argv[1],"rb").read()
b = dat[0x1504:]
p = dat[:0x564]
open(sys.argv[2],"wb").write(p + b'\x15\x00\x00\x00' * 1000 + b)
