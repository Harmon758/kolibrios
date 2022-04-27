if tup.getconfig("NO_FASM") ~= "" then return end
tup.rule("echo lang fix " .. ((tup.getconfig("LANG") == "") and "en" or tup.getconfig("LANG")) .. " > %o", {"lang.inc"})
tup.rule({"bootbios.asm", extra_inputs = {"lang.inc"}}, "fasm %f %o ", "bootbios.bin")
tup.rule({"bootbios.asm", extra_inputs = {"lang.inc"}}, "fasm %f %o -dextended_primary_loader=1", "bootbios.bin.ext_loader")
tup.rule({"bootbios.asm", extra_inputs = {"lang.inc"}}, "fasm %f %o -dpretest_build=1", "bootbios.bin.pretest")
tup.rule({"kernel.asm", extra_inputs = {"bootbios.bin", "lang.inc"}}, "fasm -m 65536 %f %o " .. tup.getconfig("KERPACK_CMD"), "kernel.mnt")
tup.rule({"kernel.asm", extra_inputs = {"bootbios.bin.ext_loader", "lang.inc"}}, "fasm -m 131072 %f %o -s %o.fas -dextended_primary_loader=1" .. tup.getconfig("KERPACK_CMD"), {"kernel.mnt.ext_loader", extra_outputs = {"kernel.mnt.ext_loader.fas"}})
tup.rule({"kernel.asm", extra_inputs = {"bootbios.bin.pretest", "lang.inc"}}, "fasm -m 65536 %f %o -dpretest_build=1 -ddebug_com_base=0xe9", "kernel.mnt.pretest")
tup.rule({"kernel.asm", extra_inputs = {"lang.inc"}}, "fasm -m 65536 %f %o -dUEFI=1 -dextended_primary_loader=1", "kolibri.krn")
