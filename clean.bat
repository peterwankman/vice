@echo off

CALL :cleanprojdir vice
CALL :cleanprojdir bin

IF EXIST ipch rd /q /s ipch
IF EXIST vice.sdf del /q vice.sdf
IF EXIST vice.opensdf del /q vice.opensdf
IF EXIST vice.suo (
	ATTRIB -h vice.suo
	DEL /q vice.suo
)

EXIT /B 0

:cleanprojdir
SETLOCAL

ECHO Cleaning %1

PUSHD %1
CALL :delstarifexist *.user
IF EXIST win32 rd /q /s win32
IF EXIST x64 rd /q /s x64
POPD

ENDLOCAL
EXIT /B

:delstarifexist
SETLOCAL

FOR %%F IN ("%1") DO (
	DEL /q "%%~nxF"
)

ENDLOCAL
EXIT /B