#!/bin/bash
# This script does for linux the same as build.bat for DOS,
# it compiles the KoOS kernel, hopefully ;-)

	echo "lang fix ru"
	echo "lang fix ru" > lang.inc
	fasm -m 16384 downloader.asm downloader
	rm -f lang.inc
	exit 0




