export MakeInc_cmd=${SRCROOT}/makedefs/MakeInc.cmd
export MakeInc_def=${SRCROOT}/makedefs/MakeInc.def
export MakeInc_rule=${SRCROOT}/makedefs/MakeInc.rule
export MakeInc_dir=${SRCROOT}/makedefs/MakeInc.dir

<<<<<<< HEAD
include $(MakeInc_cmd)
include $(MakeInc_def)

MD_SUPPORTED_KPI_FILENAME = SupportedKPIs-${CURRENT_ARCH_CONFIG_LC}.txt
MI_SUPPORTED_KPI_FILENAME = SupportedKPIs-all-archs.txt

INSTALL_KEXT_DIR = $(DSTROOT)$(INSTALL_EXTENSIONS_DIR)

KEXT_PLIST_LIST = \
	System.kext/Info.plist \
	System.kext/PlugIns/AppleNMI.kext/Info.plist \
	System.kext/PlugIns/ApplePlatformFamily.kext/Info.plist \
	System.kext/PlugIns/IONVRAMFamily.kext/Info.plist \
	System.kext/PlugIns/IOSystemManagement.kext/Info.plist

SYMBOL_COMPONENT_LIST =	\
        BSDKernel	\
        IOKit		\
        Libkern		\
        Mach		\
        MACFramework	\
        Unsupported	\
        Private

# In general you want it to be possible to have a CPU sub-type's symbol exports
# alias to the parent type's exports. This is a special-case way to handle it
# for now:
ifeq ($(CURRENT_ARCH_CONFIG_LC),x86_64h)
EXPORT_SOURCE_ARCH_CONFIG_LC = x86_64
else
EXPORT_SOURCE_ARCH_CONFIG_LC = $(CURRENT_ARCH_CONFIG_LC)
endif

KEXT_MACHO_LIST = $(foreach symbolset,$(filter-out Dummy,$(SYMBOL_COMPONENT_LIST)),System.kext/PlugIns/$(symbolset).kext/$(symbolset))
KEXT_PLIST_LIST += $(foreach symbolset,$(filter-out Dummy,$(SYMBOL_COMPONENT_LIST)),System.kext/PlugIns/$(symbolset).kext/Info.plist)

SYMROOT_INSTALL_KEXT_MACHO_FILES = $(addprefix $(SYMROOT)/,$(KEXT_MACHO_LIST))
DSTROOT_INSTALL_KEXT_MACHO_FILES = $(addprefix $(INSTALL_KEXT_DIR)/,$(KEXT_MACHO_LIST))

SYMROOT_INSTALL_KEXT_PLISTS = $(addprefix $(SYMROOT)/,$(KEXT_PLIST_LIST))
DSTROOT_INSTALL_KEXT_PLISTS = $(addprefix $(INSTALL_KEXT_DIR)/,$(KEXT_PLIST_LIST))

EXPORTS_FILES = $(foreach symbolset,$(SYMBOL_COMPONENT_LIST),$(symbolset).exports $(symbolset).$(EXPORT_SOURCE_ARCH_CONFIG_LC).exports) Unused.exports

SYMBOL_SET_BUILD = $(foreach symbolset, $(SYMBOL_COMPONENT_LIST), $(OBJPATH)/$(symbolset).symbolset)

$(OBJPATH)/allsymbols: $(OBJPATH)/$(KERNEL_FILE_NAME)
	$(_v)$(NM) -gj $< > $@

$(SYMBOL_SET_BUILD): $(OBJPATH)/%.symbolset :  %.exports %.$(EXPORT_SOURCE_ARCH_CONFIG_LC).exports $(OBJPATH)/allsymbols $(KEXT_CREATE_SYMBOL_SET)
	@echo SYMBOLSET $* "($(CURRENT_ARCH_CONFIG_LC))"
	$(_v)$(KEXT_CREATE_SYMBOL_SET)					\
		$(ARCH_FLAGS_$(CURRENT_ARCH_CONFIG))			\
		-import $(OBJPATH)/allsymbols				\
		-export $(SOURCE)/$*.exports				\
		-export $(SOURCE)/$*.$(EXPORT_SOURCE_ARCH_CONFIG_LC).exports	\
		-output $@ $(_vstdout)

.PHONY: check_all_exports

check_all_exports: $(OBJPATH)/allsymbols $(KEXT_CREATE_SYMBOL_SET)
	$(_v)$(KEXT_CREATE_SYMBOL_SET)					\
		$(ARCH_FLAGS_$(CURRENT_ARCH_CONFIG))			\
		-import $(OBJPATH)/allsymbols				\
		$(foreach symbolset,$(filter-out Private,$(SYMBOL_COMPONENT_LIST)),	\
			-export $(SOURCE)/$(symbolset).exports		\
			-export $(SOURCE)/$(symbolset).$(EXPORT_SOURCE_ARCH_CONFIG_LC).exports)	\
		-output /dev/null $(_vstdout)
	$(_v)$(KEXT_CREATE_SYMBOL_SET)					\
		$(ARCH_FLAGS_$(CURRENT_ARCH_CONFIG))			\
		-import $(OBJPATH)/allsymbols				\
		$(foreach symbolset,$(filter-out Unsupported,$(SYMBOL_COMPONENT_LIST)),	\
			-export $(SOURCE)/$(symbolset).exports		\
			-export $(SOURCE)/$(symbolset).$(EXPORT_SOURCE_ARCH_CONFIG_LC).exports)	\
		-output /dev/null $(_vstdout)

$(OBJPATH)/$(MD_SUPPORTED_KPI_FILENAME): $(EXPORTS_FILES)
	@echo SUPPORTED_KPI "($(CURRENT_ARCH_CONFIG_LC))"
	$(_v)$(SRCROOT)/config/list_supported.sh $(SOURCE) $(EXPORT_SOURCE_ARCH_CONFIG_LC) $@

$(OBJPATH)/$(MI_SUPPORTED_KPI_FILENAME): $(EXPORTS_FILES)
	@echo SUPPORTED_KPI "(all)"
	$(_v)$(SRCROOT)/config/list_supported.sh $(SOURCE) all $@

build_symbol_sets: check_all_exports $(SYMBOL_SET_BUILD) $(OBJPATH)/allsymbols \
			$(OBJPATH)/$(MD_SUPPORTED_KPI_FILENAME) \
			$(OBJPATH)/$(MI_SUPPORTED_KPI_FILENAME)

do_config_all::	build_symbol_sets

# There's no simple static pattern rule for these paths, so hardcode dependencies in the command list
$(SYMROOT_INSTALL_KEXT_MACHO_FILES): ALWAYS
	$(_v)$(MKDIR) $(dir $@)
	$(_v)if [ $(OBJROOT)/.symbolset.timestamp -nt $@ ]; then		\
		echo INSTALLSYM symbolset $(notdir $@) "($(CURRENT_ARCH_CONFIG_LC))";	\
		$(INSTALL) $(EXEC_INSTALL_FLAGS) $(OBJPATH)/$(@F).symbolset $@;	\
		cmdstatus=$$?;							\
	else									\
		echo INSTALLSYM symbolset $(notdir $@) "($(CURRENT_ARCH_CONFIG_LC))";	\
		$(LIPO) -create $@ $(OBJPATH)/$(@F).symbolset -output $@ 2>/dev/null || true;	\
		cmdstatus=$$?;							\
	fi;									\
	exit $$cmdstatus

$(SYMROOT_INSTALL_KEXT_PLISTS): $(SYMROOT)/% : $(SOURCE)/%
	$(_v)$(MKDIR) $(dir $@)
	@echo INSTALLSYM kextplist $*
	$(_v)$(INSTALL) $(DATA_INSTALL_FLAGS) $< $@
	$(_v)$(NEWVERS) $@ $(_vstdout)

$(DSTROOT_INSTALL_KEXT_PLISTS): $(INSTALL_KEXT_DIR)/% : $(SYMROOT)/%
	$(_v)$(MKDIR) $(dir $@)
	@echo INSTALL kextplist $*
	$(_v)$(INSTALL) $(DATA_INSTALL_FLAGS) $< $@

$(DSTROOT_INSTALL_KEXT_MACHO_FILES): $(INSTALL_KEXT_DIR)/% : $(SYMROOT)/% ALWAYS
	$(_v)$(MKDIR) $(dir $@)
	@echo INSTALL $(notdir $@) "($(CURRENT_ARCH_CONFIG_LC))"
	$(_v)$(INSTALL) $(EXEC_INSTALL_FLAGS) $< $@

$(DSTROOT)/$(KRESDIR)/$(MD_SUPPORTED_KPI_FILENAME) $(DSTROOT)/$(KRESDIR)/$(MI_SUPPORTED_KPI_FILENAME): $(DSTROOT)/$(KRESDIR)/% : $(OBJPATH)/%
	$(_v)$(MKDIR) $(dir $@)
	@echo INSTALL $*
	$(_v)$(INSTALL) $(INSTALL_FLAGS) $< $@

do_config_install::	$(SYMROOT_INSTALL_KEXT_MACHO_FILES) \
				$(SYMROOT_INSTALL_KEXT_PLISTS) \
				$(DSTROOT_INSTALL_KEXT_MACHO_FILES) \
				$(DSTROOT_INSTALL_KEXT_PLISTS) \
				$(DSTROOT)/$(KRESDIR)/$(MD_SUPPORTED_KPI_FILENAME) \
				$(DSTROOT)/$(KRESDIR)/$(MI_SUPPORTED_KPI_FILENAME)

$(OBJPATH)/all-kpi.exp: $(EXPORTS_FILES)
	$(_v)$(SOURCE)/generate_linker_exports.sh $@ $+

do_build_all:: $(OBJPATH)/all-kpi.exp
=======

include $(MakeInc_cmd)
include $(MakeInc_def)

ALL_SUBDIRS = 

INSTINC_SUBDIRS = 

INSTINC_SUBDIRS_PPC = 

INSTINC_SUBDIRS_I386 = 

EXPINC_SUBDIRS = 

EXPINC_SUBDIRS_PPC =  

EXPINC_SUBDIRS_I386 = 

COMP_SUBDIRS = 

INST_SUBDIRS =	

INSTALL_DATA_LIST= \
	System.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/AppleNMI.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/ApplePlatformFamily.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/BSDKernel.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/IOADBFamily.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/IOKit.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/IONVRAMFamily.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/IOSystemManagement.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/Libkern.kext/Contents/Info.plist \
	System.kext/Contents/PlugIns/Mach.kext/Contents/Info.plist

INSTALL_DATA_DIR= \
	/System/Library/Extensions/

INSTMAN_SUBDIRS = 
<<<<<<< HEAD
>>>>>>> origin/10.2
=======

#

KEXT_CREATE_SYMBOL_SET = /usr/local/bin/kextsymboltool
NEWVERS = $(SRCROOT)/config/newvers.pl

ifneq ($(MACHINE_CONFIG), DEFAULT)
OBJPATH = $(OBJROOT)/$(KERNEL_CONFIG)_$(ARCH_CONFIG)_$(MACHINE_CONFIG)
else
OBJPATH = $(OBJROOT)/$(KERNEL_CONFIG)_$(ARCH_CONFIG)
endif

SYMBOL_COMPONENT_LIST =	\
        System6.0	\
        BSDKernel	\
        IOKit		\
        Libkern	\
        Mach	\
	Unsupported

ifdef MAC
SYMBOL_COMPONENT_LIST += MACFramework
MACFRAMEWORKEXPORTS = \
	-export $(SRCROOT)/$(COMPONENT)/MACFramework.exports \
	-export $(SRCROOT)/$(COMPONENT)/MACFramework.$(ARCH_CONFIG_LC).exports
endif

SYMBOL_SET_BUILD = $(foreach set, $(SYMBOL_COMPONENT_LIST), $(OBJPATH)/$(set).symbolset)
SYMBOL_SET_FAT = $(foreach set, $(SYMBOL_COMPONENT_LIST), $(OBJROOT)/$(set).symbolset)

## .SUFFIXES: .symbolset .symbollist

$(OBJPATH)/allsymbols: $(OBJPATH)/mach_kernel
	$(_v)nm -gj $< > $@

$(SYMBOL_SET_BUILD): $(OBJPATH)/%.symbolset :  %.exports %.$(ARCH_CONFIG_LC).exports $(OBJPATH)/allsymbols
	$(_v)$(KEXT_CREATE_SYMBOL_SET) \
		$($(addsuffix $(ARCH_CONFIG),ARCH_FLAGS_ALL_)) \
		-import $(OBJPATH)/allsymbols \
		-export $*.exports \
		-export $*.$(ARCH_CONFIG_LC).exports \
		-output $@;

ifneq ($(MACHINE_CONFIG), DEFAULT)
$(SYMBOL_SET_FAT): $(OBJROOT)/%.symbolset : $(foreach arch, $(INSTALL_ARCHS), $(OBJROOT)/$(KERNEL_CONFIG)_$(arch)_$(MACHINE_CONFIG)/%.symbolset)
	$(LIPO) $(foreach arch, $(INSTALL_ARCHS), $(OBJROOT)/$(KERNEL_CONFIG)_$(arch)_$(MACHINE_CONFIG)/$*.symbolset) -create -output $@;
else
$(SYMBOL_SET_FAT): $(OBJROOT)/%.symbolset : $(foreach arch, $(INSTALL_ARCHS), $(OBJROOT)/$(KERNEL_CONFIG)_$(arch)/%.symbolset)
	$(_v)$(LIPO) $(foreach arch, $(INSTALL_ARCHS), $(OBJROOT)/$(KERNEL_CONFIG)_$(arch)/$*.symbolset) -create -output $@;
endif

build_symbol_sets:	$(SYMBOL_SET_BUILD)
	$(_v)$(KEXT_CREATE_SYMBOL_SET) \
		$($(addsuffix $(ARCH_CONFIG),ARCH_FLAGS_ALL_)) \
		-import $(OBJPATH)/allsymbols \
		-export $(SRCROOT)/$(COMPONENT)/Libkern.exports \
		-export $(SRCROOT)/$(COMPONENT)/Libkern.$(ARCH_CONFIG_LC).exports \
		-export $(SRCROOT)/$(COMPONENT)/Mach.exports \
		-export $(SRCROOT)/$(COMPONENT)/Mach.$(ARCH_CONFIG_LC).exports \
		-export $(SRCROOT)/$(COMPONENT)/IOKit.exports \
		-export $(SRCROOT)/$(COMPONENT)/IOKit.$(ARCH_CONFIG_LC).exports \
		-export $(SRCROOT)/$(COMPONENT)/BSDKernel.exports \
		-export $(SRCROOT)/$(COMPONENT)/BSDKernel.$(ARCH_CONFIG_LC).exports \
		$(MACFRAMEWORKEXPORTS) \
		-export $(SRCROOT)/$(COMPONENT)/Unsupported.exports \
		-export $(SRCROOT)/$(COMPONENT)/Unsupported.$(ARCH_CONFIG_LC).exports \
		-output /dev/null;

install_symbol_sets:	$(SYMBOL_SET_FAT) $(SRCROOT)/config/MasterVersion
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/System6.0.symbolset $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/System6.0.kext/kernel.6.0;
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/BSDKernel.symbolset $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/BSDKernel.kext/BSDKernel;
ifdef MAC
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/MACFramework.symbolset $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/MACFramework.kext/MACFramework;
endif
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/IOKit.symbolset     $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/IOKit.kext/IOKit;
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/Libkern.symbolset   $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/Libkern.kext/Libkern;
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/Mach.symbolset      $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/Mach.kext/Mach;
	$(_v)install $(INSTALL_FLAGS) $(OBJROOT)/Unsupported.symbolset      $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/Unsupported.kext/Unsupported;
	$(_v)$(NEWVERS) $(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/AppleNMI.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/ApplePlatformFamily.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/BSDKernel.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/MACFramework.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/IOKit.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/IONVRAMFamily.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/IOSystemManagement.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/Libkern.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/Mach.kext/Info.plist \
		$(DSTROOT)/$(INSTALL_DATA_DIR)/System.kext/PlugIns/Unsupported.kext/Info.plist

do_build_all:	build_symbol_sets

do_build_install:	install_symbol_sets
>>>>>>> origin/10.5

include $(MakeInc_rule)
include $(MakeInc_dir)
