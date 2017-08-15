MODULE := \
	patcher \
	elfreader \
	cheater
cheater_lib := scanmem
inc := $(shell ls *.h)
CC := gcc
all:$(MODULE)
$(foreach m,$(MODULE),$(eval $(m)_src := $(m).c))
$(foreach m,$(MODULE),$(eval TARGET := $(m))$(eval DEP := $($(m)_src) $(inc))$(eval include build/reg_rule.mk))
.PHONY:clean
clean:
	-@ rm -rf $(MODULE)
