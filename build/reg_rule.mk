$(TARGET):$(DEP)
	@ echo "$($(@)_src) => $@"
	@ $(CC) -o $@ $($(@)_src) $(addprefix -l,$($(@)_lib)) $(CC_FLAG)
	@ $(STRIP) $@
