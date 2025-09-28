core_root = $(CURDIR)
src_dir = $(core_root)/3rdparty

ifndef NOPARALLEL
export MAKEFLAGS+=" -j$$(( $$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || getconf NPROCESSORS_ONLN 2>/dev/null || echo 8) + 1)) "
endif

build ?= build
prefix ?= $(core_root)/install

rime_deps = librime nanomsg thorvg

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

librime:
	cd $(src_dir)/librime; \
	cmake . -B$(build) \
	-DCMAKE_BUILD_TYPE:STRING=Releas \
	-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
	--no-warn-unused-cli \
	-G Ninja \
	-DCMAKE_INSTALL_PREFIX:PATH="$(prefix)" \
	&& cmake --build $(build) --target install

nanomsg:
	cd $(src_dir)/nanomsg; \
	cmake . -B$(build) \
	-DLEVELDB_BUILD_BENCHMARKS:BOOL=OFF \
	-DLEVELDB_BUILD_TESTS:BOOL=OFF \
	-DCMAKE_BUILD_TYPE:STRING="Release" \
	-DCMAKE_INSTALL_PREFIX:PATH="$(prefix)" \
	&& cmake --build $(build) --target install

thorvg:
	cd $(src_dir)/thorvg; \
	cmake . -B$(build) \
	-DCMAKE_BUILD_TYPE:STRING="Release" \
	-DCMAKE_INSTALL_PREFIX:PATH="$(prefix)" \
	&& cmake --build $(build) --target install
