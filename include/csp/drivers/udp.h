#ifndef _UDP_H_
#define _UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int udp_init();

int udp_send(void *data, uint8_t length);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _UDP_H_ */
