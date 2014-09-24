if tup.getconfig("NO_GCC") ~= "" or tup.getconfig("NO_FASM") ~= "" then return end
tup.include("../../../../../programs/use_gcc.lua")
CFLAGS = CFLAGS_OPTIMIZE_SPEED .. " -c -DBUILD_DLL -DMISSING_SYSCALL_NAMES"
LDFLAGS = "-shared -s -T libcdll.lds --out-implib $(SDK_DIR)/lib/libc.dll.a --image-base 0"
-- LDFLAGS = LDFLAGS .. " --output-def libc.orig.def"

SDK_DIR = "../../.."

LIBC_TOPDIR = "."
LIBC_INCLUDES = "include"
NAME = "libc"
DEFINES = "-D_IEEE_LIBM"
INCLUDES = "-Iinclude"

TOOLCHAIN_LIBPATH = tup.getconfig("TOOLCHAIN_LIBPATH")
-- if not given explicitly in config, try to guess
if TOOLCHAIN_LIBPATH == "" then
  if tup.getconfig("TUP_PLATFORM") == "win32"
  then TOOLCHAIN_LIBPATH="C:\\MinGW\\msys\\1.0\\home\\autobuild\\tools\\win32\\mingw32\\lib"
  else TOOLCHAIN_LIBPATH="/home/autobuild/tools/win32/mingw32/lib"
  end
end
LIBPATH = "-L$(SDK_DIR)/lib -L$(TOOLCHAIN_LIBPATH)"
STATIC_SRCS = {"crt/start.S", "crt/crt1.c", "crt/crt2.c", "crt/chkstk.S", "crt/exit.c", "pe/crtloader.c"}
LIBDLL_SRCS = {"crt/dllstart.c", "crt/chkstk.S", "crt/exit.S", "crt/pseudo-reloc.c", "crt/setjmp.S"}
LIBCDLL_SRCS = {"crt/crtdll.c", "crt/pseudo-reloc.c", "crt/chkstk.S", "crt/exit.S", "pe/loader.c"}
LIBCRT_SRCS = {"crt/start.S", "crt/chkstk.S", "crt/crt3.c", "crt/pseudo-reloc.c", "pe/crtloader.c"}
CORE_SRCS = {
  "argz/buf_findstr.c", "argz/envz_get.c",
  "crt/emutls.c", "crt/thread.S", "crt/tls.S", "crt/setjmp.S", "crt/cpu_features.c",
  "ctype/ctype_.c", "ctype/isascii.c", "ctype/isblank.c", "ctype/isalnum.c",
  "ctype/isalpha.c", "ctype/iscntrl.c", "ctype/isdigit.c", "ctype/islower.c",
  "ctype/isupper.c", "ctype/isprint.c", "ctype/ispunct.c", "ctype/isspace.c",
  "ctype/iswctype.c", "ctype/iswalnum.c", "ctype/iswalpha.c", "ctype/iswblank.c",
  "ctype/iswcntrl.c", "ctype/iswdigit.c", "ctype/iswgraph.c", "ctype/iswlower.c",
  "ctype/iswprint.c", "ctype/iswpunct.c", "ctype/iswspace.c", "ctype/iswupper.c",
  "ctype/iswxdigit.c", "ctype/isxdigit.c", "ctype/toascii.c", "ctype/tolower.c",
  "ctype/toupper.c", "ctype/towctrans.c", "ctype/towlower.c", "ctype/towupper.c",
  "ctype/wctrans.c", "ctype/wctype.c",
  "errno/errno.c",
  "locale/locale.c", "locale/lctype.c", "locale/ldpart.c",
  "reent/impure.c", "reent/init_reent.c", "reent/getreent.c", "reent/mutex.c",
  "reent/gettimeofdayr.c", "reent/isattyr.c", "reent/openr.c", "reent/closer.c",
  "reent/linkr.c", "reent/readr.c", "reent/lseekr.c", "reent/fstatr.c",
  "reent/writer.c", "reent/timesr.c", "reent/unlinkr.c",
  "search/qsort.c", "search/bsearch.c",
  "signal/signal.c",
  "sys/close.c", "sys/create.c", "sys/delete.c", "sys/errno.c", "sys/finfo.c",
  "sys/fsize.c", "sys/fstat.c", "sys/gettod.c", "sys/io.c", "sys/ioread.c",
  "sys/iowrite.c", "sys/isatty.c", "sys/lseek.c", "sys/open.c", "sys/read.c",
  "sys/stat.c", "sys/unlink.c", "sys/write.c", "sys/io_alloc.S",
  "time/asctime.c", "time/asctime_r.c", "time/clock.c", "time/ctime.c",
  "time/ctime_r.c", "time/difftime.c", "time/gettzinfo.c", "time/gmtime.c",
  "time/gmtime_r.c", "time/mktime.c", "time/mktm_r.c", "time/lcltime.c",
  "time/lcltime_r.c", "time/strftime.c", "time/time.c", "time/timelocal.c",
  "time/tzlock.c", "time/tzvars.c"
}
STDLIB_SRCS = {
  "__atexit.c", "__call_atexit.c", "abort.c", "abs.c", "assert.c", "atexit.c",
  "atof.c", "atoi.c", "atol.c", "div.c", "dtoa.c", "dtoastub.c", "exit.c",
  "gdtoa-gethex.c", "gdtoa-hexnan.c", "getenv.c", "mprec.c", "mbtowc.c",
  "mbtowc_r.c", "mbrtowc.c", "mlock.c", "calloc.c", "malloc.c", "mallocr.c",
  "rand.c", "rand_r.c", "rand48.c", "realloc.c", "seed48.c", "srand48.c",
  "strtod.c", "strtol.c", "strtold.c", "strtoll.c", "strtoll_r.c", "strtoul.c",
  "strtoull.c", "strtoull_r.c", "system.c", "wcrtomb.c", "wctomb_r.c"
}
STRING_SRCS = {
  "memcpy.c", "memcmp.c", "memmove.c", "memset.c", "memchr.c", "stpcpy.c",
  "stpncpy.c", "strcat.c", "strchr.c", "strcmp.c", "strcoll.c", "strcasecmp.c",
  "strncasecmp.c", "strncat.c", "strncmp.c", "strncpy.c", "strndup.c",
  "strndup_r.c", "strnlen.c", "strcasestr.c", "strdup.c", "strdup_r.c",
  "strerror.c", "strlen.c", "strrchr.c", "strpbrk.c", "strsep.c", "strstr.c",
  "strtok.c", "strtok_r.c", "strupr.c", "strcspn.c", "strspn.c", "strcpy.c",
  "u_strerr.c"
}

STDIO_SRCS = {
  "clearerr.c", "diprintf.c", "dprintf.c", "printf.c", "putchar.c", "fgetc.c",
  "fgets.c", "fopen.c", "fclose.c", "fdopen.c", "fflush.c", "flags.c",
  "fileno.c", "findfp.c", "fiprintf.c", "fiscanf.c", "fprintf.c", "fputc.c",
  "fputs.c", "fputwc.c", "fread.c", "freopen.c", "fscanf.c", "fseek.c",
  "fseeko.c", "ftell.c", "ftello.c", "fwrite.c", "fvwrite.c", "fwalk.c",
  "putc.c", "puts.c", "refill.c", "rget.c", "remove.c", "setvbuf.c", "stdio.c",
  "tmpfile.c", "tmpnam.c", "ungetc.c", "vasniprintf.c", "vasnprintf.c",
  "vdprintf.c", "vdiprintf.c", "vscanf.c", "vsprintf.c", "vsnprintf.c",
  "vsscanf.c", "makebuf.c", "wsetup.c", "wbuf.c", "sccl.c", "siprintf.c",
  "sniprintf.c", "snprintf.c", "sprintf.c", "sscanf.c"
}

MATH_SRCS = {
  "e_acos.c", "e_acosh.c", "e_asin.c", "e_atan2.c", "e_atanh.c", "e_cosh.c", "e_exp.c", "e_fmod.c",
  "e_hypot.c", "e_j0.c", "e_j1.c", "e_jn.c", "e_log.c", "e_log10.c", "e_pow.c", "e_rem_pio2.c",
  "e_remainder.c", "e_scalb.c", "e_sinh.c", "e_sqrt.c", "ef_acos.c", "ef_acosh.c", "ef_asin.c",
  "ef_atan2.c", "ef_atanh.c", "ef_cosh.c", "ef_exp.c", "ef_fmod.c", "ef_hypot.c", "ef_j0.c", "ef_j1.c",
  "ef_jn.c", "ef_log.c", "ef_log10.c", "ef_pow.c", "ef_rem_pio2.c", "ef_remainder.c", "ef_scalb.c",
  "ef_sinh.c", "ef_sqrt.c", "er_gamma.c", "er_lgamma.c", "erf_gamma.c", "erf_lgamma.c", "f_exp.c",
  "f_expf.c", "f_llrint.c", "f_llrintf.c", "f_llrintl.c", "f_lrint.c", "f_lrintf.c", "f_lrintl.c",
  "f_pow.c", "f_powf.c", "f_rint.c", "f_rintf.c", "f_rintl.c", "feclearexcept.c", "fetestexcept.c",
  "k_cos.c", "k_rem_pio2.c", "k_sin.c", "k_standard.c", "k_tan.c", "kf_cos.c", "kf_rem_pio2.c", "kf_sin.c",
  "kf_tan.c", "s_asinh.c", "s_atan.c", "s_cbrt.c", "s_ceil.c", "s_copysign.c", "s_cos.c", "s_erf.c", "s_exp10.c", "s_expm1.c",
  "s_fabs.c", "s_fdim.c", "s_finite.c", "s_floor.c", "s_fma.c", "s_fmax.c", "s_fmin.c", "s_fpclassify.c",
  "s_frexp.c", "s_ilogb.c", "s_infconst.c", "s_infinity.c", "s_isinf.c", "s_isinfd.c", "s_isnan.c",
  "s_isnand.c", "s_ldexp.c", "s_lib_ver.c", "s_llrint.c", "s_llround.c", "s_log1p.c", "s_log2.c",
  "s_logb.c", "s_lrint.c", "s_lround.c", "s_matherr.c", "s_modf.c", "s_nan.c", "s_nearbyint.c",
  "s_nextafter.c", "s_pow10.c", "s_remquo.c", "s_rint.c", "s_round.c", "s_scalbln.c", "s_scalbn.c",
  "s_signbit.c", "s_signif.c", "s_sin.c", "s_tan.c", "s_tanh.c", "s_trunc.c", "scalblnl.c", "scalbnl.c",
  "sf_asinh.c", "sf_atan.c", "sf_cbrt.c", "sf_ceil.c", "sf_copysign.c", "sf_cos.c", "sf_erf.c",
  "sf_exp10.c", "sf_expm1.c", "sf_fabs.c", "sf_fdim.c", "sf_finite.c", "sf_floor.c", "sf_fma.c",
  "sf_fmax.c", "sf_fmin.c", "sf_fpclassify.c", "sf_frexp.c", "sf_ilogb.c", "sf_infinity.c",
  "sf_isinf.c", "sf_isinff.c", "sf_isnan.c", "sf_isnanf.c", "sf_ldexp.c", "sf_llrint.c",
  "sf_llround.c", "sf_log1p.c", "sf_log2.c", "sf_logb.c", "sf_lrint.c", "sf_lround.c", "sf_modf.c",
  "sf_nan.c", "sf_nearbyint.c", "sf_nextafter.c", "sf_pow10.c", "sf_remquo.c", "sf_rint.c",
  "sf_round.c", "sf_scalbln.c", "sf_scalbn.c", "sf_signif.c", "sf_sin.c", "sf_tan.c", "sf_tanh.c",
  "sf_trunc.c", "w_acos.c", "w_acosh.c", "w_asin.c", "w_atan2.c", "w_atanh.c", "w_cosh.c", "w_drem.c",
  "w_exp.c", "w_exp2.c", "w_fmod.c", "w_gamma.c", "w_hypot.c", "w_j0.c", "w_j1.c", "w_jn.c", "w_lgamma.c",
  "w_log.c", "w_log10.c", "w_pow.c", "w_remainder.c", "w_scalb.c", "w_sincos.c", "w_sinh.c", "w_sqrt.c",
  "w_tgamma.c", "wf_acos.c", "wf_acosh.c", "wf_asin.c", "wf_atan2.c", "wf_atanh.c", "wf_cosh.c",
  "wf_drem.c", "wf_exp.c", "wf_exp2.c", "wf_fmod.c", "wf_gamma.c", "wf_hypot.c", "wf_j0.c", "wf_j1.c",
  "wf_jn.c", "wf_lgamma.c", "wf_log.c", "wf_log10.c", "wf_pow.c", "wf_remainder.c", "wf_scalb.c",
  "wf_sincos.c", "wf_sinh.c", "wf_sqrt.c", "wf_tgamma.c", "wr_gamma.c", "wr_lgamma.c", "wrf_gamma.c",
  "wrf_lgamma.c",
  "f_atan2.S", "f_atan2f.S", "f_frexp.S", "f_frexpf.S", "f_ldexp.S", "f_ldexpf.S", "f_log.S",
  "f_log10.S", "f_log10f.S", "f_logf.S", "f_tan.S", "f_tanf.S"
}

function prepend(what, to)
  local result = {}
  for i,v in ipairs(to) do
    table.insert(result, what .. v)
  end
  return result
end

-- make shared
LIB_SRCS = LIBCDLL_SRCS

LIB_SRCS += CORE_SRCS
LIB_SRCS += prepend("stdio/", STDIO_SRCS)
LIB_SRCS += prepend("string/", STRING_SRCS)
LIB_SRCS += prepend("stdlib/", STDLIB_SRCS)
LIB_SRCS += prepend("math/", MATH_SRCS)

ALL_OBJS = {}
function compile(list)
  local result = {}
  for i,v in ipairs(list) do
    if ALL_OBJS[v] then
      -- already compiled
    elseif v:sub(-2) == ".c" or v:sub(-2) == ".S" then
      ALL_OBJS[v] = tup.rule(v, "kos32-gcc $(CFLAGS) $(DEFINES) $(INCLUDES) -o %o %f", v:sub(1, -3) .. ".o")
    elseif v:sub(-4) == ".asm" then
      ALL_OBJS[v] = tup.rule(v, "fasm %f %o", v:sub(1, -5) .. ".obj")
    end
    result += ALL_OBJS[v]
  end
  return result
end

LIB_OBJS = compile(LIB_SRCS)
LIBCRT_OBJS = compile(LIBCRT_SRCS)
LIBDLL_OBJS = compile(LIBDLL_SRCS)

vfprintf_extra_objs = {
  {"-DFLOATING_POINT", "stdio/vfprintf.o"},
  {"-DINTEGER_ONLY", "stdio/vfiprintf.o"},
  {"-DSTRING_ONLY", "stdio/svfprintf.o"},
  {"-DINTEGER_ONLY -DSTRING_ONLY", "stdio/svfiprintf.o"},
}
for i,v in ipairs(vfprintf_extra_objs) do
  LIB_OBJS += tup.rule("stdio/vfprintf.c", "kos32-gcc $(CFLAGS) $(DEFINES) $(INCLUDES) -fshort-enums " .. v[1] .. " -c %f -o %o", v[2])
end

vfscanf_extra_objs = {
  {"", "stdio/vfscanf.o"},
  {"-DINTEGER_ONLY", "stdio/vfiscanf.o"},
  {"-DSTRING_ONLY", "stdio/svscanf.o"},
  {"-DINTEGER_ONLY -DSTRING_ONLY", "stdio/svfiscanf.o"},
}
for i,v in ipairs(vfscanf_extra_objs) do
  LIB_OBJS += tup.rule("stdio/vfscanf.c", "kos32-gcc $(CFLAGS) $(DEFINES) $(INCLUDES) -fshort-enums " .. v[1] .. " -c %f -o %o", v[2])
end

tup.rule(LIB_OBJS, "kos32-ld " .. LDFLAGS .. " " .. LIBPATH .. " -o %o %f -lgcc --version-script libc.ver " .. tup.getconfig("KPACK_CMD"),
  {SDK_DIR .. "/bin/libc.dll", extra_outputs = {SDK_DIR .. "/lib/libc.dll.a", SDK_DIR .. "/lib/<libc.dll.a>"}})
tup.rule(LIBCRT_OBJS, "kos32-ar rcs %o %f", {SDK_DIR .. "/lib/libapp.a", extra_outputs = {SDK_DIR .. "/lib/<libapp.a>"}})
tup.rule(LIBDLL_OBJS, "kos32-ar rcs %o %f", {SDK_DIR .. "/lib/libdll.a", extra_outputs = {SDK_DIR .. "/lib/<libdll.a>"}})
