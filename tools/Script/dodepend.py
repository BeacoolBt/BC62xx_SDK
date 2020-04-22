#!python3
#import .d; out .c & .h
import sys
import re
#slist = []
argLen = len(sys.argv)
#print argLen
if(argLen != 1):
	print("input param error")
	sys.exit()

for line in sys.stdin:
    t = re.sub(r'.*:','', line) #Delete "*.o *.d:"
    a = re.sub(r' \\','', t)    #Delete " \" at the end.
    b = re.sub(r'^[ \t]','', a) #Delete space and tab at start line.
    c = re.sub(r' ','\n', b)    #split each file with '\r\n'
    print(c,end=''),
#    slist.append(c)

#print("sorted:")
#slist.sort();
#for i in slist:
#    print(i,end='')