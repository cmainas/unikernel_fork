#include <sys/proc.h>
#include <sys/conf.h>
#include <sys/param.h>		/* defines used in kernel.h */
#include <sys/module.h>
#include <sys/systm.h>
#include <sys/atomic.h>
#include <sys/device.h>
#include <sys/kmem.h>
#include <sys/errno.h>
#include <sys/kernel.h>		/* types used in module initialization */
#include <sys/conf.h>		/* cdevsw struct */
#include <sys/uio.h>		/* uio struct */
#include <sys/malloc.h>
#include <sys/bus.h>		/* structs, prototypes for pci bus stuff and DEVMETHOD macros! */

#include <dev/pci/pcidevs.h>
#include <dev/pci/pcivar.h>	/* For pci_get macros! */
#include <dev/pci/pcireg.h>

/* The softc holds our per-instance data. */
struct ivshmem_softc {
	device_t		dev;
	pci_chipset_tag_t       pc;
	pcitag_t                tag;
	bus_dma_tag_t           dma_tag;
	/* (mmio) control registers i.e. the "register memory region" */
	bus_space_tag_t         reg_tag;
	bus_space_handle_t      reg_handle;
	bus_size_t              reg_size;
        /* data mmio region */
	bus_space_tag_t         data_tag;
	bus_space_handle_t      data_handle;
	bus_size_t              data_size;
        /* irq handling */
	pci_intr_handle_t       *ihp;
	unsigned int		irq;
};

static int ivshmem_match(device_t dev, cfdata_t cf, void *v);
static void ivshmem_attach(device_t parent, device_t self, void *);
static int ivshmem_detach(device_t dev, int flags);

CFATTACH_DECL_NEW(ivshmem, sizeof(struct ivshmem_softc), ivshmem_match, 
		ivshmem_attach, ivshmem_detach, NULL);

/* PCI Support Functions */

/*
 * Compare the device ID of this device against the IDs that this driver
 * supports.  If there is a match, set the description and return success.
 */
static int ivshmem_match(device_t dev, cfdata_t cf, void *v)
{
	struct pci_attach_args *pa;
	//printf("IVSHMEM: Hello form ivshmem_match\n");
	pa = (struct pci_attach_args *) v;
	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_QUMRANET) {
		printf("IVSHMEM: Found match\n");
		if(PCI_PRODUCT(pa->pa_id) == 0x1110)
			return 1;
	}
	return 0;
}

/* Attach function is only called if the probe is successful. */

static void ivshmem_attach(device_t parent, device_t self, void *v)
{
	struct pci_attach_args *pa = (struct pci_attach_args *) v;
	//int revision = PCI_REVISION(pa->pa_class);
	struct ivshmem_softc *sc = device_private(self);
	//pci_intr_handle_t ih;
	bus_space_tag_t iot;
	bus_space_handle_t ioh;
	bus_size_t iosize; 
	//char intrbuf[PCI_INTRSTR_LEN];
	pci_chipset_tag_t pc = pa->pa_pc;
	pcitag_t tag = pa->pa_tag;
	pcireg_t csr;
	//pcireg_t id = pci_conf_read(pc, tag, PCI_SUBSYS_ID_REG);
	
	pci_aprint_devinfo(pa, NULL);
	csr = pci_conf_read(pc, tag, PCI_COMMAND_STATUS_REG);
	printf("csr: %08x\n", csr);
	if ((csr & PCI_COMMAND_MEM_ENABLE) == 0) {
		aprint_error_dev(self, "memory access is disabled\n");
		return;
	}
	sc->dev = self;
	sc->pc = pc;
	sc->tag = tag;
	//if (pci_mapreg_map(pa, PCI_BAR(0), PCI_MAPREG_TYPE_MEM, 0, &iot, 
	//			&ioh, NULL, &iosize)) {
	//	aprint_error_dev(self, "can't map reg\n");
	//	return;
	//}
	//sc->reg_tag = iot;
	//sc->reg_handle = ioh;
	//sc->reg_size = iosize;
	/* data region */
	if (pci_mapreg_map(pa, PCI_BAR(2), PCI_MAPREG_TYPE_MEM, 0, &iot, 
				&ioh, NULL, &iosize)) {
		aprint_error_dev(self, "can't map data\n");
		return;
	}
	sc->data_tag = iot;
	sc->data_handle = ioh;
	sc->data_size = iosize;
	printf("IVSHMEM: tag %ld\n", iot);
	printf("IVSHMEM: handle %ld\n", ioh);
	printf("IVSHMEM: size %ld\n", iosize);
	bus_space_write_4(sc->data_tag, sc->data_handle, 0, 77);
	int tmp = bus_space_read_4(sc->data_tag, sc->data_handle, 0);
	printf("IVSHMEM: read %d\n", tmp);
	/* interrupts */
	return;
}

/* Detach device. */

static int ivshmem_detach(device_t dev, int flags)
{
	struct ivshmem_softc *sc = device_private(dev);
	printf("IVSHMEM: Hello from ivshmem_detach\n");
	/* Teardown the state in our softc created in our attach routine. */
	//if (sc->reg_size) {
	//	bus_space_unmap(sc->reg_tag, sc->reg_handle, sc->reg_size);
	//	sc->reg_size = 0;
	//}
	if (sc->data_size) {
		bus_space_unmap(sc->data_tag, sc->data_handle, sc->data_size);
		sc->data_size = 0;
	}
	return 0;
}

