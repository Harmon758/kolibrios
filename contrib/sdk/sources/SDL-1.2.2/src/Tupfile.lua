if tup.getconfig("NO_GCC") ~= "" or tup.getconfig("NO_NASM") ~= "" then return end
tup.include("../../../../../programs/use_gcc.lua")
tup.include("../../../../../programs/use_menuetlibc.lua")
tup.include("../../../../../programs/use_sound.lua")
INCLUDES = INCLUDES .. " -I. -I../include -Ihermes -Iaudio -Ivideo -Ievents -Ijoystick -Icdrom -Ithread -Itimer -Iendian -Ifile"
CFLAGS = CFLAGS .. ' -D_REENTRANT -DPACKAGE=\"SDL\" -DVERSION=\"1.2.2\"'
CFLAGS = CFLAGS .. ' -DENABLE_AUDIO -UDISABLE_AUDIO -DDISABLE_JOYSTICK'
CFLAGS = CFLAGS .. ' -DDISABLE_CDROM -DDISABLE_THREADS -DENABLE_TIMERS'
CFLAGS = CFLAGS .. ' -DUSE_ASMBLIT -DENABLE_MENUETOS -DNO_SIGNAL_H -DDISABLE_STDIO -DNEED_SDL_GETENV'
CFLAGS = CFLAGS .. ' -DENABLE_FILE -UDISABLE_FILE -D__MENUETOS__ -DDEBUG_VIDEO -UWIN32'
FOLDERS = {
  "",
  "audio/",
  "endian/",
  "events/",
  "file/",
  "hermes/",
  "joystick/",
  "thread/",
  "timer/",
  "timer/dummy/",
  "video/",
  "video/menuetos/",
}

for i,v in ipairs(FOLDERS) do
  compile_gcc(v .. "*.c", v .. "%B.o")
  tup.append_table(OBJS,
    tup.foreach_rule(v .. "*.asm", "nasm -f coff -o %o %f", v .. "%B.o")
  )
end
tup.rule(OBJS, "kos32-ar rcs %o %f", {"../../../lib/libSDL.a", "../../../lib/<libSDL>"})
