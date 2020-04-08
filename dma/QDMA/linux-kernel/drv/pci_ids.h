/*
 * This file is part of the Xilinx DMA IP Core driver for Linux
 *
 * Copyright (c) 2017-2019,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 */

#ifndef __XDMA_PCI_ID_H__
#define __XDMA_PCI_ID_H__
/**
 * @file
 * @brief This file contains the list of pcie devices supported for qdma driver
 *
 */

/**
 * list of pcie devices supported for qdma driver
 */
static const struct pci_device_id pci_ids[] = {

#ifdef __QDMA_VF__
	/** Gen 1 VF */
	/** PCIe lane width x1 */
	{ PCI_DEVICE(0x10ee, 0xa400), },	/** VF on PF 0 */
#else
	/** Gen 1 PF */
	/** PCIe lane width x1 */
	{ PCI_DEVICE(0x10ee, 0xa400), },	/** PF 0 */
#endif

	{0,}
};

/** module device table */
MODULE_DEVICE_TABLE(pci, pci_ids);

#endif /* ifndef __XDMA_PCI_ID_H__ */
