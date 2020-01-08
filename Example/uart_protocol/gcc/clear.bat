@echo off
echo 存储类型为:eeprom和flash
echo 可以用1表示eeprom
echo 可以用2表示flash
set /P Storage_Type=请输入存储的类型:
e pu

IF "%Storage_Type%" EQU "1" (goto clear_eeprom)

IF "%Storage_Type%" EQU "2" (goto clear_flash)

IF /I "%Storage_Type%" EQU "eeprom" (goto clear_eeprom) 

IF /I "%Storage_Type%" EQU "flash" (goto clear_flash) ELSE (goto clear_eeprom)



:clear_eeprom
echo clear eeprom
e ep empty.dat 0
e ep empty.dat 1000
e ep empty.dat 2000
e ep empty.dat 3000
e ep empty.dat 4000
e ep empty.dat 5000
e ep empty.dat 6000
e ep empty.dat 7000
e ep empty.dat 8000

goto end

:clear_flash
echo clear flash
e fp empty.dat 0
e fp empty.dat 1000
e fp empty.dat 2000
e fp empty.dat 3000
e fp empty.dat 4000
e fp empty.dat 5000
e fp empty.dat 6000
e fp empty.dat 7000
e fp empty.dat 8000
e fp empty.dat 9000
e fp empty.dat a000
e fp empty.dat b000
e fp empty.dat c000
e fp empty.dat d000
e fp empty.dat e000
e fp empty.dat f000
e fp empty.dat 10000
e fp empty.dat 11000
e fp empty.dat 12000
e fp empty.dat 13000
e fp empty.dat 14000
e fp empty.dat 15000
e fp empty.dat 16000
e fp empty.dat 17000
e fp empty.dat 18000
e fp empty.dat 19000
e fp empty.dat 1a000
e fp empty.dat 1b000
e fp empty.dat 1c000
e fp empty.dat 1d000
e fp empty.dat 1e000
e fp empty.dat 1f000

:end
pause