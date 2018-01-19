#!env python
#-*- coding: utf-8 -*-
"""
File    : a.py
Project :
Author  : ze00
Email   : zerozakiGeek@gmail.com
Date    : 2018-01-18
"""
dat = open("game1_13.dat","rb").read()
b = dat[0x1504:]
p = dat[:0x564]
open("f","wb").write(p + b'\x15\x00\x00\x00' * 1000 + b)
