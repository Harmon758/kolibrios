if tup.getconfig("NO_FASM") ~= "" then return end
tup.foreach_rule({"vox_creator.asm", "vox_mover.asm"}, "fasm %f %o " .. tup.getconfig("KPACK_CMD"), "%B")
