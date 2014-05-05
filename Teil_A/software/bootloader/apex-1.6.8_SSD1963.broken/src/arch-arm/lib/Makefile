#
# Makefile for APEX loader
#
#   Copyright (C) 2004 Marc Singer
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 2 as published by the Free Software Foundation.
#   Please refer to the file debian/copyright for further details.
#
# Part of the reason for dropping these assembler coded
# implementations is to avoid interwork problems. 

lib-y := div64.o memchr.o memzero.o
#lib-y += memcpy2.o
#lib-y += memcpy.o
#lib-y += relocate.o
#lib-y += entry.o
lib-y += lib1funcs.o
lib-y += cp15.o
lib-y += cp15-invalidate-dcache-va.o cp15-wait.o cp15-cleanall-dcache.o
lib-y += memmove.o
lib-y += muldi3.o
