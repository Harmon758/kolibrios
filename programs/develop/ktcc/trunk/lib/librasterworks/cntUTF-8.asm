format ELF

include "__lib__.inc"

fun      equ countUTF8Z
fun_str  equ 'cntUTF-8'

section '.text'

fun_name db fun_str, 0

section '.data'

extrn lib_name
public fun

fun dd fun_name
lib dd lib_name
