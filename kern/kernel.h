#ifndef KERNEL_H
#define KERNEL_H

//exposes a data point for user programs who needs data from the kernel
typedef struct os_metaData{
    volatile unsigned int utilizationRate;
} KernelMetaData;

#endif