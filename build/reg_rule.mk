$(TARGET):$(DEP)
	$(CC) -o $@ $($(@)_src) $(addprefix -l,$($(@)_lib))
