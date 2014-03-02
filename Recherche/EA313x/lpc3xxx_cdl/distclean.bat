@echo off

Echo Cleaning objs
del /s *.o
Echo Cleaning libs
del /s lib*.a
Echo Cleaning exec
del /s *.elf
del /s *.axf
del /s *.srec
del /s *.map
del /s *.bin
del /s .depend
del /s .viafile
del /s .vias
del /s *.wrn
del /s *.c.orig
del /s *.h.orig

Echo Cleaning out lib directories
for /F %%f in ('dir /s /ad /b .\lib') do del /f/s/q %%f >nul 2>&1 && rd /s/q %%f >nul 2>&1
