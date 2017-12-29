MODULE := \
	patcher \
	elfreader \
	cheater
inc := $(shell find inc)
NDK_STANDALONE ?= $(HOME)/ndk/arm
NDK_TOOCHAIN ?= $(NDK_STANDALONE)/bin/arm-linux-androideabi
NDK ?= $(HOME)/android-ndk-r14b
NDK_SYSROOT ?= $(NDK)/platforms/android-24/arch-arm
CC := $(NDK_TOOCHAIN)-gcc
CC_FLAG := -static -g -Iinc -Wall -std=c99 --sysroot=$(NDK_SYSROOT)
all:$(MODULE)
$(foreach m,$(MODULE),$(eval $(m)_src := src/$(m).c))
cheater_src += ptrace.c
$(foreach m,$(MODULE),$(eval TARGET := $(m))$(eval DEP := $($(m)_src) $(inc))$(eval include build/reg_rule.mk))
.PHONY:clean
clean:
	-@ rm -rf $(MODULE)
