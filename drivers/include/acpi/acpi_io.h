#ifndef _ACPI_IO_H_
#define _ACPI_IO_H_

#include <linux/io.h>

#include <asm/acpi.h>


void acpi_os_map_iomem(acpi_physical_address phys, acpi_size size);
void acpi_os_unmap_iomem(void __iomem *virt, acpi_size size);
void *acpi_os_get_iomem(acpi_physical_address phys, unsigned int size);

int acpi_os_map_generic_address(struct acpi_generic_address *addr);
void acpi_os_unmap_generic_address(struct acpi_generic_address *addr);

#endif
