@erase lang.inc
@echo lang fix ru >lang.inc
@fasm magnify.asm magnify
@kpack magnify
@erase lang.inc
@pause