if tup.getconfig("NO_FASM") ~= "" then return end
tup.rule("OpusN.asm", 'fasm "%f" "%o" ' .. tup.getconfig("KPACK_CMD"), "OpusN.skn")
