#!/usr/bin/perl

my @rom = ();
die "missing start and end address" if(@ARGV < 2);
$addrhi = 0;
$baseaddr = hex($ARGV[0]);
$endaddr = hex($ARGV[1]);
$bits = 32;
$bits = $ARGV[2] if(@ARGV > 2);
$endian = 1;

while (<STDIN>) {
	$line++;
	$count = (hex substr($_, 1, 2));
	$addr = (hex substr($_, 3, 4));
	$type = (hex substr($_, 7, 2));
	if($type == 2 || $type == 4) {
		$addrhi = hex(substr($_, 9, 4)) << ($type == 2 ? 4 : 16);
#		printf "type=%d, addr=%x, line=%d\n", $type, $addrhi, $line;
	}
	if($type == 0 && $addrhi + $addr >= $baseaddr && $addrhi <= $endaddr) {
		for($x=0; $x<$count; $x++) {
			$rom[$addrhi - $baseaddr + $addr + $x] = (hex substr($_, 9+2*$x, 2)) ; 
		}
	}
}
if($bits == 1) {
	for($x=0; $x<@rom; $x++) {
			printf ("%x", $rom[$x]);
		}
		print "\n";
} else {
	for($x=0; $x<@rom; $x+=$bits/8) {
		for($i = 0;$i < $bits/8;$i++) {
			printf ("%02x", $endian ? $rom[$x+$bits/8-1-$i] : $rom[$x+$i]);
		}
		print "\n";
	}
}
