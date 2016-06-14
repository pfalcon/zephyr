# Makefile - Kinetis SDK
#
# Copyright (c) 2016, Freescale Semiconductor, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Translate the SOC name and part number into the ksdk device and CPU name
# respectively.
KSDK_DEVICE = $(shell echo $(CONFIG_SOC) | tr '[:lower:]' '[:upper:]')
KSDK_CPU = CPU_$(subst $(DQUOTE),,$(CONFIG_SOC_PART_NUMBER))

ifdef CONFIG_HAS_KSDK
ZEPHYRINCLUDE += -I$(srctree)/ext/hal/ksdk/devices/$(KSDK_DEVICE)
ZEPHYRINCLUDE += -I$(srctree)/ext/hal/ksdk/drivers
ZEPHYRINCLUDE += -I$(srctree)/ext/hal/ksdk/components/phyksz8081

# The ksdk uses the CPU name to expose SOC-specific features of a given
# peripheral. For example, the UART peripheral may be instantiated with/without
# a hardware FIFO, and the size of that FIFO may be different for each instance
# in a given SOC. See fsl_device_registers.h and $(KSDK_DEVICE)_features.h
KBUILD_CFLAGS += -D$(KSDK_CPU)

# Build ksdk device-specific objects. Although it is not normal Kbuild
# practice, drilling down like this avoids the need for repetitive Makefiles
# for every ksdk device.
obj-$(CONFIG_ETH_KSDK) += components/phyksz8081/fsl_phy.o
obj-$(CONFIG_ETH_KSDK) += drivers/fsl_enet.o
obj-$(CONFIG_KSDK_RNGA) += drivers/fsl_rnga.o
obj-y += devices/$(KSDK_DEVICE)/fsl_clock.o

# Build ksdk drivers that can be used for multiple SoC's.
obj-y += drivers/

endif
