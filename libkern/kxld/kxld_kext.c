/*
 * Copyright (c) 2008, 2013 Apple Inc. All rights reserved.
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
#include <string.h>
#include <mach/vm_param.h>
#include <mach/vm_types.h>
#include <mach/kmod.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>
#include <sys/types.h>

#if KERNEL
    #include <libkern/kernel_mach_header.h>
    #include <libkern/OSKextLib.h>
    #include <libkern/OSKextLibPrivate.h>
    #include <mach/vm_param.h>
    #include <mach-o/fat.h>
#else /* !KERNEL */
    #include <architecture/byte_order.h>
    #include <mach/mach_init.h>
    #include <mach-o/arch.h>
    #include <mach-o/swap.h>
#endif /* KERNEL */

#define DEBUG_ASSERT_COMPONENT_NAME_STRING "kxld"
#include <AssertMacros.h>

#include "kxld_demangle.h"
#include "kxld_dict.h"
#include "kxld_kext.h"
#include "kxld_object.h"
#include "kxld_reloc.h"
#include "kxld_sect.h"
#include "kxld_seg.h"
#include "kxld_symtab.h"
#include "kxld_util.h"
#include "kxld_vtable.h"

struct symtab_command;

struct kxld_kext {
    KXLDObject *kext;
    KXLDObject *interface;
    KXLDArray vtables;
    KXLDDict vtable_index;
    boolean_t vtables_created;
    boolean_t vtable_index_created;
};

/*******************************************************************************
* Prototypes
*******************************************************************************/

static kern_return_t export_symbols_through_interface(
    const KXLDObject *kext, const KXLDObject *interface, 
    KXLDDict *defined_symbols_by_name, 
    KXLDDict *defined_cxx_symbol_by_value,
    KXLDDict *obsolete_symbols_by_name);
static kern_return_t export_symbols(const KXLDObject *kext,
    KXLDDict *defined_symbols_by_name, 
    KXLDDict *defined_cxx_symbols_by_value);

static kern_return_t create_vtables(KXLDKext *kext,
    const KXLDDict *defined_symbols, const KXLDDict *defined_cxx_symbols);
static kern_return_t get_vtable_syms_from_smcp(KXLDKext *kext, 
    const KXLDDict *defined_symbols, KXLDSym *super_metaclass_ptr_sym, 
    KXLDSym **vtable_sym_out, KXLDSym **meta_vtable_sym_out);

static kern_return_t resolve_symbols(KXLDKext *kext, 
    const KXLDDict *defined_symbols, const KXLDDict *obsolete_symbols);

static kern_return_t patch_vtables(KXLDKext *kext, KXLDDict *patched_vtables,
    const KXLDDict *defined_symbols);
static const KXLDSym *get_metaclass_symbol_from_super_meta_class_pointer_symbol(
    KXLDKext *kext, KXLDSym *super_metaclass_pointer_sym);
static kern_return_t create_vtable_index(KXLDKext *kext);

static kern_return_t validate_symbols(KXLDKext *kext);

/*******************************************************************************
*******************************************************************************/
size_t
kxld_kext_sizeof(void)
{
    return sizeof(KXLDKext);
}

/*******************************************************************************
*******************************************************************************/
kern_return_t
kxld_kext_init(KXLDKext *kext, KXLDObject *kext_object, 
    KXLDObject *interface_object)
{
    kern_return_t rval = KERN_FAILURE;

    check(kext);
    check(kext_object);

    kext->kext = kext_object;

    if (interface_object) {
        kext->interface = interface_object;

        rval = kxld_object_index_symbols_by_name(kext->kext);
        require_noerr(rval, finish);
    }
    
    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
void
kxld_kext_clear(KXLDKext *kext)
{
    KXLDVTable *vtable = NULL;
    u_int i;

    check(kext);

    for (i = 0; i < kext->vtables.nitems; ++i) {
        vtable = kxld_array_get_item(&kext->vtables, i);
        kxld_vtable_clear(vtable);
    }
    kxld_array_reset(&kext->vtables);
    kxld_dict_clear(&kext->vtable_index);

    kext->kext = NULL;
    kext->interface = NULL;
    kext->vtables_created = FALSE;
    kext->vtable_index_created = FALSE;
}


/*******************************************************************************
*******************************************************************************/
void 
kxld_kext_deinit(KXLDKext *kext)
{
    KXLDVTable *vtable = NULL;
    u_int i;

    check(kext);

    for (i = 0; i < kext->vtables.maxitems; ++i) {
        vtable = kxld_array_get_slot(&kext->vtables, i);
        kxld_vtable_deinit(vtable);
    }
    kxld_array_deinit(&kext->vtables);
    kxld_dict_deinit(&kext->vtable_index);

    bzero(kext, sizeof(*kext));
}

/*******************************************************************************
*******************************************************************************/
kern_return_t 
kxld_kext_export_symbols(const KXLDKext *kext, 
    KXLDDict *defined_symbols_by_name,
    KXLDDict *obsolete_symbols_by_name,
    KXLDDict *defined_cxx_symbols_by_value)
{
    kern_return_t rval = KERN_FAILURE;

    check(kext);

    if (kext->interface) {
        rval = export_symbols_through_interface(kext->kext, kext->interface, 
            defined_symbols_by_name, obsolete_symbols_by_name,
            defined_cxx_symbols_by_value);
        require_noerr(rval, finish);
    } else {
        rval = export_symbols(kext->kext, defined_symbols_by_name,
            defined_cxx_symbols_by_value);
        require_noerr(rval, finish);
    }

    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
kern_return_t
export_symbols_through_interface(const KXLDObject *kext,
    const KXLDObject *interface, KXLDDict *defined_symbols_by_name,
    KXLDDict *obsolete_symbols_by_name, KXLDDict *defined_cxx_symbols_by_value)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    const KXLDSymtab *kext_symtab = NULL;
    const KXLDSymtab *interface_symtab = NULL;
    KXLDSym *kext_sym = NULL;
    const KXLDSym *interface_sym = NULL;

    check(kext);
    check(interface);

    kext_symtab = kxld_object_get_symtab(kext);
    interface_symtab = kxld_object_get_symtab(interface);

    if (defined_symbols_by_name) {
        /* Add exported symbols */
        (void) kxld_symtab_iterator_init(&iter, interface_symtab, 
            kxld_sym_is_undefined, FALSE);
        while ((interface_sym = kxld_symtab_iterator_get_next(&iter))) {
            kext_sym = kxld_symtab_get_locally_defined_symbol_by_name(kext_symtab, 
                interface_sym->name);
            if (!kext_sym) {
                kxld_log(kKxldLogLinking, kKxldLogWarn,
                    "In interface %s of %s, couldn't find symbol %s\n", 
                    kxld_object_get_name(interface), kxld_object_get_name(kext),
                    interface_sym->name);
                continue;
            }

            rval = kxld_dict_insert(defined_symbols_by_name, 
                kext_sym->name, kext_sym);
            require_noerr(rval, finish);
        }

        /* Add indirect symbols */
        (void) kxld_symtab_iterator_init(&iter, interface_symtab, 
            kxld_sym_is_indirect, FALSE);
        while ((interface_sym = kxld_symtab_iterator_get_next(&iter))) {
            kext_sym = kxld_symtab_get_locally_defined_symbol_by_name(kext_symtab, 
                interface_sym->alias);
            if (!kext_sym) {
                kxld_log(kKxldLogLinking, kKxldLogWarn,
                    "In interface %s of %s, couldn't find indirect symbol %s (%s)\n", 
                    kxld_object_get_name(interface), kxld_object_get_name(kext),
                    interface_sym->alias, interface_sym->name);
                continue;
            }

            rval = kxld_dict_insert(defined_symbols_by_name, 
                interface_sym->name, kext_sym);
            require_noerr(rval, finish);
        }
    }

    /* Add obsolete symbols */
    if (obsolete_symbols_by_name) {
        (void) kxld_symtab_iterator_init(&iter, interface_symtab, 
            kxld_sym_is_obsolete, FALSE);
        while ((kext_sym = kxld_symtab_iterator_get_next(&iter))) {
            rval = kxld_dict_insert(obsolete_symbols_by_name, 
                kext_sym->name, kext_sym);
            require_noerr(rval, finish);
        }
    }

    /* Add C++ symbols */
    if (defined_cxx_symbols_by_value) {
        (void) kxld_symtab_iterator_init(&iter, kext_symtab,
            kxld_sym_is_cxx, FALSE);
        while ((kext_sym = kxld_symtab_iterator_get_next(&iter))) {
            rval = kxld_dict_insert(defined_cxx_symbols_by_value,
                &kext_sym->link_addr, kext_sym);
            require_noerr(rval, finish);
        }
    }

    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
kern_return_t
export_symbols(const KXLDObject *kext, KXLDDict *defined_symbols_by_name,
    KXLDDict *defined_cxx_symbols_by_value)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;

    (void) kxld_symtab_iterator_init(&iter, kxld_object_get_symtab(kext), 
        kxld_sym_is_exported, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        if (defined_symbols_by_name) {
            rval = kxld_dict_insert(defined_symbols_by_name, sym->name, sym);
            require_noerr(rval, finish);
        }

        if (kxld_sym_is_cxx(sym) && defined_cxx_symbols_by_value) {
            rval = kxld_dict_insert(defined_cxx_symbols_by_value,
                &sym->link_addr, sym);
            require_noerr(rval, finish);
        }
    }

    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
kern_return_t 
kxld_kext_export_vtables(KXLDKext *kext, const KXLDDict *defined_cxx_symbols,
    const KXLDDict *defined_symbols, KXLDDict *vtables)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDVTable *vtable = NULL;
    u_int i = 0;

    check(kext);
    check(defined_symbols);
    check(defined_cxx_symbols);
    check(vtables);

    rval = create_vtables(kext, defined_cxx_symbols, defined_symbols);
    require_noerr(rval, finish);

    for (i = 0; i < kext->vtables.nitems; ++i) {
        vtable = kxld_array_get_item(&kext->vtables, i);

        rval = kxld_dict_insert(vtables, vtable->name, vtable);
        require_noerr(rval, finish);
    }
    
    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
void 
kxld_kext_get_vmsize(const KXLDKext *kext, 
    u_long *header_size, u_long *vmsize)
{
    (void) kxld_object_get_vmsize(kext->kext, header_size, vmsize);
}

/*******************************************************************************
 *******************************************************************************/
void 
kxld_kext_set_linked_object_size(KXLDKext *kext, u_long vmsize)
{
    (void) kxld_object_set_linked_object_size(kext->kext, vmsize);
}


/*******************************************************************************
*******************************************************************************/
kern_return_t 
kxld_kext_export_linked_object(const KXLDKext *kext, 
    u_char *linked_object, kxld_addr_t *kmod_info)
{
    kern_return_t rval = KERN_FAILURE;
    const KXLDSym *kmodsym = NULL;

    kmodsym = kxld_symtab_get_locally_defined_symbol_by_name(
        kxld_object_get_symtab(kext->kext), KXLD_KMOD_INFO_SYMBOL);
    require_action(kmodsym, finish, rval=KERN_FAILURE;
        kxld_log(kKxldLogLinking, kKxldLogErr, kKxldLogNoKmodInfo));
 
    *kmod_info = kmodsym->link_addr;

    rval = kxld_object_export_linked_object(kext->kext, linked_object);
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
kern_return_t
kxld_kext_relocate(KXLDKext *kext, kxld_addr_t link_address,
    KXLDDict *patched_vtables, const KXLDDict *defined_symbols, 
    const KXLDDict *obsolete_symbols, const KXLDDict *defined_cxx_symbols)
{
    kern_return_t rval = KERN_FAILURE;

    check(kext);
    check(patched_vtables);
    check(defined_symbols);
    check(obsolete_symbols);

    /* Kexts that are being relocated need symbols indexed by value for vtable
     * creation and patching. Note that we don't need to index by value for
     * dependencies that have already been linked because their symbols are
     * already in the global cxx value table. It's important to index the
     * symbols by value before we relocate the symbols because the vtable
     * entries will still have unrelocated values.
     */
    rval = kxld_object_index_cxx_symbols_by_value(kext->kext);
    require_noerr(rval, finish);

    rval = kxld_object_index_symbols_by_name(kext->kext);
    require_noerr(rval, finish);

    rval = kxld_object_relocate(kext->kext, link_address);
    require_noerr(rval, finish);

    rval = resolve_symbols(kext, defined_symbols, obsolete_symbols);
    require_noerr(rval, finish);

    rval = create_vtables(kext, defined_cxx_symbols, /* defined_symbols */ NULL);
    require_noerr(rval, finish);

    rval = patch_vtables(kext, patched_vtables, defined_symbols);
    require_noerr(rval, finish);
    
    rval = validate_symbols(kext);
    require_noerr(rval, finish);

    rval = kxld_object_process_relocations(kext->kext, patched_vtables);
    require_noerr(rval, finish);

    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
* The defined symbols argument is optional.  When supplied, create_vtables()
* will look for vtable symbols in the defined_symbols dictionary.  Otherwise,
* it will look in the kext's symbol table for vtable symbols.
*
* We do this because there are two types of KXLDKext objects that call
* create_vtables(), those that have been linked, and those that haven't.  The
* linked kexts export their symbols into the global symbol table that is used
* for symbol resolution, so we can look there for vtable symbols without
* having to index their local symbol table separately.
* 
* Unlinked kexts haven't yet had their symbols exported into the global table,
* so we have to index their local symbol table separately.
*******************************************************************************/
static kern_return_t 
create_vtables(KXLDKext *kext, const KXLDDict *defined_cxx_symbols,
    const KXLDDict *defined_symbols)
{
    kern_return_t rval = KERN_FAILURE;
    const KXLDSymtab *symtab = NULL;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    KXLDSym *vtable_sym = NULL;
    KXLDSym *meta_vtable_sym = NULL;
    KXLDVTable *vtable = NULL;
    KXLDVTable *meta_vtable = NULL;
    u_int i = 0;
    u_int nvtables = 0;

    if (kext->vtables_created) {
        rval = KERN_SUCCESS;
        goto finish;
    }

    symtab = kxld_object_get_symtab(kext->kext);

    if (kxld_object_is_linked(kext->kext)) {
        /* Create a vtable object for every vtable symbol */
        kxld_symtab_iterator_init(&iter, symtab, kxld_sym_is_vtable, FALSE);
        nvtables = kxld_symtab_iterator_get_num_remaining(&iter);
    } else {
        /* We walk over the super metaclass pointer symbols because classes
         * with them are the only ones that need patching.  Then we double the
         * number of vtables we're expecting, because every pointer will have a
         * class vtable and a MetaClass vtable.
         */
        kxld_symtab_iterator_init(&iter, symtab, 
            kxld_sym_is_super_metaclass_pointer, FALSE);
        nvtables = kxld_symtab_iterator_get_num_remaining(&iter) * 2;
    }

    rval = kxld_array_init(&kext->vtables, sizeof(KXLDVTable), nvtables);
    require_noerr(rval, finish);

    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        if (kxld_object_is_linked(kext->kext)) {
            vtable_sym = sym;
            meta_vtable_sym = NULL;
            meta_vtable = NULL;
        } else {
            rval = get_vtable_syms_from_smcp(kext, defined_symbols, sym,
                &vtable_sym, &meta_vtable_sym);
            require_noerr(rval, finish);
        }

        vtable = kxld_array_get_item(&kext->vtables, i++);
        rval = kxld_vtable_init(vtable, vtable_sym, kext->kext, 
            defined_cxx_symbols);
        require_noerr(rval, finish);

        /* meta_vtable_sym will be null when we don't support strict
         * patching and can't find the metaclass vtable. If that's the
         * case, we just reduce the expect number of vtables by 1.
         */
        if (!kxld_object_is_linked(kext->kext)) {
            if (meta_vtable_sym) {
                meta_vtable = kxld_array_get_item(&kext->vtables, i++);
                rval = kxld_vtable_init(meta_vtable, meta_vtable_sym, 
                    kext->kext, defined_cxx_symbols);
                require_noerr(rval, finish);
            } else {
                kxld_array_resize(&kext->vtables, --nvtables);
                meta_vtable = NULL;
            }
        }
    }
    require_action(i == kext->vtables.nitems, finish, 
        rval=KERN_FAILURE);

    kext->vtables_created = TRUE;
    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
get_vtable_syms_from_smcp(KXLDKext *kext, const KXLDDict *defined_symbols,
    KXLDSym *super_metaclass_ptr_sym, KXLDSym **vtable_sym_out, 
    KXLDSym **meta_vtable_sym_out)
{
    kern_return_t rval = KERN_FAILURE;
    const KXLDSymtab *symtab = NULL;
    KXLDSym *vtable_sym = NULL;
    KXLDSym *meta_vtable_sym = NULL;
    char class_name[KXLD_MAX_NAME_LEN];
    char vtable_name[KXLD_MAX_NAME_LEN];
    char meta_vtable_name[KXLD_MAX_NAME_LEN];
    char *demangled_name1 = NULL;
    char *demangled_name2 = NULL;
    size_t demangled_length1 = 0;
    size_t demangled_length2 = 0;

    check(kext);
    check(vtable_sym_out);
    check(meta_vtable_sym_out);

    require(!kxld_object_is_kernel(kext->kext), finish);

    symtab = kxld_object_get_symtab(kext->kext);

    /* Get the class name from the smc pointer */
    rval = kxld_sym_get_class_name_from_super_metaclass_pointer(
        super_metaclass_ptr_sym, class_name, sizeof(class_name));
    require_noerr(rval, finish);

    /* Get the vtable name from the class name */
    rval = kxld_sym_get_vtable_name_from_class_name(class_name,
        vtable_name, sizeof(vtable_name));
    require_noerr(rval, finish);

    /* Get the vtable symbol */
    if (defined_symbols) {
        vtable_sym = kxld_dict_find(defined_symbols, vtable_name);
    } else {
        vtable_sym = kxld_symtab_get_locally_defined_symbol_by_name(symtab, 
            vtable_name);
    }
    require_action(vtable_sym, finish, rval=KERN_FAILURE;
        kxld_log(kKxldLogPatching, kKxldLogErr, kKxldLogMissingVtable,
        vtable_name, class_name));

    /* Get the meta vtable name from the class name */
    rval = kxld_sym_get_meta_vtable_name_from_class_name(class_name,
        meta_vtable_name, sizeof(meta_vtable_name));
    require_noerr(rval, finish);

    /* Get the meta vtable symbol */
    if (defined_symbols) {
        meta_vtable_sym = kxld_dict_find(defined_symbols, meta_vtable_name);
    } else {
        meta_vtable_sym = kxld_symtab_get_locally_defined_symbol_by_name(symtab,
            meta_vtable_name);
    }
    if (!meta_vtable_sym) {
        if (kxld_object_target_supports_strict_patching(kext->kext)) {
            kxld_log(kKxldLogPatching, kKxldLogErr, 
                kKxldLogMissingVtable, 
                meta_vtable_name, class_name);
            rval = KERN_FAILURE;
            goto finish;
        } else {
            kxld_log(kKxldLogPatching, kKxldLogErr, 
                "Warning: " kKxldLogMissingVtable, 
                kxld_demangle(meta_vtable_name, &demangled_name1, 
                    &demangled_length1), 
                kxld_demangle(class_name, &demangled_name2, 
                    &demangled_length2));
        }
    }
    
    *vtable_sym_out = vtable_sym;
    *meta_vtable_sym_out = meta_vtable_sym;
    rval = KERN_SUCCESS;
finish:
    if (demangled_name1) kxld_free(demangled_name1, demangled_length1);
    if (demangled_name2) kxld_free(demangled_name2, demangled_length2);

    return rval;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
resolve_symbols(KXLDKext *kext, const KXLDDict *defined_symbols, 
    const KXLDDict *obsolete_symbols)
{
    kern_return_t rval = KERN_FAILURE;
    const KXLDSymtab *symtab = NULL;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    KXLDSym *defined_sym = NULL;
    const char *name = NULL;
    boolean_t tests_for_weak = FALSE;
    boolean_t error = FALSE;
    char *demangled_name = NULL;
    size_t demangled_length = 0;

    check(kext->kext);
    check(defined_symbols);
    check(obsolete_symbols);

    symtab = kxld_object_get_symtab(kext->kext);

    /* Check if the kext tests for weak symbols */
    sym = kxld_symtab_get_symbol_by_name(symtab, KXLD_WEAK_TEST_SYMBOL);
    tests_for_weak = (sym != NULL);

    /* Check for duplicate symbols */
    kxld_symtab_iterator_init(&iter, symtab, kxld_sym_is_exported, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        defined_sym = kxld_dict_find(defined_symbols, sym->name);
        if (defined_sym) { 
            /* Not a problem if the symbols have the same address */
            if (defined_sym->link_addr == sym->link_addr) {
                continue;
            }

            if (!error) {
                error = TRUE;
                kxld_log(kKxldLogLinking, kKxldLogErr,
                    "The following symbols were defined more than once:");
            }

            kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s: %p - %p", 
                kxld_demangle(sym->name, &demangled_name, &demangled_length),
                (void *) (uintptr_t) sym->link_addr, 
                (void *) (uintptr_t) defined_sym->link_addr);
        }
    }
    require_noerr_action(error, finish, rval=KERN_FAILURE);

    /* Resolve undefined and indirect symbols */

    /* Iterate over all unresolved symbols */
    kxld_symtab_iterator_init(&iter, symtab, 
        kxld_sym_is_unresolved, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {

        /* Common symbols are not supported */
        if (kxld_sym_is_common(sym)) {

            if (!error) {
                error = TRUE;
                if (kxld_object_target_supports_common_symbols(kext->kext)) {
                    kxld_log(kKxldLogLinking, kKxldLogErr, 
                        "The following common symbols were not resolved:");
                } else {
                    kxld_log(kKxldLogLinking, kKxldLogErr, 
                        "Common symbols are not supported in kernel extensions. " 
                         "Use -fno-common to build your kext. "
                         "The following are common symbols:");
                }
            }
            kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s", 
                kxld_demangle(sym->name, &demangled_name, &demangled_length));

        } else {

            /* Find the address of the defined symbol */
            if (kxld_sym_is_undefined(sym)) {
                name = sym->name;
            } else {
                name = sym->alias;
            }
            defined_sym = kxld_dict_find(defined_symbols, name);
            
            /* Resolve the symbol.  If a definition cannot be found, then:
             * 1) Psuedokexts log a warning and proceed
             * 2) Actual kexts delay the error until validation in case vtable
             *    patching replaces the undefined symbol.
             */

            if (defined_sym) {

                rval = kxld_sym_resolve(sym, defined_sym->link_addr);
                require_noerr(rval, finish);

                if (obsolete_symbols && kxld_dict_find(obsolete_symbols, name)) {
                    kxld_log(kKxldLogLinking, kKxldLogWarn, 
                        "This kext uses obsolete symbol %s.", 
                        kxld_demangle(name, &demangled_name, &demangled_length));
                }

            } else if (kxld_sym_is_weak(sym)) {
                kxld_addr_t addr = 0;

                /* Make sure that the kext has referenced gOSKextUnresolved.
                 */
                require_action(tests_for_weak, finish, 
                   rval=KERN_FAILURE;
                   kxld_log(kKxldLogLinking, kKxldLogErr, 
                      "This kext has weak references but does not test for "
                      "them. Test for weak references with "
                      "OSKextSymbolIsResolved(). (found in <libkern/OSKextLib.h>)"));

#if KERNEL
                /* Get the address of the default weak address.
                 */
                addr = (kxld_addr_t) &kext_weak_symbol_referenced;
#else  
                /* This is run during symbol generation only, so we only 
                 * need a filler value here.
                 */
                addr = 0xF00DD00D;
#endif /* KERNEL */

                rval = kxld_sym_resolve(sym, addr);
                require_noerr(rval, finish);
            }
        }
    }
    require_noerr_action(error, finish, rval=KERN_FAILURE);

    rval = KERN_SUCCESS;

finish:
    if (demangled_name) kxld_free(demangled_name, demangled_length);

    return rval;
}

/*******************************************************************************
* We must patch vtables to ensure binary compatibility, and to perform that
* patching, we have to determine the vtables' inheritance relationships.  The
* MetaClass system gives us a way to do that:
*   1) Iterate over all of the super MetaClass pointer symbols.  Every class
*      that inherits from OSObject will have a pointer in its MetaClass that
*      points to the MetaClass's super MetaClass.
*   2) Derive the name of the class from the super MetaClass pointer.
*   3) Derive the name of the class's vtable from the name of the class
*   4) Follow the super MetaClass pointer to get the address of the super
*      MetaClass's symbol
*   5) Look up the super MetaClass symbol by address
*   6) Derive the super class's name from the super MetaClass name
*   7) Derive the super class's vtable from the super class's name
* This procedure will allow us to find all of the OSObject-derived classes and
* their super classes, and thus patch all of the vtables.
*
* We also have to take care to patch up the MetaClass's vtables.  The
* MetaClasses follow a parallel hierarchy to the classes, so once we have the
* class name and super class name, we can also derive the MetaClass name and
* the super MetaClass name, and thus find and patch their vtables as well.
*******************************************************************************/

#define kOSMetaClassVTableName "__ZTV11OSMetaClass"

static kern_return_t
patch_vtables(KXLDKext *kext, KXLDDict *patched_vtables, 
    const KXLDDict *defined_symbols)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    const KXLDSymtab *symtab = NULL;
    const KXLDSym *metaclass = NULL;
    KXLDSym *super_metaclass_pointer = NULL;
    KXLDSym *final_sym = NULL;
    KXLDVTable *vtable = NULL;
    KXLDVTable *super_vtable = NULL;
    char class_name[KXLD_MAX_NAME_LEN];
    char super_class_name[KXLD_MAX_NAME_LEN];
    char vtable_name[KXLD_MAX_NAME_LEN];
<<<<<<< HEAD
    char super_vtable_name[KXLD_MAX_NAME_LEN];
    char final_sym_name[KXLD_MAX_NAME_LEN];
    char *demangled_name1 = NULL;
    char *demangled_name2 = NULL;
    size_t demangled_length1 = 0;;
    size_t demangled_length2 = 0;
    size_t len = 0;
=======
    char meta_vtable_name[KXLD_MAX_NAME_LEN];
    char *demangled_name1 = NULL;
    char *demangled_name2 = NULL;
    size_t demangled_length1 = 0;
    size_t demangled_length2 = 0;
    u_int i = 0;
>>>>>>> origin/10.6
    u_int nvtables = 0;
    u_int npatched = 0;
    u_int nprogress = 0;
    boolean_t failure = FALSE;

    check(kext);
    check(patched_vtables);

    symtab = kxld_object_get_symtab(kext->kext);

    rval = create_vtable_index(kext);
    require_noerr(rval, finish);

    /* Find each super meta class pointer symbol */

    kxld_symtab_iterator_init(&iter, symtab, 
        kxld_sym_is_super_metaclass_pointer, FALSE);
    nvtables = kxld_symtab_iterator_get_num_remaining(&iter);

    while (npatched < nvtables) {
        npatched = 0;
        nprogress = 0;
        kxld_symtab_iterator_reset(&iter);
        while((super_metaclass_pointer = kxld_symtab_iterator_get_next(&iter))) 
        {
            /* Get the class name from the smc pointer */
            rval = kxld_sym_get_class_name_from_super_metaclass_pointer(
                super_metaclass_pointer, class_name, sizeof(class_name));
            require_noerr(rval, finish);

            /* Get the vtable name from the class name */
            rval = kxld_sym_get_vtable_name_from_class_name(class_name,
                vtable_name, sizeof(vtable_name));
            require_noerr(rval, finish);

            /* Get the vtable and make sure it hasn't been patched */
            vtable = kxld_dict_find(&kext->vtable_index, vtable_name);
            require_action(vtable, finish, rval=KERN_FAILURE;
                kxld_log(kKxldLogPatching, kKxldLogErr, kKxldLogMissingVtable,
<<<<<<< HEAD
                    vtable_name, class_name));
=======
                vtable_name, class_name));

            /* Get the meta vtable name from the class name */
            rval = kxld_sym_get_meta_vtable_name_from_class_name(class_name,
                meta_vtable_name, sizeof(meta_vtable_name));
            require_noerr(rval, finish);

            /* Get the meta vtable symbol */
            meta_vtable_sym = kxld_symtab_get_symbol_by_name(kext->symtab,
                meta_vtable_name);
            if (!meta_vtable_sym) {
                /* If we don't support strict patching and we can't find the vtable,
                 * log a warning and reduce the expected number of vtables by 1.
                 */
                if (target_supports_strict_patching(kext)) {
                    kxld_log(kKxldLogPatching, kKxldLogErr, kKxldLogMissingVtable, 
                        meta_vtable_name, class_name);
                    rval = KERN_FAILURE;
                    goto finish;
                } else {
                    kxld_log(kKxldLogPatching, kKxldLogErr, 
                        "Warning: " kKxldLogMissingVtable, 
                        kxld_demangle(meta_vtable_name, &demangled_name1, 
                            &demangled_length1), 
                        kxld_demangle(class_name, &demangled_name2, 
                            &demangled_length2));
                    kxld_array_resize(&kext->vtables, --nvtables);
                }
            }
        }
>>>>>>> origin/10.6

            if (!vtable->is_patched) {

                /* Find the SMCP's meta class symbol */
                metaclass = get_metaclass_symbol_from_super_meta_class_pointer_symbol(
                    kext, super_metaclass_pointer);
                require_action(metaclass, finish, rval=KERN_FAILURE);

<<<<<<< HEAD
=======
        if (kext->link_type == KXLD_LINK_KERNEL) {
            /* Initialize the kernel vtable */
            rval = kxld_vtable_init_from_kernel_macho(vtable, vtable_sym, 
                vtable_sect, kext->symtab, &kext->relocator);
            require_noerr(rval, finish);
        } else {
            /* Initialize the class vtable */
            if (kext->is_final_image) {
                rval = kxld_vtable_init_from_final_macho(vtable, vtable_sym, 
                    vtable_sect, kext->symtab, &kext->relocator, &kext->extrelocs);
                require_noerr(rval, finish);
            } else {
                rval = kxld_vtable_init_from_object_macho(vtable, vtable_sym, 
                    vtable_sect, kext->symtab, &kext->relocator);
                require_noerr(rval, finish);
            }

            /* meta_vtable_sym will be null when we don't support strict patching
             * and can't find the metaclass vtable.
             */
            if (meta_vtable_sym) {
                /* Get the vtable's section */
                meta_vtable_sect = kxld_array_get_item(&kext->sects, 
                    meta_vtable_sym->sectnum);
                require_action(vtable_sect, finish, rval=KERN_FAILURE);
               
                meta_vtable = kxld_array_get_item(&kext->vtables, i++);
                
                /* Initialize the metaclass vtable */
                if (kext->is_final_image) {
                    rval = kxld_vtable_init_from_final_macho(meta_vtable, meta_vtable_sym, 
                        meta_vtable_sect, kext->symtab, &kext->relocator, &kext->extrelocs);
                    require_noerr(rval, finish);
                } else {
                    rval = kxld_vtable_init_from_object_macho(meta_vtable, meta_vtable_sym, 
                        meta_vtable_sect, kext->symtab, &kext->relocator);
                    require_noerr(rval, finish);
                }
            }
        }
    }
    require_action(i == kext->vtables.nitems, finish, 
        rval=KERN_FAILURE);

    /* Map vtable names to the vtable structures */
    rval = kxld_dict_init(&kext->vtable_index, kxld_dict_string_hash, 
        kxld_dict_string_cmp, kext->vtables.nitems);
    require_noerr(rval, finish);

    for (i = 0; i < kext->vtables.nitems; ++i) {
        vtable = kxld_array_get_item(&kext->vtables, i);
        rval = kxld_dict_insert(&kext->vtable_index, vtable->name, vtable);
        require_noerr(rval, finish);
    }

    rval = KERN_SUCCESS;

finish:

    if (demangled_name1) kxld_free(demangled_name1, demangled_length1);
    if (demangled_name2) kxld_free(demangled_name2, demangled_length2);

    return rval;
}

/*******************************************************************************
* Temporary workaround for PR-6668105 
* new, new[], delete, and delete[] may be overridden globally in a kext.
* We should do this with some sort of weak symbols, but we'll use a whitelist 
* for now to minimize risk.  
*******************************************************************************/
static void
restrict_private_symbols(KXLDKext *kext)
{
    const char *private_symbols[] = {
        KXLD_KMOD_INFO_SYMBOL,
        KXLD_OPERATOR_NEW_SYMBOL,
        KXLD_OPERATOR_NEW_ARRAY_SYMBOL,
        KXLD_OPERATOR_DELETE_SYMBOL,
        KXLD_OPERATOR_DELETE_ARRAY_SYMBOL
    };
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    const char *name = NULL;
    u_int i = 0;

    kxld_symtab_iterator_init(&iter, kext->symtab, kxld_sym_is_exported, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        for (i = 0; i < const_array_len(private_symbols); ++i) {
            name = private_symbols[i];
            if (!streq(sym->name, name)) {
                continue;
            }

            kxld_sym_mark_private(sym);
        }
    }
}

/*******************************************************************************
*******************************************************************************/
void
kxld_kext_clear(KXLDKext *kext)
{
    KXLDSeg *seg = NULL;
    KXLDSect *sect = NULL;
    KXLDVTable *vtable = NULL;
    u_int i;

    check(kext);

#if !KERNEL
    if (kext->link_type == KXLD_LINK_KERNEL) {
        unswap_macho(kext->file, kext->host_order, kext->target_order);
    }
#endif /* !KERNEL */

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);
        kxld_seg_clear(seg);
    }
    kxld_array_reset(&kext->segs);

    for (i = 0; i < kext->sects.nitems; ++i) {
        sect = kxld_array_get_item(&kext->sects, i);
        kxld_sect_clear(sect);
    }
    kxld_array_reset(&kext->sects);

    for (i = 0; i < kext->vtables.nitems; ++i) {
        vtable = kxld_array_get_item(&kext->vtables, i);
        kxld_vtable_clear(vtable);
    }
    kxld_array_reset(&kext->vtables);

    kxld_array_reset(&kext->extrelocs);
    kxld_array_reset(&kext->locrelocs);
    kxld_dict_clear(&kext->vtable_index);
    kxld_relocator_clear(&kext->relocator);
    kxld_uuid_clear(&kext->uuid);

    if (kext->symtab) kxld_symtab_clear(kext->symtab);

    kext->link_addr = 0;
    kext->kmod_link_addr = 0;
    kext->cputype = 0;
    kext->cpusubtype = 0;
    kext->link_type = KXLD_LINK_UNKNOWN;
    kext->is_final_image = FALSE;
    kext->got_is_created = FALSE;
}



/*******************************************************************************
*******************************************************************************/
void 
kxld_kext_deinit(KXLDKext *kext)
{
    KXLDSeg *seg = NULL;
    KXLDSect *sect = NULL;
    KXLDVTable *vtable = NULL;
    u_int i;

    check(kext);

#if !KERNEL
    if (kext->link_type == KXLD_LINK_KERNEL) {
        unswap_macho(kext->file, kext->host_order, kext->target_order);
    }
#endif /* !KERNEL */

    for (i = 0; i < kext->segs.maxitems; ++i) {
        seg = kxld_array_get_slot(&kext->segs, i);
        kxld_seg_deinit(seg);
    }
    kxld_array_deinit(&kext->segs);

    for (i = 0; i < kext->sects.maxitems; ++i) {
        sect = kxld_array_get_slot(&kext->sects, i);
        kxld_sect_deinit(sect);
    }
    kxld_array_deinit(&kext->sects);

    for (i = 0; i < kext->vtables.maxitems; ++i) {
        vtable = kxld_array_get_slot(&kext->vtables, i);
        kxld_vtable_deinit(vtable);
    }
    kxld_array_deinit(&kext->vtables);

    kxld_array_deinit(&kext->extrelocs);
    kxld_array_deinit(&kext->locrelocs);
    kxld_dict_deinit(&kext->vtable_index);

    if (kext->symtab) {
        kxld_symtab_deinit(kext->symtab);
        kxld_free(kext->symtab, kxld_symtab_sizeof());
    }

    bzero(kext, sizeof(*kext));
}

/*******************************************************************************
*******************************************************************************/
boolean_t
kxld_kext_is_true_kext(const KXLDKext *kext)
{
    return (kext->link_type == KXLD_LINK_KEXT);
}

/*******************************************************************************
*******************************************************************************/
void
kxld_kext_get_vmsize(const KXLDKext *kext, u_long *header_size, u_long *vmsize)
{
    check(kext);
    check(header_size);
    check(vmsize);
    *header_size = 0;
    *vmsize = 0;

    /* vmsize is the padded header page(s) + segment vmsizes */

    *header_size = (kext->is_final_image) ?
        0 : round_page(get_macho_header_size(kext));
    *vmsize = *header_size + get_macho_data_size(kext);

}

/*******************************************************************************
*******************************************************************************/
const struct kxld_symtab * 
kxld_kext_get_symtab(const KXLDKext *kext)
{
    check(kext);

    return kext->symtab;
}

/*******************************************************************************
*******************************************************************************/
u_int
kxld_kext_get_num_symbols(const KXLDKext *kext)
{
    check(kext);

    return kxld_symtab_get_num_symbols(kext->symtab);
}

/*******************************************************************************
*******************************************************************************/
void 
kxld_kext_get_vtables(KXLDKext *kext, const KXLDArray **vtables)
{
    check(kext);
    check(vtables);

    *vtables = &kext->vtables;
}

/*******************************************************************************
*******************************************************************************/
u_int
kxld_kext_get_num_vtables(const KXLDKext *kext)
{
    check(kext);

    return kext->vtables.nitems;
}

/*******************************************************************************
*******************************************************************************/
KXLDSeg *
kxld_kext_get_seg_by_name(const KXLDKext *kext, const char *segname)
{
    KXLDSeg *seg = NULL;
    u_int i = 0;

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);

        if (streq(segname, seg->segname)) break;

        seg = NULL;
    }

    return seg;
}

/*******************************************************************************
*******************************************************************************/
KXLDSect *
kxld_kext_get_sect_by_name(const KXLDKext *kext, const char *segname, 
    const char *sectname)
{
    KXLDSect *sect = NULL;
    u_int i = 0;

    for (i = 0; i < kext->sects.nitems; ++i) {
        sect = kxld_array_get_item(&kext->sects, i);

        if (streq(segname, sect->segname) && streq(sectname, sect->sectname)) {
            break;
        }

        sect = NULL;
    }

    return sect;
}

/*******************************************************************************
*******************************************************************************/
int
kxld_kext_get_sectnum_for_sect(const KXLDKext *kext, const KXLDSect *sect)
{
    kern_return_t rval = KERN_FAILURE;
    u_int idx = -1;

    rval = kxld_array_get_index(&kext->sects, sect, &idx);
    if (rval) idx = -1;

    return idx;
}

/*******************************************************************************
*******************************************************************************/
const KXLDArray * 
kxld_kext_get_section_order(const KXLDKext *kext __unused)
{
#if KXLD_USER_OR_OBJECT
    if (kext->link_type == KXLD_LINK_KERNEL && target_supports_object(kext)) {
        return kext->section_order;
    }
#endif /* KXLD_USER_OR_OBJECT */

    return NULL;
}

/*******************************************************************************
*******************************************************************************/
static u_long
get_macho_header_size(const KXLDKext *kext)
{
    KXLDSeg *seg = NULL;
    u_long header_size = 0;
    u_int i = 0;

    check(kext);

    /* Mach, segment, and UUID headers */

    if (kxld_kext_is_32_bit(kext)) {
        header_size += sizeof(struct mach_header);
    } else {
        header_size += sizeof(struct mach_header_64);
    }

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);
        header_size += kxld_seg_get_macho_header_size(seg, kxld_kext_is_32_bit(kext));
    }

    if (kext->uuid.has_uuid) {
        header_size += kxld_uuid_get_macho_header_size();
    }

    return header_size;
}

/*******************************************************************************
*******************************************************************************/
static u_long
get_macho_data_size(const KXLDKext *kext)
{
    KXLDSeg *seg = NULL;
    u_long data_size = 0;
    u_int i = 0;

    check(kext);

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);
        data_size += (u_long) kxld_seg_get_vmsize(seg);
    }

    return data_size;
}

/*******************************************************************************
*******************************************************************************/
kern_return_t kxld_kext_export_linked_object(const KXLDKext *kext,
    u_char *linked_object, kxld_addr_t *kmod_info_kern)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSeg *seg = NULL;
    u_long size = 0;
    u_long header_size = 0;
    u_long header_offset = 0;
    u_long data_offset = 0;
    u_int ncmds = 0;
    u_int i = 0;

    check(kext);
    check(linked_object);
    check(kmod_info_kern);
    *kmod_info_kern = 0;

    /* Calculate the size of the headers and data */

    header_size = get_macho_header_size(kext);
    data_offset = (kext->is_final_image) ? header_size : round_page(header_size);
    size = data_offset + get_macho_data_size(kext);

    /* Copy data to the file */

    ncmds = kext->segs.nitems + (kext->uuid.has_uuid == TRUE);

    rval = export_macho_header(kext, linked_object, ncmds, 
        &header_offset, header_size);
    require_noerr(rval, finish);

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);

        rval = kxld_seg_export_macho_to_vm(seg, linked_object, &header_offset, 
            header_size, size, kext->link_addr, kxld_kext_is_32_bit(kext));
        require_noerr(rval, finish);
    }

    if (kext->uuid.has_uuid) {
        rval = kxld_uuid_export_macho(&kext->uuid, linked_object, 
            &header_offset, header_size);
        require_noerr(rval, finish);
    }

    *kmod_info_kern = kext->kmod_link_addr;

#if !KERNEL
    unswap_macho(linked_object, kext->host_order, kext->target_order);
#endif /* KERNEL */

    rval = KERN_SUCCESS;

finish:
    return rval;
}

#if !KERNEL
/*******************************************************************************
*******************************************************************************/
kern_return_t
kxld_kext_export_symbol_file(const KXLDKext *kext, 
    u_char **_symbol_file, u_long *_filesize)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSeg *seg = NULL;
    u_char *file = NULL;
    u_long size = 0;
    u_long header_size = 0;
    u_long header_offset = 0;
    u_long data_offset = 0;
    u_int ncmds = 0;
    u_int i = 0;

    check(kext);
    check(_symbol_file);
    *_symbol_file = NULL;

    /* Calculate the size of the file */

    if (kxld_kext_is_32_bit(kext)) {
        header_size += sizeof(struct mach_header);
    } else {
        header_size += sizeof(struct mach_header_64);
    }

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);
        header_size += kxld_seg_get_macho_header_size(seg, kxld_kext_is_32_bit(kext));
        size += kxld_seg_get_macho_data_size(seg);
    }

    header_size += kxld_symtab_get_macho_header_size();
    size += kxld_symtab_get_macho_data_size(kext->symtab, FALSE, 
        kxld_kext_is_32_bit(kext));

    if (kext->uuid.has_uuid) {
        header_size += kxld_uuid_get_macho_header_size();
    }

    data_offset = round_page(header_size);
    size += data_offset;

    /* Allocate the symbol file */

    file = kxld_page_alloc_untracked(size);
    require_action(file, finish, rval=KERN_RESOURCE_SHORTAGE);
    bzero(file, size);

    /* Copy data to the file */

    ncmds = kext->segs.nitems + (kext->uuid.has_uuid == TRUE) + 1; /* +1 for symtab */
    rval = export_macho_header(kext, file, ncmds, &header_offset, header_size);
    require_noerr(rval, finish);

    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);
        rval = kxld_seg_export_macho_to_file_buffer(seg, file, &header_offset, 
            header_size, &data_offset, size, kxld_kext_is_32_bit(kext));
        require_noerr(rval, finish);
    }

    rval = kxld_symtab_export_macho(kext->symtab, file, &header_offset,
        header_size, &data_offset, size, FALSE, kxld_kext_is_32_bit(kext));
    require_noerr(rval, finish);

    if (kext->uuid.has_uuid) {
        rval = kxld_uuid_export_macho(&kext->uuid, file, &header_offset, 
            header_size);
        require_noerr(rval, finish);
    }

    header_offset = header_size;

    /* Commit */

    unswap_macho(file, kext->host_order, kext->target_order);

    *_filesize = size;
    *_symbol_file = file;
    file = NULL;
    rval = KERN_SUCCESS;

finish:

    if (file) {
        kxld_page_free_untracked(file, size);
        file = NULL;
    }

    check(!file);
    check((!rval) ^ (!*_symbol_file));

    return rval;
}
#endif

/*******************************************************************************
*******************************************************************************/
boolean_t 
kxld_kext_target_needs_swap(const KXLDKext *kext __unused)
{
#if KERNEL
    return FALSE;
#else
    return (kext->target_order != kext->host_order);
#endif /* KERNEL */
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
export_macho_header(const KXLDKext *kext, u_char *buf, u_int ncmds,
    u_long *header_offset, u_long header_size)
{
    kern_return_t rval = KERN_FAILURE;

    check(kext);
    check(buf);
    check(header_offset);

    KXLD_3264_FUNC(kxld_kext_is_32_bit(kext), rval, 
        export_macho_header_32, export_macho_header_64, 
        kext, buf, ncmds, header_offset, header_size);
    require_noerr(rval, finish);

    rval = KERN_SUCCESS;

finish:
    return rval;
}

#if KXLD_USER_OR_ILP32
/*******************************************************************************
*******************************************************************************/
static kern_return_t
export_macho_header_32(const KXLDKext *kext, u_char *buf, u_int ncmds,
    u_long *header_offset, u_long header_size)
{
    kern_return_t rval = KERN_FAILURE;
    struct mach_header *mach = NULL;

    check(kext);
    check(buf);
    check(header_offset);

    require_action(sizeof(*mach) <= header_size - *header_offset, finish,
        rval=KERN_FAILURE);
    mach = (struct mach_header *) (buf + *header_offset);

    mach->magic = MH_MAGIC;
    mach->cputype = kext->cputype;
    mach->filetype = kext->filetype;
    mach->ncmds = ncmds;
    mach->sizeofcmds = (uint32_t) (header_size - sizeof(*mach));
    mach->flags = MH_NOUNDEFS;

    *header_offset += sizeof(*mach);

    rval = KERN_SUCCESS;

finish:
    return rval;
}
#endif /* KXLD_USER_OR_ILP32 */

#if KXLD_USER_OR_LP64
/*******************************************************************************
*******************************************************************************/
static kern_return_t
export_macho_header_64(const KXLDKext *kext, u_char *buf, u_int ncmds,
    u_long *header_offset, u_long header_size)
{
    kern_return_t rval = KERN_FAILURE;
    struct mach_header_64 *mach = NULL;

    check(kext);
    check(buf);
    check(header_offset);
    
    require_action(sizeof(*mach) <= header_size - *header_offset, finish,
        rval=KERN_FAILURE);
    mach = (struct mach_header_64 *) (buf + *header_offset);
    
    mach->magic = MH_MAGIC_64;
    mach->cputype = kext->cputype;
    mach->cpusubtype = kext->cpusubtype;
    mach->filetype = kext->filetype;
    mach->ncmds = ncmds;
    mach->sizeofcmds = (uint32_t) (header_size - sizeof(*mach));
    mach->flags = MH_NOUNDEFS;

    *header_offset += sizeof(*mach);

    rval = KERN_SUCCESS;

finish:
    return rval;
}
#endif /* KXLD_USER_OR_LP64 */

/*******************************************************************************
*******************************************************************************/
kern_return_t 
kxld_kext_resolve(KXLDKext *kext, struct kxld_dict *patched_vtables,
    struct kxld_dict *defined_symbols)
{
    kern_return_t rval = KERN_FAILURE;

    require_action(kext->link_type == KXLD_LINK_PSEUDO_KEXT, finish,
        rval=KERN_FAILURE);

    /* Resolve symbols */
    rval = resolve_symbols(kext, defined_symbols, NULL);
    require_noerr(rval, finish);

    /* Validate symbols */
    rval = validate_symbols(kext);
    require_noerr(rval, finish);

    /* Pseudokexts re-export their dependencies' vtables */
    rval = copy_vtables(kext, patched_vtables);
    require_noerr(rval, finish);

    rval = KERN_SUCCESS;

finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
kern_return_t
kxld_kext_relocate(KXLDKext *kext, kxld_addr_t link_address,
    KXLDDict *patched_vtables, KXLDDict *defined_symbols, 
    KXLDDict *obsolete_symbols)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSeg *seg = NULL;
    u_int i = 0;

    check(kext);
    check(patched_vtables);
    check(defined_symbols);

    require_action(kext->link_type == KXLD_LINK_KEXT, finish, rval=KERN_FAILURE);

    kext->link_addr = link_address;

    /* Relocate segments (which relocates the sections) */
    for (i = 0; i < kext->segs.nitems; ++i) {
        seg = kxld_array_get_item(&kext->segs, i);
        kxld_seg_relocate(seg, link_address);
    }

    /* Relocate symbols */
    rval = kxld_symtab_relocate(kext->symtab, &kext->sects);
    require_noerr(rval, finish);
 
    /* Populate kmod info structure */
    rval = populate_kmod_info(kext);
    require_noerr(rval, finish);
   
    /* Resolve symbols */
    rval = resolve_symbols(kext, defined_symbols, obsolete_symbols);
    require_noerr(rval, finish);
  
    /* Patch vtables */
    rval = patch_vtables(kext, patched_vtables, defined_symbols);
    require_noerr(rval, finish);
    
    /* Validate symbols */
    rval = validate_symbols(kext);
    require_noerr(rval, finish);

    /* Process relocation entries and populate the global offset table.
     *
     * For final linked images: the relocation entries are contained in a couple
     * of tables hanging off the end of the symbol table.  The GOT has its own
     * section created by the linker; we simply need to fill it.
     *
     * For object files: the relocation entries are bound to each section.
     * The GOT, if it exists for the target architecture, is created by kxld,
     * and we must populate it according to our internal structures.
     */
    if (kext->is_final_image) {
#if KXLD_USER_OR_BUNDLE
        rval = process_symbol_pointers(kext);
        require_noerr(rval, finish);

        rval = process_relocs_from_tables(kext);
        require_noerr(rval, finish);
#else
        require_action(FALSE, finish, rval=KERN_FAILURE);
#endif /* KXLD_USER_OR_BUNDLE */
    } else {
#if KXLD_USER_OR_GOT
        /* Populate GOT */
        rval = populate_got(kext);
        require_noerr(rval, finish);
#endif /* KXLD_USER_OR_GOT */
#if KXLD_USER_OR_OBJECT
        rval = process_relocs_from_sections(kext);
        require_noerr(rval, finish);
#else
        require_action(FALSE, finish, rval=KERN_FAILURE);
#endif /* KXLD_USER_OR_OBJECT */
    }

    rval = KERN_SUCCESS;

finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
resolve_symbols(KXLDKext *kext, KXLDDict *defined_symbols, 
    KXLDDict *obsolete_symbols)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    void *addrp = NULL;
    kxld_addr_t addr = 0;
    const char *name = NULL;
    boolean_t tests_for_weak = FALSE;
    boolean_t error = FALSE;
    boolean_t warning = FALSE;
    char *demangled_name = NULL;
    size_t demangled_length = 0;

    check(kext);
    check(defined_symbols);

    /* Check if the kext tests for weak symbols */
    sym = kxld_symtab_get_symbol_by_name(kext->symtab, KXLD_WEAK_TEST_SYMBOL);
    tests_for_weak = (sym != NULL);

    /* Check for duplicate symbols */
    kxld_symtab_iterator_init(&iter, kext->symtab, kxld_sym_is_exported, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        addrp = kxld_dict_find(defined_symbols, sym->name);
        if (addrp) { 
            /* Convert to a kxld_addr_t */
            if (kxld_kext_is_32_bit(kext)) {
                addr = (kxld_addr_t) (*(uint32_t*)addrp);
            } else {
                addr = (kxld_addr_t) (*(uint64_t*)addrp);
            }

            /* Not a problem if the symbols have the same address */
            if (addr == sym->link_addr) {
                continue;
            }

            if (!error) {
                error = TRUE;
                kxld_log(kKxldLogLinking, kKxldLogErr,
                    "The following symbols were defined more than once:");
            }

            kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s: %p - %p", 
                kxld_demangle(sym->name, &demangled_name, &demangled_length),
                (void *) (uintptr_t) sym->link_addr, 
                (void *) (uintptr_t) addr);
        }
    }
    require_noerr_action(error, finish, rval=KERN_FAILURE);

    /* Resolve undefined and indirect symbols */

    /* Iterate over all unresolved symbols */
    kxld_symtab_iterator_init(&iter, kext->symtab, 
        kxld_sym_is_unresolved, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {

        /* Common symbols are not supported */
        if (kxld_sym_is_common(sym)) {

            if (!error) {
                error = TRUE;
                if (target_supports_common(kext)) {
                    kxld_log(kKxldLogLinking, kKxldLogErr, 
                        "The following common symbols were not resolved:");
                } else {
                    kxld_log(kKxldLogLinking, kKxldLogErr, 
                        "Common symbols are not supported in kernel extensions. " 
                         "Use -fno-common to build your kext. "
                         "The following are common symbols:");
                }
            }
            kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s", 
                kxld_demangle(sym->name, &demangled_name, &demangled_length));

        } else {

            /* Find the address of the defined symbol */
            if (kxld_sym_is_undefined(sym)) {
                name = sym->name;
            } else {
                name = sym->alias;
            }
            addrp = kxld_dict_find(defined_symbols, name);
            
            /* Resolve the symbol.  If a definition cannot be found, then:
             * 1) Psuedokexts log a warning and proceed
             * 2) Actual kexts delay the error until validation in case vtable
             *    patching replaces the undefined symbol.
             */

            if (addrp) {

                /* Convert to a kxld_addr_t */
                if (kxld_kext_is_32_bit(kext)) {
                    addr = (kxld_addr_t) (*(uint32_t*)addrp);
                } else {
                    addr = (kxld_addr_t) (*(uint64_t*)addrp);
                }

                boolean_t is_exported = (kext->link_type == KXLD_LINK_PSEUDO_KEXT);

                rval = kxld_sym_resolve(sym, addr, is_exported);
                require_noerr(rval, finish);

                if (obsolete_symbols && kxld_dict_find(obsolete_symbols, name)) {
                    kxld_log(kKxldLogLinking, kKxldLogWarn, 
                        "This kext uses obsolete symbol %s.", 
                        kxld_demangle(name, &demangled_name, &demangled_length));
                }

            } else if (kext->link_type == KXLD_LINK_PSEUDO_KEXT) {
                /* Pseudokexts ignore undefined symbols, because any actual
                 * kexts that need those symbols will fail to link anyway, so
                 * there's no need to block well-behaved kexts.
                 */
                if (!warning) {
                    kxld_log(kKxldLogLinking, kKxldLogWarn, 
                        "This symbol set has the following unresolved symbols:");
                    warning = TRUE;
                }
                kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s", 
                    kxld_demangle(sym->name, &demangled_name, &demangled_length));
                kxld_sym_delete(sym);

            } else if (kxld_sym_is_weak(sym)) {
                /* Make sure that the kext has referenced gOSKextUnresolved.
                 */
                require_action(tests_for_weak, finish, 
                   rval=KERN_FAILURE;
                   kxld_log(kKxldLogLinking, kKxldLogErr, 
                      "This kext has weak references but does not test for "
                      "them. Test for weak references with "
                      "OSKextIsSymbolResolved()."));

#if KERNEL
                /* Get the address of the default weak address.
                 */
                addr = (kxld_addr_t) &kext_weak_symbol_referenced;
#else  
                /* This is run during symbol generation only, so we only 
                 * need a filler value here.
                 */
                addr = kext->link_addr;
#endif /* KERNEL */

                rval = kxld_sym_resolve(sym, addr, /* exported */ FALSE);
                require_noerr(rval, finish);
            }
        }
    }
    require_noerr_action(error, finish, rval=KERN_FAILURE);

    rval = KERN_SUCCESS;

finish:
    if (demangled_name) kxld_free(demangled_name, demangled_length);

    return rval;
}

/*******************************************************************************
*******************************************************************************/
static boolean_t
target_supports_strict_patching(KXLDKext *kext)
{
    check(kext);

    return (kext->cputype != CPU_TYPE_I386 && 
            kext->cputype != CPU_TYPE_POWERPC);
}

/*******************************************************************************
* We must patch vtables to ensure binary compatibility, and to perform that
* patching, we have to determine the vtables' inheritance relationships.  The
* MetaClass system gives us a way to do that:
*   1) Iterate over all of the super MetaClass pointer symbols.  Every class
*      that inherits from OSObject will have a pointer in its MetaClass that
*      points to the MetaClass's super MetaClass.
*   2) Derive the name of the class from the super MetaClass pointer.
*   3) Derive the name of the class's vtable from the name of the class
*   4) Follow the super MetaClass pointer to get the address of the super
*      MetaClass's symbol
*   5) Look up the super MetaClass symbol by address
*   6) Derive the super class's name from the super MetaClass name
*   7) Derive the super class's vtable from the super class's name
* This procedure will allow us to find all of the OSObject-derived classes and
* their super classes, and thus patch all of the vtables.
*
* We also have to take care to patch up the MetaClass's vtables.  The
* MetaClasses follow a parallel hierarchy to the classes, so once we have the
* class name and super class name, we can also derive the MetaClass name and
* the super MetaClass name, and thus find and patch their vtables as well.
*******************************************************************************/

#define kOSMetaClassVTableName "__ZTV11OSMetaClass"

static kern_return_t
patch_vtables(KXLDKext *kext, KXLDDict *patched_vtables, 
    KXLDDict *defined_symbols)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    KXLDSym *metaclass = NULL;
    KXLDSym *super_metaclass_pointer = NULL;
    KXLDSym *final_sym = NULL;
    KXLDVTable *vtable = NULL;
    KXLDVTable *super_vtable = NULL;
    char class_name[KXLD_MAX_NAME_LEN];
    char super_class_name[KXLD_MAX_NAME_LEN];
    char vtable_name[KXLD_MAX_NAME_LEN];
    char super_vtable_name[KXLD_MAX_NAME_LEN];
    char final_sym_name[KXLD_MAX_NAME_LEN];
    char *demangled_name1 = NULL;
    char *demangled_name2 = NULL;
    size_t demangled_length1 = 0;;
    size_t demangled_length2 = 0;
    size_t len = 0;
    u_int nvtables = 0;
    u_int npatched = 0;
    u_int nprogress = 0;
    boolean_t failure = FALSE;

    check(kext);
    check(patched_vtables);

    /* Find each super meta class pointer symbol */

    kxld_symtab_iterator_init(&iter, kext->symtab, 
        kxld_sym_is_super_metaclass_pointer, FALSE);
    nvtables = kxld_symtab_iterator_get_num_remaining(&iter);

    while (npatched < nvtables) {
        npatched = 0;
        nprogress = 0;
        kxld_symtab_iterator_reset(&iter);
        while((super_metaclass_pointer = kxld_symtab_iterator_get_next(&iter))) 
        {
            /* Get the class name from the smc pointer */
            rval = kxld_sym_get_class_name_from_super_metaclass_pointer(
                super_metaclass_pointer, class_name, sizeof(class_name));
            require_noerr(rval, finish);

            /* Get the vtable name from the class name */
            rval = kxld_sym_get_vtable_name_from_class_name(class_name,
                vtable_name, sizeof(vtable_name));
            require_noerr(rval, finish);

            /* Get the vtable and make sure it hasn't been patched */
            vtable = kxld_dict_find(&kext->vtable_index, vtable_name);
            require_action(vtable, finish, rval=KERN_FAILURE;
                kxld_log(kKxldLogPatching, kKxldLogErr, kKxldLogMissingVtable,
                    vtable_name, class_name));

            if (!vtable->is_patched) {

                /* Find the SMCP's meta class symbol */
                rval = get_metaclass_symbol_from_super_meta_class_pointer_symbol(
                    kext, super_metaclass_pointer, &metaclass);
                require_noerr(rval, finish);

>>>>>>> origin/10.6
                /* Get the super class name from the super metaclass */
                rval = kxld_sym_get_class_name_from_metaclass(metaclass,
                    super_class_name, sizeof(super_class_name));
                require_noerr(rval, finish);
<<<<<<< HEAD
=======

                /* Get the super vtable name from the class name */
                rval = kxld_sym_get_vtable_name_from_class_name(super_class_name,
                    super_vtable_name, sizeof(super_vtable_name));
                require_noerr(rval, finish);

                if (failure) {
                    kxld_log(kKxldLogPatching, kKxldLogErr, 
                        "\t'%s' (super vtable '%s')", 
                        kxld_demangle(vtable_name, &demangled_name1, 
                            &demangled_length1), 
                        kxld_demangle(super_vtable_name, &demangled_name2, 
                            &demangled_length2));
                    continue;
                }

                /* Get the super vtable if it's been patched */
                super_vtable = kxld_dict_find(patched_vtables, super_vtable_name);
                if (!super_vtable) continue;

                /* Get the final symbol's name from the super vtable */
                rval = kxld_sym_get_final_sym_name_from_class_name(super_class_name, 
                    final_sym_name, sizeof(final_sym_name));
                require_noerr(rval, finish);

                /* Verify that the final symbol does not exist.  First check
                 * all the externally defined symbols, then check locally.
                 */
                final_sym = kxld_dict_find(defined_symbols, final_sym_name);
                if (!final_sym) {
                    final_sym = kxld_symtab_get_symbol_by_name(kext->symtab, 
                        final_sym_name);
                }
                require_action(!final_sym, finish, 
                    rval=KERN_FAILURE;
                    kxld_log(kKxldLogPatching, kKxldLogErr, 
                        "Class '%s' is a subclass of final class '%s'.",
                        kxld_demangle(class_name, &demangled_name1, 
                            &demangled_length1), 
                        kxld_demangle(super_class_name, &demangled_name2, 
                            &demangled_length2)));

                /* Patch the class's vtable */
                rval = kxld_vtable_patch(vtable, super_vtable, kext->symtab,
                    target_supports_strict_patching(kext));
                require_noerr(rval, finish);

                /* Add the class's vtable to the set of patched vtables */
                rval = kxld_dict_insert(patched_vtables, vtable->name, vtable);
                require_noerr(rval, finish);

                /* Get the meta vtable name from the class name */
                rval = kxld_sym_get_meta_vtable_name_from_class_name(class_name,
                    vtable_name, sizeof(vtable_name));
                require_noerr(rval, finish);

                /* Get the meta vtable.  Whether or not it should exist has already
                 * been tested in create_vtables(), so if it doesn't exist and we're
                 * still running, we can safely skip it.
                 */
                vtable = kxld_dict_find(&kext->vtable_index, vtable_name);
                if (!vtable) {
                    ++nprogress;
                    ++npatched;
                    continue;
                }
                require_action(!vtable->is_patched, finish, rval=KERN_FAILURE);

                /* There is no way to look up a metaclass vtable at runtime, but
                 * we know that every class's metaclass inherits directly from 
                 * OSMetaClass, so we just hardcode that vtable name here.
                 */
                len = strlcpy(super_vtable_name, kOSMetaClassVTableName,
                    sizeof(super_vtable_name));
                require_action(len == const_strlen(kOSMetaClassVTableName),
                    finish, rval=KERN_FAILURE);
                       
                /* Get the super meta vtable */
                super_vtable = kxld_dict_find(patched_vtables, super_vtable_name);
                require_action(super_vtable && super_vtable->is_patched, 
                    finish, rval=KERN_FAILURE);

                /* Patch the meta class's vtable */
                rval = kxld_vtable_patch(vtable, super_vtable,
                    kext->symtab, target_supports_strict_patching(kext));
                require_noerr(rval, finish);

                /* Add the MetaClass's vtable to the set of patched vtables */
                rval = kxld_dict_insert(patched_vtables, vtable->name, vtable);
                require_noerr(rval, finish);
                
                ++nprogress;
            }

            ++npatched;
        }

        require_action(!failure, finish, rval=KERN_FAILURE);
        if (!nprogress) {
            failure = TRUE;
            kxld_log(kKxldLogPatching, kKxldLogErr, 
                "The following vtables were unpatchable because each one's " 
                "parent vtable either was not found or also was not patchable:");
        }
    }

    rval = KERN_SUCCESS;
finish:
    if (demangled_name1) kxld_free(demangled_name1, demangled_length1);
    if (demangled_name2) kxld_free(demangled_name2, demangled_length2);

    return rval;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
validate_symbols(KXLDKext *kext)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    u_int error = FALSE;
    char *demangled_name = NULL;
    size_t demangled_length = 0;
    
    /* Check for any unresolved symbols */
    kxld_symtab_iterator_init(&iter, kext->symtab, kxld_sym_is_unresolved, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        if (!error) {
            error = TRUE;
            kxld_log(kKxldLogLinking, kKxldLogErr, 
                "The following symbols are unresolved for this kext:");
        }
        kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s", 
            kxld_demangle(sym->name, &demangled_name, &demangled_length));
    }
    require_noerr_action(error, finish, rval=KERN_FAILURE);

    rval = KERN_SUCCESS;

finish:
    if (demangled_name) kxld_free(demangled_name, demangled_length);
    return rval;
}

#if KXLD_USER_OR_GOT || KXLD_USER_OR_COMMON
/*******************************************************************************
*******************************************************************************/
static kern_return_t
add_section(KXLDKext *kext, KXLDSect **sect)
{
    kern_return_t rval = KERN_FAILURE;
    u_int nsects = kext->sects.nitems;

    rval = kxld_array_resize(&kext->sects, nsects + 1);
    require_noerr(rval, finish);

    *sect = kxld_array_get_item(&kext->sects, nsects);

    rval = KERN_SUCCESS;

finish:
    return rval;
}
#endif /* KXLD_USER_OR_GOT || KXLD_USER_OR_COMMON */

#if KXLD_USER_OR_GOT
/*******************************************************************************
*******************************************************************************/
static boolean_t
target_has_got(const KXLDKext *kext)
{
    return FALSE:
}

/*******************************************************************************
* Create and initialize the Global Offset Table
*******************************************************************************/
static kern_return_t
create_got(KXLDKext *kext)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSect *sect = NULL;
    u_int ngots = 0;
    u_int i = 0;

    if (!target_has_got(kext)) {
        rval = KERN_SUCCESS;
        goto finish;
    }

    for (i = 0; i < kext->sects.nitems; ++i) {
        sect = kxld_array_get_item(&kext->sects, i);
        ngots += kxld_sect_get_ngots(sect, &kext->relocator, 
            kext->symtab);
    }

    rval = add_section(kext, &sect);
    require_noerr(rval, finish);

    rval = kxld_sect_init_got(sect, ngots);
    require_noerr(rval, finish);

    kext->got_is_created = TRUE;
    rval = KERN_SUCCESS;

finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
populate_got(KXLDKext *kext)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSect *sect = NULL;
    u_int i = 0;

    if (!target_has_got(kext) || !kext->got_is_created) {
        rval = KERN_SUCCESS;
        goto finish;
    }

    for (i = 0; i < kext->sects.nitems; ++i) {
        sect = kxld_array_get_item(&kext->sects, i);
        if (streq_safe(sect->segname, KXLD_SEG_GOT, sizeof(KXLD_SEG_GOT)) &&
            streq_safe(sect->sectname, KXLD_SECT_GOT, sizeof(KXLD_SECT_GOT)))
        {
            kxld_sect_populate_got(sect, kext->symtab,
                kxld_kext_target_needs_swap(kext));
            break;
        }
    }

    require_action(i < kext->sects.nitems, finish, rval=KXLD_MISSING_GOT);

    rval = KERN_SUCCESS;

finish:
    return rval;
}
#endif /* KXLD_USER_OR_GOT */

/*******************************************************************************
*******************************************************************************/
static boolean_t
target_supports_common(const KXLDKext *kext)
{
    check(kext);
    return (kext->cputype == CPU_TYPE_I386 || 
            kext->cputype == CPU_TYPE_POWERPC);
}

#if KXLD_USER_OR_COMMON
/*******************************************************************************
* If there are common symbols, calculate how much space they'll need
* and create/grow the __DATA __common section to accommodate them.
* Then, resolve them against that section.
*******************************************************************************/
static kern_return_t
resolve_common_symbols(KXLDKext *kext)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    KXLDSect *sect = NULL;
    kxld_addr_t base_addr = 0;
    kxld_size_t size = 0;
    kxld_size_t total_size = 0;
    u_int align = 0;
    u_int max_align = 0;
    u_int sectnum = 0;

    if (!target_supports_common(kext)) {
        rval = KERN_SUCCESS;
        goto finish;
    }

    /* Iterate over the common symbols to calculate their total aligned size */
    kxld_symtab_iterator_init(&iter, kext->symtab, kxld_sym_is_common, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        align = kxld_sym_get_common_align(sym);
        size = kxld_sym_get_common_size(sym);

        if (align > max_align) max_align = align;

        total_size = kxld_align_address(total_size, align) + size;
    }
>>>>>>> origin/10.6

                /* Get the super vtable name from the class name */
                rval = kxld_sym_get_vtable_name_from_class_name(super_class_name,
                    super_vtable_name, sizeof(super_vtable_name));
                require_noerr(rval, finish);

                /* Get the super vtable if it's been patched */
                super_vtable = kxld_dict_find(patched_vtables, super_vtable_name);

                if (failure) {
                    const KXLDVTable *unpatched_super_vtable;
                    unpatched_super_vtable = kxld_dict_find(&kext->vtable_index,
                        super_vtable_name);

                    /* If the parent's vtable hasn't been patched, warn that
                     * this vtable is unpatchable because of the parent.
                     */
                    if (!super_vtable) {
                        kxld_log(kKxldLogPatching, kKxldLogErr, 
                            "The %s was not patched because its parent, "
                            "the %s, was not %s.",
                            kxld_demangle(vtable_name, &demangled_name1, 
                                &demangled_length1), 
                            kxld_demangle(super_vtable_name, &demangled_name2, 
                                &demangled_length2),
                            (unpatched_super_vtable) ? "patchable" : "found");
                    }
                    continue;
                }

                if (!super_vtable) continue;

                /* Get the final symbol's name from the super vtable */
                rval = kxld_sym_get_final_sym_name_from_class_name(super_class_name, 
                    final_sym_name, sizeof(final_sym_name));
                require_noerr(rval, finish);

                /* Verify that the final symbol does not exist.  First check
                 * all the externally defined symbols, then check locally.
                 */
                final_sym = kxld_dict_find(defined_symbols, final_sym_name);
                if (!final_sym) {
                    final_sym = kxld_symtab_get_locally_defined_symbol_by_name(
                        symtab, final_sym_name);
                }
                if (final_sym) {
                    kxld_log(kKxldLogPatching, kKxldLogErr, 
                        "Class '%s' is a subclass of final class '%s'.",
                        kxld_demangle(class_name, &demangled_name1, 
                            &demangled_length1), 
                        kxld_demangle(super_class_name, &demangled_name2, 
                            &demangled_length2));
                    continue;
                }

                /* Patch the class's vtable */
                rval = kxld_vtable_patch(vtable, super_vtable, kext->kext);
                if (rval) continue;

                /* Add the class's vtable to the set of patched vtables */
                rval = kxld_dict_insert(patched_vtables, vtable->name, vtable);
                require_noerr(rval, finish);

                /* Get the meta vtable name from the class name */
                rval = kxld_sym_get_meta_vtable_name_from_class_name(class_name,
                    vtable_name, sizeof(vtable_name));
                require_noerr(rval, finish);

                /* Get the meta vtable.  Whether or not it should exist has already
                 * been tested in create_vtables(), so if it doesn't exist and we're
                 * still running, we can safely skip it.
                 */
                vtable = kxld_dict_find(&kext->vtable_index, vtable_name);
                if (!vtable) {
                    ++nprogress;
                    ++npatched;
                    continue;
                }
                require_action(!vtable->is_patched, finish, rval=KERN_FAILURE);

                /* There is no way to look up a metaclass vtable at runtime, but
                 * we know that every class's metaclass inherits directly from 
                 * OSMetaClass, so we just hardcode that vtable name here.
                 */
                len = strlcpy(super_vtable_name, kOSMetaClassVTableName,
                    sizeof(super_vtable_name));
                require_action(len == const_strlen(kOSMetaClassVTableName),
                    finish, rval=KERN_FAILURE);
                       
                /* Get the super meta vtable */
                super_vtable = kxld_dict_find(patched_vtables, super_vtable_name);
                require_action(super_vtable && super_vtable->is_patched, 
                    finish, rval=KERN_FAILURE);

                /* Patch the meta class's vtable */
                rval = kxld_vtable_patch(vtable, super_vtable, kext->kext);
                require_noerr(rval, finish);

                /* Add the MetaClass's vtable to the set of patched vtables */
                rval = kxld_dict_insert(patched_vtables, vtable->name, vtable);
                require_noerr(rval, finish);
                
                ++nprogress;
            }

            ++npatched;
        }

        require_action(!failure, finish, rval=KERN_FAILURE);
        failure = (nprogress == 0);
    }

    rval = KERN_SUCCESS;
finish:
    if (demangled_name1) kxld_free(demangled_name1, demangled_length1);
    if (demangled_name2) kxld_free(demangled_name2, demangled_length2);

    return rval;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
create_vtable_index(KXLDKext *kext)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDVTable *vtable = NULL;
    u_int i = 0;

    if (kext->vtable_index_created) {
        rval = KERN_SUCCESS;
        goto finish;
    }

    /* Map vtable names to the vtable structures */
    rval = kxld_dict_init(&kext->vtable_index, kxld_dict_string_hash, 
        kxld_dict_string_cmp, kext->vtables.nitems);
    require_noerr(rval, finish);

    for (i = 0; i < kext->vtables.nitems; ++i) {
        vtable = kxld_array_get_item(&kext->vtables, i);
        rval = kxld_dict_insert(&kext->vtable_index, vtable->name, vtable);
        require_noerr(rval, finish);
    }

    kext->vtable_index_created = TRUE;
    rval = KERN_SUCCESS;
finish:
    return rval;
}

/*******************************************************************************
*******************************************************************************/
static const KXLDSym *
get_metaclass_symbol_from_super_meta_class_pointer_symbol(KXLDKext *kext,
    KXLDSym *super_metaclass_pointer_sym)
{
    kern_return_t rval = KERN_FAILURE;
    const KXLDReloc *reloc = NULL;
    const KXLDSect *sect = NULL;
    const KXLDSym *metaclass = NULL;
    
    check(kext);
    check(super_metaclass_pointer_sym);

    /* Get the relocation entry that fills in the super metaclass pointer. */
    reloc = kxld_object_get_reloc_at_symbol(kext->kext,
        super_metaclass_pointer_sym);
    require_action(reloc, finish, rval=KERN_FAILURE);

    /* Get the section of the super metaclass pointer. */
    sect = kxld_object_get_section_by_index(kext->kext,
        super_metaclass_pointer_sym->sectnum);
    require_action(sect, finish, rval=KERN_FAILURE);

    /* Get the symbol that will be filled into the super metaclass pointer. */
    metaclass = kxld_object_get_symbol_of_reloc(kext->kext, reloc, sect);
finish:
    return metaclass;
}

/*******************************************************************************
*******************************************************************************/
static kern_return_t
validate_symbols(KXLDKext *kext)
{
    kern_return_t rval = KERN_FAILURE;
    KXLDSymtabIterator iter;
    KXLDSym *sym = NULL;
    u_int error = FALSE;
    char *demangled_name = NULL;
    size_t demangled_length = 0;
    
    /* Check for any unresolved symbols */
    kxld_symtab_iterator_init(&iter, kxld_object_get_symtab(kext->kext), 
        kxld_sym_is_unresolved, FALSE);
    while ((sym = kxld_symtab_iterator_get_next(&iter))) {
        if (!error) {
            error = TRUE;
            kxld_log(kKxldLogLinking, kKxldLogErr, 
                "The following symbols are unresolved for this kext:");
        }
        kxld_log(kKxldLogLinking, kKxldLogErr, "\t%s", 
            kxld_demangle(sym->name, &demangled_name, &demangled_length));
    }
    require_noerr_action(error, finish, rval=KERN_FAILURE);

    rval = KERN_SUCCESS;

finish:
    if (demangled_name) kxld_free(demangled_name, demangled_length);
    return rval;
}

