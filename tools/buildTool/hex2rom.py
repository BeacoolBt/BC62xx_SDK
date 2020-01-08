#!python

import sys

argLen = len(sys.argv)
if(argLen != 3):
	print("input param error")
	sys.exit()

totalLen = 0
addrhi = 0
baseaddr = int(sys.argv[1], 16)#0 #sys.argv[1]
endaddr = int(sys.argv[2], 16)#400000 #sys.argv[2]
rom = [0]*endaddr
#print (endaddr)
#print (len(rom))
#print endaddr,baseaddr
endian = 1
if(argLen == 4):
	bits = int(sys.argv[3], 16)
bits = 32
#n = 0

for line in sys.stdin:
	#print line
	count = int(line[1:3], 16)
	addr = int(line[3:7], 16)
	type = int(line[7:9], 16)

	if type == 2:
		addrhi = int(line[9:13], 16) << 4;
		#print addrhi
	elif type == 4:
		addrhi = int(line[9:13], 16) << 16;

	if type == 0 and addrhi + addr >= baseaddr and addrhi <= endaddr:
		for num in range(0,count):
			pos = addrhi-baseaddr+addr+num
			rom[pos] = int(line[9+2*num:11+2*num], 16)
			if pos > totalLen: 
				totalLen = pos
			#n=n+1
			#print n

if(bits == 1):
	for x in rom:
		sys.stdout.write(hex(rom[x]));
		sys.stdout.write("\n");
else:
	totalLen = totalLen+1
	if(totalLen%(bits>>3) != 0):
		print("total len error!")
	for x in range(0, totalLen, bits>>3):
		for i in range(0, bits>>3):
			if endian == 1:
				sys.stdout.write('{:02x}'.format(rom[x+(bits>>3)-1-i]))
			else:
				sys.stdout.write('{:02x}'.format(rom[x+i]))
		#print str(" " + str(x) + " ")
		sys.stdout.write("\n")