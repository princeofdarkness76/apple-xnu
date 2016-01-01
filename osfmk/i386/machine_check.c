/*
 * Copyright (c) 2007-2011 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */

#include <kern/kalloc.h>
#include <mach/mach_time.h>
#include <i386/cpu_data.h>
#include <i386/cpuid.h>
#include <i386/cpu_topology.h>
#include <i386/cpu_threads.h>
<<<<<<< HEAD
<<<<<<< HEAD
#include <i386/lapic.h>
=======
>>>>>>> origin/10.5
=======
#include <i386/lapic.h>
>>>>>>> origin/10.9
#include <i386/machine_cpu.h>
#include <i386/machine_check.h>
#include <i386/proc_reg.h>

/*
 * At the time of the machine-check exception, all hardware-threads panic.
 * Each thread saves the state of its MCA registers to its per-cpu data area.
 *
 * State reporting is serialized so one thread dumps all valid state for all
 * threads to the panic log. This may entail spinning waiting for other
 * threads to complete saving state to memory. A timeout applies to this wait
 * -- in particular, a 3-strikes timeout may prevent a thread from taking
 * part is the affair.
 */

#define IF(bool,str)	((bool) ? (str) : "")

static boolean_t	mca_initialized = FALSE;
static boolean_t	mca_MCE_present = FALSE;
static boolean_t	mca_MCA_present = FALSE;
static uint32_t		mca_family = 0;
static unsigned int	mca_error_bank_count = 0;
static boolean_t	mca_control_MSR_present = FALSE;
<<<<<<< HEAD
<<<<<<< HEAD
=======
static boolean_t	mca_threshold_status_present = FALSE;
static boolean_t	mca_extended_MSRs_present = FALSE;
static unsigned int	mca_extended_MSRs_count = 0;
>>>>>>> origin/10.5
=======
>>>>>>> origin/10.9
static boolean_t	mca_cmci_present = FALSE;
static ia32_mcg_cap_t	ia32_mcg_cap;
decl_simple_lock_data(static, mca_lock);

typedef struct {
	ia32_mci_ctl_t		mca_mci_ctl;
	ia32_mci_status_t	mca_mci_status;
	ia32_mci_misc_t		mca_mci_misc;
	ia32_mci_addr_t		mca_mci_addr;
} mca_mci_bank_t;

typedef struct mca_state {
	boolean_t		mca_is_saved;
	boolean_t		mca_is_valid;	/* some state is valid */
	ia32_mcg_ctl_t		mca_mcg_ctl;
	ia32_mcg_status_t	mca_mcg_status;
	mca_mci_bank_t		mca_error_bank[0];
} mca_state_t;

typedef enum {
	CLEAR,
	DUMPING,
	DUMPED
} mca_dump_state_t;
static volatile mca_dump_state_t mca_dump_state = CLEAR;

static void
mca_get_availability(void)
{
	uint64_t	features = cpuid_info()->cpuid_features;
	uint32_t	family =   cpuid_info()->cpuid_family;
	uint32_t	model =    cpuid_info()->cpuid_model;
	uint32_t	stepping = cpuid_info()->cpuid_stepping;
<<<<<<< HEAD

	if ((model == CPUID_MODEL_HASWELL     && stepping < 3) ||
	    (model == CPUID_MODEL_HASWELL_ULT && stepping < 1) ||
	    (model == CPUID_MODEL_CRYSTALWELL && stepping < 1))
		panic("Haswell pre-C0 steppings are not supported");
=======
>>>>>>> origin/10.8

	mca_MCE_present = (features & CPUID_FEATURE_MCE) != 0;
	mca_MCA_present = (features & CPUID_FEATURE_MCA) != 0;
	mca_family = family;

<<<<<<< HEAD
=======
	if ((model == CPUID_MODEL_HASWELL     && stepping < 3) ||
	    (model == CPUID_MODEL_HASWELL_ULT && stepping < 1) ||
	    (model == CPUID_MODEL_CRYSTALWELL && stepping < 1))
		panic("Haswell pre-C0 steppings are not supported");

>>>>>>> origin/10.8
	/*
	 * If MCA, the number of banks etc is reported by the IA32_MCG_CAP MSR.
	 */
	if (mca_MCA_present) {
		ia32_mcg_cap.u64 = rdmsr64(IA32_MCG_CAP);
		mca_error_bank_count = ia32_mcg_cap.bits.count;
		mca_control_MSR_present = ia32_mcg_cap.bits.mcg_ctl_p;
<<<<<<< HEAD
<<<<<<< HEAD
		mca_cmci_present = ia32_mcg_cap.bits.mcg_ext_corr_err_p;
=======
		mca_threshold_status_present = ia32_mcg_cap.bits.mcg_tes_p;
		mca_cmci_present = ia32_mcg_cap.bits.mcg_ext_corr_err_p;
		if (family == 0x0F) {
			mca_extended_MSRs_present = ia32_mcg_cap.bits.mcg_ext_p;
			mca_extended_MSRs_count = ia32_mcg_cap.bits.mcg_ext_cnt;
		}
>>>>>>> origin/10.5
=======
		mca_cmci_present = ia32_mcg_cap.bits.mcg_ext_corr_err_p;
>>>>>>> origin/10.9
	}
}

void
mca_cpu_init(void)
{
	unsigned int	i;

	/*
	 * The first (boot) processor is responsible for discovering the
	 * machine check architecture present on this machine.
	 */
	if (!mca_initialized) {
		mca_get_availability();
		mca_initialized = TRUE;
		simple_lock_init(&mca_lock, 0);
	}

	if (mca_MCA_present) {

		/* Enable all MCA features */
		if (mca_control_MSR_present)
			wrmsr64(IA32_MCG_CTL, IA32_MCG_CTL_ENABLE);
	
		switch (mca_family) {
		case 0x06:
			/* Enable all but mc0 */
			for (i = 1; i < mca_error_bank_count; i++)
				wrmsr64(IA32_MCi_CTL(i),0xFFFFFFFFFFFFFFFFULL); 
			
			/* Clear all errors */
			for (i = 0; i < mca_error_bank_count; i++)
				wrmsr64(IA32_MCi_STATUS(i), 0ULL);
			break;
		case 0x0F:
			/* Enable all banks */
			for (i = 0; i < mca_error_bank_count; i++)
				wrmsr64(IA32_MCi_CTL(i),0xFFFFFFFFFFFFFFFFULL); 
			
			/* Clear all errors */
			for (i = 0; i < mca_error_bank_count; i++)
				wrmsr64(IA32_MCi_STATUS(i), 0ULL);
			break;
		}
	}

	/* Enable machine check exception handling if available */
	if (mca_MCE_present) {
		set_cr4(get_cr4()|CR4_MCE);
	}
}

boolean_t
mca_is_cmci_present(void)
{
	if (!mca_initialized)
		mca_cpu_init();
	return mca_cmci_present;
}

void
mca_cpu_alloc(cpu_data_t	*cdp)
{
	vm_size_t	mca_state_size;

	/*
	 * Allocate space for an array of error banks.
	 */
	mca_state_size = sizeof(mca_state_t) +
				sizeof(mca_mci_bank_t) * mca_error_bank_count;
	cdp->cpu_mca_state = kalloc(mca_state_size);
	if (cdp->cpu_mca_state == NULL) {
		printf("mca_cpu_alloc() failed for cpu %d\n", cdp->cpu_number);
		return;
	}
	bzero((void *) cdp->cpu_mca_state, mca_state_size);

	/*
	 * If the boot processor is yet have its allocation made,
	 * do this now.
	 */
	if (cpu_datap(master_cpu)->cpu_mca_state == NULL)
		mca_cpu_alloc(cpu_datap(master_cpu));
}

static void
mca_save_state(mca_state_t *mca_state)
{
	mca_mci_bank_t  *bank;
	unsigned int	i;

	assert(!ml_get_interrupts_enabled() || get_preemption_level() > 0);

	if  (mca_state == NULL)
		return;

	mca_state->mca_mcg_ctl = mca_control_MSR_present ?
					rdmsr64(IA32_MCG_CTL) : 0ULL;	
	mca_state->mca_mcg_status.u64 = rdmsr64(IA32_MCG_STATUS);

 	bank = (mca_mci_bank_t *) &mca_state->mca_error_bank[0];
	for (i = 0; i < mca_error_bank_count; i++, bank++) {
		bank->mca_mci_ctl        = rdmsr64(IA32_MCi_CTL(i));	
		bank->mca_mci_status.u64 = rdmsr64(IA32_MCi_STATUS(i));	
		if (!bank->mca_mci_status.bits.val)
			continue;
		bank->mca_mci_misc = (bank->mca_mci_status.bits.miscv)?
					rdmsr64(IA32_MCi_MISC(i)) : 0ULL;	
		bank->mca_mci_addr = (bank->mca_mci_status.bits.addrv)?
					rdmsr64(IA32_MCi_ADDR(i)) : 0ULL;	
		mca_state->mca_is_valid = TRUE;
	} 

	/*
	 * If we're the first thread with MCA state, point our package to it
	 * and don't care about races
	 */
	if (x86_package()->mca_state == NULL)
		x86_package()->mca_state = mca_state;
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/10.9

	mca_state->mca_is_saved = TRUE;
=======
>>>>>>> origin/10.5
}

void
mca_check_save(void)
{
	if (mca_dump_state > CLEAR)
		mca_save_state(current_cpu_datap()->cpu_mca_state);
}

<<<<<<< HEAD
static void
mca_report_cpu_info(void)
{
	i386_cpu_info_t *infop = cpuid_info();

	kdb_printf(" family: %d model: %d stepping: %d microcode: %d\n",
		infop->cpuid_family,
		infop->cpuid_model,
		infop->cpuid_stepping,
		infop->cpuid_microcode_version);
	kdb_printf(" signature: 0x%x\n",
		infop->cpuid_signature);
	kdb_printf(" %s\n",
		infop->cpuid_brand_string);

}

=======
>>>>>>> origin/10.9
static void
mca_dump_bank(mca_state_t *state, int i)
{
	mca_mci_bank_t		*bank;
	ia32_mci_status_t	status;

	bank = &state->mca_error_bank[i];
	status = bank->mca_mci_status;
	if (!status.bits.val)
		return;

	kdb_printf(" IA32_MC%d_STATUS(0x%x): 0x%016qx\n",
		i, IA32_MCi_STATUS(i), status.u64);

<<<<<<< HEAD
	if (status.bits.addrv)
		kdb_printf(" IA32_MC%d_ADDR(0x%x):   0x%016qx\n",
			i, IA32_MCi_ADDR(i), bank->mca_mci_addr);

	if (status.bits.miscv)
		kdb_printf(" IA32_MC%d_MISC(0x%x):   0x%016qx\n",
			i, IA32_MCi_MISC(i), bank->mca_mci_misc);
}

static void
mca_cpu_dump_error_banks(mca_state_t *state)
{
	unsigned int 		i;

	if (!state->mca_is_valid)
		return;

	for (i = 0; i < mca_error_bank_count; i++ ) {
=======
static void
mca_report_cpu_info(void)
{
	uint64_t	microcode;
	i386_cpu_info_t *infop = cpuid_info();

	// microcode revision is top 32 bits of MSR_IA32_UCODE_REV
	microcode = rdmsr64(MSR_IA32_UCODE_REV) >> 32;
	kdb_printf(" family: %d model: %d stepping: %d microcode: %d\n",
		infop->cpuid_family,
		infop->cpuid_model,
		infop->cpuid_stepping,
<<<<<<< HEAD
		(uint32_t) microcode);
	kdb_printf(" %s\n", infop->cpuid_brand_string);
}

static const char *mc8_memory_operation[] = {
	[MC8_MMM_GENERIC]		"generic",
	[MC8_MMM_READ]			"read",
	[MC8_MMM_WRITE]			"write",
	[MC8_MMM_ADDRESS_COMMAND]	"address/command",
	[MC8_MMM_RESERVED]		"reserved"
};

static void
mca_dump_bank_mc8(mca_state_t *state, int i)
{
	mca_mci_bank_t			*bank;
	ia32_mci_status_t		status;
	struct ia32_mc8_specific	mc8;
	int				mmm;

	bank = &state->mca_error_bank[i];
	status = bank->mca_mci_status;
	mc8 = status.bits_mc8;
	mmm = MIN(mc8.memory_operation, MC8_MMM_RESERVED);

	kdb_printf(
		" IA32_MC%d_STATUS(0x%x): 0x%016qx %svalid\n",
		i, IA32_MCi_STATUS(i), status.u64, IF(!status.bits.val, "in"));
	if (!status.bits.val)
		return;

	kdb_printf(
		"  Channel number:         %d%s\n"
		"  Memory Operation:       %s\n"
		"  Machine-specific error: %s%s%s%s%s%s%s%s\n"
		"  COR_ERR_CNT:            %d\n",
		mc8.channel_number,
		IF(mc8.channel_number == 15, " (unknown)"),
		mc8_memory_operation[mmm],
		IF(mc8.read_ecc,            "Read ECC"),
		IF(mc8.ecc_on_a_scrub,      "ECC on scrub"),
		IF(mc8.write_parity,        "Write parity"),
		IF(mc8.redundant_memory,    "Redundant memory"),
		IF(mc8.sparing,	            "Sparing/Resilvering"),
		IF(mc8.access_out_of_range, "Access out of Range"),
		IF(mc8.address_parity,      "Address Parity"),
		IF(mc8.byte_enable_parity,  "Byte Enable Parity"),
		mc8.cor_err_cnt);
	kdb_printf(
		"  Status bits:\n%s%s%s%s%s%s",
		IF(status.bits.pcc,	    "   Processor context corrupt\n"),
		IF(status.bits.addrv,	    "   ADDR register valid\n"),
		IF(status.bits.miscv,	    "   MISC register valid\n"),
		IF(status.bits.en,	    "   Error enabled\n"),
		IF(status.bits.uc,	    "   Uncorrected error\n"),
		IF(status.bits.over,	    "   Error overflow\n"));
	if (status.bits.addrv)
		kdb_printf(
			" IA32_MC%d_ADDR(0x%x): 0x%016qx\n",
			i, IA32_MCi_ADDR(i), bank->mca_mci_addr);
	if (status.bits.miscv) {
		ia32_mc8_misc_t	mc8_misc;

		mc8_misc.u64 = bank->mca_mci_misc;
		kdb_printf(
			" IA32_MC%d_MISC(0x%x): 0x%016qx\n"
			"  DIMM:     %d\n"
			"  Channel:  %d\n"
			"  Syndrome: 0x%x\n",
			i, IA32_MCi_MISC(i), mc8_misc.u64,
			mc8_misc.bits.dimm,
			mc8_misc.bits.channel,
			(int) mc8_misc.bits.syndrome);
	}
}

static const char *mca_threshold_status[] = {
	[THRESHOLD_STATUS_NO_TRACKING]	"No tracking",
	[THRESHOLD_STATUS_GREEN]	"Green",
	[THRESHOLD_STATUS_YELLOW]	"Yellow",
	[THRESHOLD_STATUS_RESERVED]	"Reserved"
};

=======
		infop->cpuid_microcode_version);
	kdb_printf(" signature: 0x%x\n",
		infop->cpuid_signature);
	kdb_printf(" %s\n",
		infop->cpuid_brand_string);

}

>>>>>>> origin/10.9
static void
mca_dump_bank(mca_state_t *state, int i)
{
	mca_mci_bank_t		*bank;
	ia32_mci_status_t	status;

	bank = &state->mca_error_bank[i];
	status = bank->mca_mci_status;
	if (!status.bits.val)
		return;

<<<<<<< HEAD
	kdb_printf(
		"  MCA error code:            0x%04x\n",
		status.bits.mca_error);
	kdb_printf(
		"  Model specific error code: 0x%04x\n",
		status.bits.model_specific_error);
	if (!mca_threshold_status_present) {
		kdb_printf(
			"  Other information:         0x%08x\n",
			status.bits.other_information);
	} else {
		int	threshold = status.bits_tes_p.threshold;
		kdb_printf(
			"  Other information:         0x%08x\n"
		        "  Threshold-based status:    %s\n",
			status.bits_tes_p.other_information,
			(status.bits_tes_p.uc == 0) ?
			    mca_threshold_status[threshold] :
			    "Undefined");
	}
	kdb_printf(
		"  Status bits:\n%s%s%s%s%s%s",
		IF(status.bits.pcc,   "   Processor context corrupt\n"),
		IF(status.bits.addrv, "   ADDR register valid\n"),
		IF(status.bits.miscv, "   MISC register valid\n"),
		IF(status.bits.en,    "   Error enabled\n"),
		IF(status.bits.uc,    "   Uncorrected error\n"),
		IF(status.bits.over,  "   Error overflow\n"));
=======
	kdb_printf(" IA32_MC%d_STATUS(0x%x): 0x%016qx\n",
		i, IA32_MCi_STATUS(i), status.u64);

>>>>>>> origin/10.9
	if (status.bits.addrv)
		kdb_printf(" IA32_MC%d_ADDR(0x%x):   0x%016qx\n",
			i, IA32_MCi_ADDR(i), bank->mca_mci_addr);

	if (status.bits.miscv)
		kdb_printf(" IA32_MC%d_MISC(0x%x):   0x%016qx\n",
			i, IA32_MCi_MISC(i), bank->mca_mci_misc);
}

static void
mca_dump_error_banks(mca_state_t *state)
{
	unsigned int 		i;

<<<<<<< HEAD
	kdb_printf("MCA error-reporting registers:\n");
	for (i = 0; i < mca_error_bank_count; i++ ) {
		if (i == 8) {
			/*
			 * Fatal Memory Error
			 */

			/* Dump MC8 for local package */
			kdb_printf(" Package %d logged:\n",
				   x86_package()->ppkg_num);
			mca_dump_bank_mc8(state, 8);

			/* If there's other packages, report their MC8s */
			x86_pkg_t	*pkg;
			uint64_t	deadline;
			for (pkg = x86_pkgs; pkg != NULL; pkg = pkg->next) {
				if (pkg == x86_package())
					continue;
				deadline = mach_absolute_time() + LockTimeOut;
				while  (pkg->mca_state == NULL &&
					mach_absolute_time() < deadline)
					cpu_pause();
				if (pkg->mca_state) {
					kdb_printf(" Package %d logged:\n",
						   pkg->ppkg_num);
					mca_dump_bank_mc8(pkg->mca_state, 8);
				} else {
					kdb_printf(" Package %d timed out!\n",
						   pkg->ppkg_num);
				}
			}
			continue;
		}
>>>>>>> origin/10.5
=======
	if (!state->mca_is_valid)
		return;

	for (i = 0; i < mca_error_bank_count; i++ ) {
>>>>>>> origin/10.9
		mca_dump_bank(state, i);
	}
}

void
mca_dump(void)
{
<<<<<<< HEAD
	mca_state_t	*mca_state = current_cpu_datap()->cpu_mca_state;
	uint64_t	deadline;
	unsigned int	i = 0;
=======
	ia32_mcg_status_t	status;
	mca_state_t		*mca_state = current_cpu_datap()->cpu_mca_state;
>>>>>>> origin/10.5

	/*
	 * Capture local MCA registers to per-cpu data.
	 */
	mca_save_state(mca_state);
<<<<<<< HEAD
=======

	/*
	 * Serialize in case of multiple simultaneous machine-checks.
	 * Only the first caller is allowed to dump MCA registers,
	 * other threads spin meantime.
	 */
	simple_lock(&mca_lock);
	if (mca_dump_state > CLEAR) {
		simple_unlock(&mca_lock);
		while (mca_dump_state == DUMPING)
			cpu_pause();
		return;
	}
	mca_dump_state = DUMPING;
	simple_unlock(&mca_lock);
>>>>>>> origin/10.5

	/*
	 * Serialize: the first caller controls dumping MCA registers,
	 * other threads spin meantime.
	 */
<<<<<<< HEAD
	simple_lock(&mca_lock);
	if (mca_dump_state > CLEAR) {
		simple_unlock(&mca_lock);
		while (mca_dump_state == DUMPING)
			cpu_pause();
		return;
	}
	mca_dump_state = DUMPING;
	simple_unlock(&mca_lock);

<<<<<<< HEAD
=======
	kdb_printf(
		"Machine-check capabilities (cpu %d) 0x%016qx:\n",
		cpu_number(), ia32_mcg_cap.u64);
=======
	/*
	 * Report machine-check capabilities:
	 */
	kdb_printf("Machine-check capabilities: 0x%016qx\n", ia32_mcg_cap.u64);
>>>>>>> origin/10.9

	mca_report_cpu_info();

	kdb_printf(" %d error-reporting banks\n", mca_error_bank_count);
 
>>>>>>> origin/10.5
	/*
	 * Wait for all other hardware threads to save their state.
	 * Or timeout.
	 */
<<<<<<< HEAD
	deadline = mach_absolute_time() + LockTimeOut;
	while (mach_absolute_time() < deadline && i < real_ncpus) {
		if (!cpu_datap(i)->cpu_mca_state->mca_is_saved) {
			cpu_pause();
			continue;
		}
		i += 1;
	}
=======
	status.u64 = rdmsr64(IA32_MCG_STATUS);
	kdb_printf(
		"Machine-check status 0x%016qx:\n%s%s%s", status.u64,
		IF(status.bits.ripv, " restart IP valid\n"),
		IF(status.bits.eipv, " error IP valid\n"),
		IF(status.bits.mcip, " machine-check in progress\n"));
>>>>>>> origin/10.5

	/*
	 * Report machine-check capabilities:
	 */
<<<<<<< HEAD
	kdb_printf("Machine-check capabilities: 0x%016qx\n", ia32_mcg_cap.u64);

	mca_report_cpu_info();
=======
	mca_dump_error_banks(mca_state);
>>>>>>> origin/10.5

	kdb_printf(" %d error-reporting banks\n", mca_error_bank_count);
 
	/*
	 * Dump all processor state:
	 */
	for (i = 0; i < real_ncpus; i++) {
		mca_state_t		*mcsp = cpu_datap(i)->cpu_mca_state;
		ia32_mcg_status_t	status;

		if (mcsp == NULL ||
		    mcsp->mca_is_saved == FALSE ||
		    mcsp->mca_mcg_status.u64 == 0 ||
		    !mcsp->mca_is_valid) {
			continue;
		}
		status = mcsp->mca_mcg_status;
		kdb_printf("Processor %d: IA32_MCG_STATUS: 0x%016qx\n",
			i, status.u64);
		mca_cpu_dump_error_banks(mcsp);
	}

	/* Update state to release any other threads. */
	mca_dump_state = DUMPED;
<<<<<<< HEAD
}


extern void mca_exception_panic(void);
extern void lapic_trigger_MC(void);
void mca_exception_panic(void)
{
#if DEBUG
	lapic_trigger_MC();
#else
	kprintf("mca_exception_panic() requires DEBUG build\n");
#endif
=======
>>>>>>> origin/10.5
}
