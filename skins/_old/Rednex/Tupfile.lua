if tup.getconfig("NO_FASM") ~= "" then return end
tup.rule("Rednex.asm", 'fasm "%f" "%o" ' .. tup.getconfig("KPACK_CMD"), "Rednex.skn")
