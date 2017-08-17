MODULE := \
	patcher \
	elfreader \
	cheater
inc := $(shell find inc)
CC := gcc
CC_FLAG := -g -Iinc
all:$(MODULE)
$(foreach m,$(MODULE),$(eval $(m)_src := src/$(m).c))
cheater_lib := scanmem
$(foreach m,$(MODULE),$(eval TARGET := $(m))$(eval DEP := $($(m)_src) $(inc))$(eval include build/reg_rule.mk))
.PHONY:clean
clean:
	-@ rm -rf $(MODULE)
