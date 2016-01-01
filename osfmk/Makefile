export MakeInc_cmd=${SRCROOT}/makedefs/MakeInc.cmd
export MakeInc_def=${SRCROOT}/makedefs/MakeInc.def
export MakeInc_rule=${SRCROOT}/makedefs/MakeInc.rule
export MakeInc_dir=${SRCROOT}/makedefs/MakeInc.dir

include $(MakeInc_cmd)
include $(MakeInc_def)

INSTINC_SUBDIRS = \
	mach	\
<<<<<<< HEAD
	atm 	\
	corpses	\
	bank \
=======
	machine	\
	default_pager \
>>>>>>> origin/10.2
	device \
	default_pager \
	mach_debug \
	kern \
	ipc \
	machine \
	UserNotification \
	gssd \
	kextd \
	lockd \
	vm \
	libsa \
	kdp \
	kperf \
	prng

INSTINC_SUBDIRS_X86_64 = \
	mach	\
	i386	\
	x86_64

<<<<<<< HEAD
INSTINC_SUBDIRS_X86_64H = \
	mach	\
	i386	\
	x86_64
=======
INSTINC_SUBDIRS_PPC = \
	mach \
	ppc	\
	profiling 
>>>>>>> origin/10.2

INSTINC_SUBDIRS_ARM = \
	mach	\
	arm	\
	arm64
INSTINC_SUBDIRS_ARM64 = \
	mach	\
<<<<<<< HEAD
	arm	\
	arm64
=======
	i386	\
	profiling
>>>>>>> origin/10.2

EXPINC_SUBDIRS = \
	mach	\
	atm 	\
	corpses	\
	bank \
	device \
	default_pager \
	mach_debug \
	kern \
	kdp \
	ipc \
	machine \
	UserNotification \
	gssd \
	kextd \
	lockd \
	vm \
	libsa \
	console \
	kperf \
	prng


EXPINC_SUBDIRS_X86_64 = \
	mach	\
	i386	\
	x86_64
EXPINC_SUBDIRS_X86_64H = \
	mach	\
	i386	\
	x86_64
EXPINC_SUBDIRS_ARM = \
	mach	\
	arm	\
	arm64
EXPINC_SUBDIRS_ARM64 = \
	mach	\
	arm	\
	arm64

COMP_SUBDIRS = 	\
	conf

include $(MakeInc_rule)
include $(MakeInc_dir)
