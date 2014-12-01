if tup.getconfig("NO_GCC") ~= "" then return end
HELPERDIR = (tup.getconfig("HELPERDIR") == "") and "../../../programs" or tup.getconfig("HELPERDIR")
tup.include(HELPERDIR .. "/use_gcc.lua")
tup.include(HELPERDIR .. "/use_newlib.lua")
CFLAGS = CFLAGS .. " -UWIN32 -U_WIN32 -U__WIN32__ -DLUA_COMPAT_ALL -DLUA_ANSI -D__DYNAMIC_REENT__"
compile_gcc{
  "lapi.c",
  "lauxlib.c",
  "lbaselib.c",
  "lbitlib.c",
  "lcode.c",
  "lcorolib.c",
  "lctype.c",
  "ldblib.c",
  "ldebug.c",
  "ldo.c",
  "ldump.c",
  "lfunc.c",
  "lgc.c",
  "linit.c",
  "liolib.c",
  "llex.c",
  "lmathlib.c",
  "lmem.c",
  "loadlib.c",
  "lobject.c",
  "lopcodes.c",
  "loslib.c",
  "lparser.c",
  "lstate.c",
  "lstring.c",
  "lstrlib.c",
  "ltable.c",
  "ltablib.c",
  "ltm.c",
  "lua.c",
  "lundump.c",
  "lvm.c",
  "lzio.c",
  "kolibri.c"
}
link_gcc("lua")
