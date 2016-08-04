#ifndef _WEIR_USERSPACE_DAEMON
#define _WEIR_USERSPACE_DAEMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <android/log.h>
#include <sys/types.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <sys/un.h>
#include <weir/weir.h>

#define NETLINK_HELLO 17
#define WEIR_MGR_ALLOW 0
#define WEIR_MGR_DENY 1
#define MAX_PAYLOAD 2048
#define TAG "WEIR_DAEMON"

#endif /* _WEIR_USERSPACE_DAEMON */
