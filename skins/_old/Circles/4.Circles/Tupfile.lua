if tup.getconfig("NO_FASM") ~= "" then return end
tup.rule("4.Circles.asm", 'fasm "%f" "%o" ' .. tup.getconfig("KPACK_CMD"), "4.Circles.skn")
