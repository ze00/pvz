MODULE := \
	patcher \
	elfreader \
	cheater
inc := $(shell find inc)
CC_FLAG := -g -Iinc -Wall -std=c99 -pthread
CC_FLAG += -DHAVE_PROCMEM -DDEBUG
ifeq ($(NDK_BUILD),true)
  NDK ?= $(HOME)/android-ndk-r14b
  ifeq ($(ARM64),true)
	NDK_STANDALONE ?= $(HOME)/ndk/arm64
	NDK_TOOCHAIN ?= $(NDK_STANDALONE)/bin/aarch64-linux-android
	NDK_SYSROOT ?= $(NDK)/platforms/android-24/arch-arm64
  else
	NDK_STANDALONE ?= $(HOME)/ndk/arm
	NDK_TOOCHAIN ?= $(NDK_STANDALONE)/bin/arm-linux-androideabi
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
cheater_src += ptrace.c src/utils.c
$(foreach m,$(MODULE),$(eval TARGET := $(m))$(eval DEP := $($(m)_src) $(inc))$(eval include build/reg_rule.mk))
define make_release
	make NDK_BUILD=true --no-print-directory $(1)
	@ cp cheater release/$(2)
	@ make clean
endef
.PHONY:release
release:
	$(call make_release,NOUGHT=true,arm/cheater_nought)
	$(call make_release,,arm/cheater_marshmallow)
	$(call make_release,NOUGHT=true ARM64=true,arm64/cheater_nought)
	$(call make_release,ARM64=true,arm64/cheater_marshmallow)
.PHONY:clean
clean:
	-@ rm -rf $(MODULE)
