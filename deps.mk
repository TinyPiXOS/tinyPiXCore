core_root = $(CURDIR)
src_dir = $(core_root)/3rdparty

ifndef NOPARALLEL
export MAKEFLAGS+=" -j$$(( $$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || getconf NPROCESSORS_ONLN 2>/dev/null || echo 8) + 1)) "
endif

build ?= build
prefix ?= $(core_root)/tempSubmodule

rime_deps = librime nanomsg thorvg

# 判断平台
# 获取系统架构信息
ARCH := $(shell uname -m)

# 根据架构设置目录变量
ifeq ($(ARCH), x86_64)
    libDir := x86_64
else ifneq (,$(filter $(ARCH), i386 i686))
    libDir := x86_32
else ifneq (,$(filter $(ARCH), armv7l arm))
    libDir := arm_32
else ifneq (,$(filter $(ARCH), aarch64 arm64))
    libDir := arm_64
else
    $(warning 未知架构: $(ARCH), 使用默认目录)
    libDir := x86_64
endif

.PHONY: all clean clean-dist clean-src $(rime_deps)

all: $(rime_deps)

clean: clean-src clean-dist

clean-dist:
	git rev-parse --is-inside-work-tree > /dev/null && \
	find $(prefix)/bin $(prefix)/include $(prefix)/lib $(prefix)/share \
	-depth -maxdepth 1 \
	-exec bash -c 'git ls-files --error-unmatch "$$0" > /dev/null 2>&1 || rm -rv "$$0"' {} \; || true
	rmdir $(prefix) 2> /dev/null || true

# note: this won't clean output files under bin/, include/, lib/ and share/.
clean-src:
	for dep in $(rime_deps); do \
		rm -r $(src_dir)/$${dep}/$(build) || true; \
	done

# apt install libleveldb-dev libmarisa-dev libopencc-dev libyaml-cpp-dev libgoogle-glog-dev
librime:
	cd $(src_dir)/librime; \
	cmake . -B $(build) \
	-DCMAKE_BUILD_TYPE:STRING=Release \
	-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
	--no-warn-unused-cli \
	-G Ninja \
	-DCMAKE_INSTALL_PREFIX:PATH="$(prefix)/librime" \
	-DCMAKE_INSTALL_BINDIR:PATH="$(prefix)/librime/bin" \
	-DCMAKE_INSTALL_LIBDIR:PATH="$(prefix)/librime/lib" \
	-DCMAKE_INSTALL_INCLUDEDIR:PATH="$(prefix)/librime/include" \
	-DEXECUTABLE_OUTPUT_PATH:PATH="$(prefix)/librime/data" \
	&& cmake --build $(build) --target install; \
	cp $(prefix)/librime/data/* $(core_root)/src/data/rime/; \
	cp $(prefix)/librime/include/* $(core_root)/src/include_p/TpUtils/rime/; \
	cp $(prefix)/librime/lib/librime.so.1.5.3 $(core_root)/src/depend_lib/dynamic/$(libDir)/;

nanomsg:
	cd $(src_dir)/nanomsg; \
	cmake . -B $(build) \
	-DCMAKE_BUILD_TYPE:STRING="Release" \
	-DCMAKE_INSTALL_PREFIX:PATH="$(prefix)/nanomsg"; \
	cd $(build)	\
	&& make && make install; \
	cp $(prefix)/nanomsg/include/nanomsg/* $(core_root)/src/include_p/TpUtils/nanomsg/; \
	cp $(prefix)/nanomsg/lib/libnanomsg.a $(core_root)/src/depend_lib/static/$(libDir)/;

thorvg:
	cd $(src_dir)/thorvg; \
	meson setup builddir \
	--buildtype=release \
	-Dloaders="all" \
	-Dsavers="all" \
	-Dexamples=false \
	-Dlog="false" \
	--default-library=static \
	--prefix=$(prefix)/thorvg \
	--libdir=lib \
	&& ninja -C builddir install; \
	cp $(prefix)/thorvg/include/* $(core_root)/src/include_p/TpGUI/thorVG/; \
	cp $(prefix)/thorvg/lib/libthorvg.a $(core_root)/src/depend_lib/static/$(libDir)/;
