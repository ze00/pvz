$(TARGET):$(DEP)
	@ echo "$($(@)_src) => $@"
	@ $(CC) -o $@ $($(@)_src) $(CC_FLAG)
	@ $(STRIP) $@
