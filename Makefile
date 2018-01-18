MODULE := \
	patcher \
	elfreader \
	cheater
inc := $(shell find inc)
CC_FLAG := -g -Iinc -Wall -std=c99
CC_FLAG += -DHAVE_PROCMEM -DDEBUG
ifeq ($(NDK_BUILD),true)
  ifeq ($(ARM64),true)
	NDK_STANDALONE ?= $(HOME)/ndk/arm64
	NDK_TOOCHAIN ?= $(NDK_STANDALONE)/bin/aarch64-linux-android
	NDK ?= $(HOME)/android-ndk-r14b
	NDK_SYSROOT ?= $(NDK)/platforms/android-24/arch-arm64
  else
	NDK_STANDALONE ?= $(HOME)/ndk/arm
	NDK_TOOCHAIN ?= $(NDK_STANDALONE)/bin/arm-linux-androideabi
	NDK ?= $(HOME)/android-ndk-r14b
	NDK_SYSROOT ?= $(NDK)/platforms/android-24/arch-arm
  endif
	CC := $(NDK_TOOCHAIN)-gcc
	STRIP := $(NDK_TOOCHAIN)-strip
	CC_FLAG += -fPIC -pie --sysroot=$(NDK_SYSROOT)
else
	CC := gcc
	STRIP := strip
endif
ifeq ($(NOUGHT),true)
 CC_FLAG += -DNOUGHT
endif
all:$(MODULE)
$(foreach m,$(MODULE),$(eval $(m)_src := src/$(m).c))
cheater_src += ptrace.c
$(foreach m,$(MODULE),$(eval TARGET := $(m))$(eval DEP := $($(m)_src) $(inc))$(eval include build/reg_rule.mk))
.PHONY:release
release:
	make NDK_BUILD=true NOUGHT=true
	cp cheater release/arm/cheater_nought
	make clean
	make NDK_BUILD=true
	cp cheater release/arm/cheater_marshmallow
	make clean
	make NDK_BUILD=true NOUGHT=true ARM64=true
	cp cheater release/arm64/cheater_nought
	make clean
	make NDK_BUILD=true ARM64=true
	cp cheater release/arm64/cheater_marshmallow
	make clean
.PHONY:clean
clean:
	-@ rm -rf $(MODULE)
