#!/data/data/com.termux/files/usr/bin/python
#-*- coding: utf-8 -*-
"""
File    : a.py
Project :
Author  : ze00
Email   : zerozakiGeek@gmail.com
Date    : 2018-03-10
"""
import os
import sys
for i in open('/sdcard/Notes/com.popcap.pvz_na.txt'):
    res = i.split('|')
    if len(res) >= 2:
        addr = res[1]
        os.system("findRange.py {} {}".format(sys.argv[1], addr))
