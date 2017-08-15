MODULE := \
	patcher \
	elfreader \
	cheater
cheater_lib := scanmem
CC := gcc
$(foreach m,$(MODULE),$(eval $(m)_src := $(m).c))
all:$(MODULE)
$(MODULE):$(OBJ_SUBDIR)
	$(CC) -o $@ $($(@)_src) $(addprefix -l,$($(@)_lib))
$(OBJ_SUBDIR):
	@ mkdir -p $@
.PHONY:clean
clean:
	-@ rm -rf $(MODULE)
