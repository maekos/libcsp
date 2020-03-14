#ifndef _ETH_H_
#define _ETH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int eth_init();

int eth_send(void * data, uint32_t len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _UDP_H_ */
