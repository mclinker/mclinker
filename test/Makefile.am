
LLC="`$(LLVM_CONFIG_BIN) --bindir`/llc"
MCLD=$(abs_top_builddir)/tools/mcld/ld.mcld
FAKELD=$(abs_top_srcdir)/scripts/bin/ld.fake

LLVM_SRC_ROOT="`$(LLVM_CONFIG_BIN) --src-root`"

# 'lit' is the default test runner.
check: check-local-lit

LIT_ARGS := -s -v
# Newer versions of python try to allocate an insane amount of address
# space for its thread-local storage, don't set a limit here.
# FIXME: Those limits should be enforced by lit instead of globally.
ULIMIT=ulimit -t 600 ; ulimit -d 512000 ;

check-local-lit: lit.site.cfg
	@( $(ULIMIT) \
	  $(LLVM_SRC_ROOT)/utils/lit/lit.py $(LIT_ARGS) . )

lit.site.cfg: FORCE
	@echo "Making MCLinker 'lit.site.cfg' file..."
	@echo s=@LLC@=$(LLC)=g > lit.tmp
	@echo s=@MCLD@=$(MCLD)=g >> lit.tmp
	@echo s=@FAKELD@=$(FAKELD)=g >> lit.tmp
	@echo s=@MCLINKER_SOURCE_ROOT@=$(abs_top_srcdir)=g >> lit.tmp
	@echo s=@MCLINKER_OBJECT_ROOT@=$(abs_top_builddir)=g >> lit.tmp
	@echo s=@LLVM_CONFIG@=$(LLVM_CONFIG_BIN)=g >> lit.tmp
	@sed -f lit.tmp $(abs_srcdir)/lit.site.cfg.in > $@
	@rm -f lit.tmp

clean: FORCE
	rm -f lit.site.cfg
	rm -rf `find $(abs_builddir) -name Output -type d -print`

.PHONY: FORCE

