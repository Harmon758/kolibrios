if tup.getconfig("NO_FASM") ~= "" then return end
tup.foreach_rule({"ctrldemo.asm", "editbox_ex.asm"}, "fasm %f %o " .. tup.getconfig("KPACK_CMD"), "%B")
