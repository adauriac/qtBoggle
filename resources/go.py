#!/usr/bin/env python
import os,sys,subprocess
from sys import exit,argv
from os import popen,system
sys.path.append("/mnt/diskc/1/dauriac/lib")
def w(x):sys.stdout.writelines(x)

import jc2 as jc

for i in range(65,65+26):
    cmd = "ln -s %c.png %c0.png"%(i,i)
    print(cmd)
    jc.cmd(cmd)
    cmd = 'magick convert -rotate "90" %c.png  %c1.png'%(i,i)
    print(cmd)
    jc.cmd(cmd)
    cmd = 'magick convert -rotate "180" %c.png %c2.png'%(i,i)
    jc.cmd(cmd)
    print(cmd)
    cmd = 'magick convert -rotate "270" %c.png %c3.png'%(i,i)
    print(cmd)
    jc.cmd(cmd)
