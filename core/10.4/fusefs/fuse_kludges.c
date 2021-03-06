/*
 * Copyright (C) 2006-2008 Google. All Rights Reserved.
 * Amit Singh <singh@>
 */

#include "fuse_kludges.h"

#if M_MACFUSE_ENABLE_EXCHANGE

extern void
fuse_kludge_exchange(vnode_t v1, vnode_t v2)
{
    char *tmp_v_name = ((struct fuse_kludge_vnode *)v1)->v_name;
    ((struct fuse_kludge_vnode *)v1)->v_name =
        ((struct fuse_kludge_vnode *)v2)->v_name;
    ((struct fuse_kludge_vnode *)v2)->v_name = tmp_v_name;

    vnode_t tmp_v_parent = ((struct fuse_kludge_vnode *)v1)->v_parent;
    ((struct fuse_kludge_vnode *)v1)->v_parent =
        ((struct fuse_kludge_vnode *)v2)->v_parent;
    ((struct fuse_kludge_vnode *)v2)->v_parent = tmp_v_parent;
}

#endif /* M_MACFUSE_ENABLE_EXCHANGE */

#define MNT_KERN_FLAG_OFFSET 64

#define MNTK_LOCK_LOCAL 0x00100000
#define MNTK_UNMOUNT    0x01000000

void
vfs_setlocklocal(mount_t mp)
{
    /*
     * Horrible, horrible kludge. Dangerous to boot. "Boot", heh.
     * The issue is that we really need to do vfs_setlocklocal(mp),
     * otherwise we won't have VFS-provided advisory file locking.
     * Since the kernel's extended attributes needs to create files
     * with O_EXLOCK set, we need advisory locking for extended
     * attributes to work properly. Since ACLs depend on extended
     * attributes, vfs_setlocklocal(mp) keeps becoming critical.
     *
     * The kludge is, well, just setting the flag "by hand". This
     * means I'm hardcoding the offset of the flag word field in the
     * mount structure, which is internal to the kernel (not exposed
     * through the KPIs).
     *
     * If the field offset changes in future (not likely, but not
     * guaranteed to remain the same either), the following operation
     * could result in weird behavior, including a kernel panic.
     *
     */
    *(int *)((char *)mp + MNT_KERN_FLAG_OFFSET) |= MNTK_LOCK_LOCAL;
}
