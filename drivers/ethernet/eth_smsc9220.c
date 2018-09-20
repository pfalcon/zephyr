/*
 * Copyright (c) ...
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define LOG_MODULE_NAME eth_smsc9220
#define LOG_LEVEL CONFIG_ETHERNET_LOG_LEVEL

#include <logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <board.h>
#include <device.h>
#include <errno.h>
#include <init.h>
#include <kernel.h>
#include <misc/__assert.h>
#include <net/net_core.h>
#include <net/net_pkt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys_io.h>
#include <net/ethernet.h>

#ifdef CONFIG_SHARED_IRQ
#include <shared_irq.h>
#endif

#include "smsc9220_eth.h"

static enum ethernet_hw_caps eth_smsc9220_get_capabilities(struct device *dev)
{
	ARG_UNUSED(dev);
	printk("get_cap\n");

	return ETHERNET_LINK_10BASE_T | ETHERNET_LINK_100BASE_T;
}

static void eth_initialize(struct net_if *iface)
{
	printk("eth_initialize\n");
	return;
}

static int eth_tx(struct net_if *iface, struct net_pkt *pkt)
{
	printk("eth_tx\n");
	net_pkt_unref(pkt);
	return 0;
}

static const struct ethernet_api api_funcs = {
	.iface_api.init = eth_initialize,
	.iface_api.send = eth_tx,

	.get_capabilities = eth_smsc9220_get_capabilities,
};

static void eth_smsc9220_isr(struct device *port)
{
	printk("eth_smsc9220_isr: %x %x\n", SMSC9220->INT_STS, SMSC9220->INT_EN);
}

/* Bindings to the plaform */

static struct device DEVICE_NAME_GET(eth_smsc9220_0);

int eth_init(struct device *dev)
{
	IRQ_CONNECT(SMSC_LAN9220_0_IRQ_0,
		    SMSC_LAN9220_0_IRQ_0_PRIORITY,
		    eth_smsc9220_isr, DEVICE_GET(eth_smsc9220_0), 0);

	int ret = smsc9220_init();

	if (ret != 0) {
		LOG_ERR("smsc9220 failed to initialize");
		return -ENODEV;
	}

	printk("init ok\n");
	//k_sleep(2000);

	irq_enable(SMSC_LAN9220_0_IRQ_0);

	char mac[6];
	smsc9220_read_mac_address(mac);

	LOG_ERR("MAC %02x:%02x:%02x:%02x:%02x:%02x",
                    mac[0], mac[1],
                    mac[2], mac[3],
                    mac[4], mac[5]);

	return ret;
}

NET_DEVICE_INIT(eth_smsc9220_0, "smsc9220_0",
		eth_init, NULL/*&eth_0_runtime*/,
		NULL/*&eth_config_0*/, CONFIG_ETH_INIT_PRIORITY, &api_funcs,
		ETHERNET_L2, NET_L2_GET_CTX_TYPE(ETHERNET_L2),
		1500/*MTU*/);
