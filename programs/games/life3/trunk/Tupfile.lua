if tup.getconfig("NO_FASM") ~= "" then return end
tup.rule("echo lang fix " .. ((tup.getconfig("LANG") == "") and "en" or tup.getconfig("LANG")) .. " > lang.inc", {"lang.inc"})
tup.rule("life3.asm", "fasm %f %o " .. tup.getconfig("KPACK_CMD"), "life3")
