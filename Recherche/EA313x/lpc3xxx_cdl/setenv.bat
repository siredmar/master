@ECHO OFF
REM *********************************************************************
REM  $Id:: setenv.bat 3573 2010-05-24 19:00:19Z usb10132                $
REM Project: Build System
REM 
REM Description:
REM  This Batch file is used to set up the build environment for all
REM  projects. Run this file from a DOS command prompt window
REM 
REM Usage:
REM usage: setenv bsp_name toolchain [verbose]
REM
REM where bsp_name is either kev7a404, sdk7a404, sdk7a400, evb79524 
REM kev79520, sdk79520, kev75401, sdk75401, kev7a400, phy3180
REM toolchain is either arm (the default), gnu, or ghs
REM
REM example: setenv lh7a404 arm 1
REM *********************************************************************

REM
REM --------------------------------------------------------------------
REM User modifiable variables
REM --------------------------------------------------------------------
SET KEIL_BIN_BASE=C:\Keil\ARM\BIN40
SET KEIL_RVCT=C:\Keil\ARM\RV31
SET IAR_ROOT=C:\Program Files\IAR Systems\Embedded Workbench 5.0

REM
REM --------------------------------------------------------------------
REM Rough validate arguments
REM --------------------------------------------------------------------
SET TEST_TMP=%1
IF %TEST_TMP%TEST EQU TEST GOTO usage
SET TEST_TMP=%2
IF %TEST_TMP%TEST EQU TEST GOTO usage
SET TEST_TMP=%3
IF %TEST_TMP%TEST EQU TEST (
	SET LPC_SILENT=1
) ELSE (
	SET LPC_SILENT=0
)


REM --------------------------------------------------------------------
REM This is the path where the software projects start. This path points
REM to same directory where the setenv.bat file is located. The slashes
REM in the environment variable must be of the '\' type.
REM --------------------------------------------------------------------
SET NXPMCU_WINBASE=%CD%
REM --------------------------------------------------------------------
REM The NXPMCU_SOFTWARE environment variable is the same value as the
REM	NXPMCU_WINBASE variable, except that it is uses Unix style path
REM     slashes. Update: This has been changed to be a relpcaue of the
REM     WINBASE variable as Unix style paths aren't needed.
REM --------------------------------------------------------------------
SET NXPMCU_SOFTWARE=%CD%
SET BSP=
SET CSP=
SET TOOL=

REM --------------------------------------------------------------------
REM Get platform argument 
REM 
REM --------------------------------------------------------------------
FOR /F "tokens=*" %%A IN ('DIR /AD /b csps') DO (
	IF %%A NEQ .svn FOR /F "tokens=*" %%B IN ('DIR /AD /b csps\%%A\bsps') DO (
		IF /I %1 EQU %%B (
			SET BSP=%%B
			SET CSP=%%A
			goto platgood
		) 
	)
)  

IF %CSP%TEST EQU TEST (
	@ECHO %1 is an invalid platform type, must be one of the following
	
	FOR /F "tokens=*" %%A IN ('DIR /AD /b csps') DO (
		IF .svn NEQ %%A FOR /F "tokens=*" %%B IN ('DIR /AD /b csps\%%A\bsps') DO (
			IF .svn NEQ %%B (
				@ECHO %%B
			) 
		)
	)  
	goto error
)

:platgood

REM --------------------------------------------------------------------
REM Different chip and board packages support different tools, so the
REM selected tool must be checked against the selected chip and platform
REM here.
REM --------------------------------------------------------------------
SET MAKDIR=%NXPMCU_WINBASE%\makerule\%CSP%
IF NOT EXIST %NXPMCU_WINBASE%\makerule\%CSP%\make.%CSP%.%2 (
	@ECHO "Unsupported or unknown tool (%2) for chip %CSP%, must be:"
	FOR /F "tokens=2* delims=.." %%A IN ('DIR /b %MAKDIR%') DO (
		@ECHO %%B
	)  
	
	goto error
)

:tooldone
set TOOL=%2

REM --------------------------------------------------------------------
REM Keil toolchain specific settings
REM --------------------------------------------------------------------
IF %TOOL% EQU keil (
	IF EXIST %KEIL_BIN_BASE% (
		@ECHO Using Keil install dir  : %KEIL_BIN_BASE%
		goto keil_path
	) ELSE (
		@ECHO Keil install dir: %KEIL_BIN_BASE% not present on this machine
		@ECHO Please update KEIL_BIN_BASE variable in this batch file
		goto done
	)
) ELSE (
	goto normal
)

:keil_path
SET PATH=%KEIL_BIN_BASE%;%PATH%

:normal

REM --------------------------------------------------------------------
REM Miscellaneous
REM --------------------------------------------------------------------
set GEN=lpc
set PATH=%NXPMCU_WINBASE%\tools;%PATH%


REM --------------------------------------------------------------------
REM Success, display the system configuration status
REM --------------------------------------------------------------------
:success
@ECHO Base LPC install dir    : %NXPMCU_WINBASE%
@ECHO Base LPC unix dir       : %NXPMCU_SOFTWARE%
@ECHO Extra Tool install dir  : %NXPMCU_WINBASE%\tools
@ECHO Selected CSP            : %CSP%
@ECHO Selected BSP            : %BSP%
@ECHO Selected toolchain      : %TOOL%
goto done

REM --------------------------------------------------------------------
REM Error configuring system
REM --------------------------------------------------------------------
:error
@ECHO ERROR configuring build system
@ECHO Configuration attempted with board %1, and tool %2

:usage
@ECHO _________________________________________________
@ECHO usage: setenv bsp_name toolchain [verbose]
@ECHO _
@ECHO where 
@ECHO bsp_name: sdk7a404, sdk7a400, sdk79524 etc
@ECHO toolchain: rvw, gnu, or ads
@ECHO verbose: non-empty string will force verbose build 
@ECHO _
@ECHO Silent build example: setenv sdk7a404 gnu 
@ECHO Verbose build example: setenv sdk7a404 gnu 1 
@ECHO _________________________________________________

:done
