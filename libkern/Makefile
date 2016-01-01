export MakeInc_cmd=${SRCROOT}/makedefs/MakeInc.cmd
export MakeInc_def=${SRCROOT}/makedefs/MakeInc.def
export MakeInc_rule=${SRCROOT}/makedefs/MakeInc.rule
export MakeInc_dir=${SRCROOT}/makedefs/MakeInc.dir

include $(MakeInc_cmd)
include $(MakeInc_def)

INSTINC_SUBDIRS = \
<<<<<<< HEAD
	libkern
INSTINC_SUBDIRS_X86_64 = libkern
INSTINC_SUBDIRS_X86_64H = libkern
INSTINC_SUBDIRS_ARM = libkern
INSTINC_SUBDIRS_ARM64 = libkern
=======
	libkern \
	uuid

INSTINC_SUBDIRS_PPC = ${INSTINC_SUBDIRS}

INSTINC_SUBDIRS_I386 = ${INSTINC_SUBDIRS}

INSTINC_SUBDIRS_ARM = ${INSTINC_SUBDIRS}
>>>>>>> origin/10.5

EXPINC_SUBDIRS = \
	libkern
EXPINC_SUBDIRS_X86_64 = libkern
EXPINC_SUBDIRS_X86_64H = libkern
EXPINC_SUBDIRS_ARM = libkern
EXPINC_SUBDIRS_ARM64 = libkern

COMP_SUBDIRS = conf kmod

include $(MakeInc_rule)
include $(MakeInc_dir)
