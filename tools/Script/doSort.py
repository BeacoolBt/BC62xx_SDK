#!python3
#import .d; out .c & .h
import sys
import re
import os

slist = []
argLen = len(sys.argv)
#print argLen
if(argLen != 1):
	print("input param error")
	sys.exit()

for line in sys.stdin:
    slist.append(line)

#print("sorted:")
#slist.sort();
#for i in slist:
#    print(i,end='')

#current directory
currentDir = os.getcwd()	#获取文件的当前路径
#source directory
sourceDir = os.path.abspath(os.path.join(currentDir, r'..\\..\\..\\'))  #获取源代码的路径

ll = list(set(slist))
ll.sort(key=slist.index);
for i in ll:
    rootpath = os.path.abspath(os.path.join(currentDir, i)) #获取绝对路径
    print(os.path.relpath(rootpath, sourceDir), end='') #将返回从 start 路径到 path 的相对路径的字符串。如果没有提供 start，就使用当前工作目录作为开始路径。