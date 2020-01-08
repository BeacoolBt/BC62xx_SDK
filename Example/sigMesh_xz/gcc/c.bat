::@make clean

@make || exit /b
@cd output
@..\..\..\..\tools\buildTool\geneep -f -n
@cd ..

@e fp || exit /b

choice /t 1 /d y /n >nul
@e ku || exit /b

choice /t 1 /d y /n >nul
@e k || exit /b