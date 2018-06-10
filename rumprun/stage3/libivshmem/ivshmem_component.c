#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$Comso: ivshmem_component.c,v 0.1 2018/06/01$");

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/stat.h>

#include <rump-sys/kern.h>
#include <rump-sys/dev.h>
#include <rump-sys/vfs.h>

#include "ioconf.c"

RUMP_COMPONENT(RUMP_COMPONENT_DEV)
{
	config_init_component(cfdriver_ioconf_ivshmem,
			cfattach_ioconf_ivshmem, cfdata_ioconf_ivshmem);
}

RUMP_COMPONENT(RUMP_COMPONENT_POSTINIT)
{
	extern const struct cdevsw ivshmem_cdevsw;
	devmajor_t bmaj = -1, cmaj = -1;
	int error;
	cmaj = cdevsw_lookup_major(&ivshmem_cdevsw);

	if ((error = devsw_attach("ivshmem", NULL, &bmaj,
					&ivshmem_cdevsw, &cmaj)) != 0)
		panic("cannot attach ivshmem: %d", error);

	if ((error = rump_vfs_makeonedevnode(S_IFCHR, "/dev/ivshmem",
	    cmaj, 0)) != 0)
		panic("cannot create /dev/ivshmem: %d", error);

	//rump_pdev_add(ivshmem_attach, 4);
}


