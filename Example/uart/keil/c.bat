
@python ..\..\..\tools\buildTool\hex2rom.py 0 40000 < output\keil.hex > output\out.rom || exit /b
@copy ..\..\..\patch\ramcode.rom .\output\ramcode.rom || exit /b
@copy ..\..\..\patch\patch.dat + ..\..\..\patch\user.dat + ..\..\..\patch\rf.dat .\output\sched.rom || exit /b
@copy ..\..\..\patch\memmap.format .\output\memmap.format || exit /b
	
@cd output
@..\..\..\..\tools\buildTool\geneep -f -n
@cd ..
