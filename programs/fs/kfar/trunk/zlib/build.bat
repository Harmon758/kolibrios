@fasm.exe -m 32768 zlib.asm zlib.obj
@kpack zlib.obj
@fasm.exe -m 32768 example1.asm example1.kex
@kpack example1.kex
pause