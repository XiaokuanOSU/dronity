/*
 * SYSCALL_DEFINE4(epoll_ctl, int, epfd, int, op, int, fd, struct epoll_event __user *, event)
 *
 * When successful, epoll_ctl() returns zero.
 * When an error occurs, epoll_ctl() returns -1 and errno is set appropriately.
 */
#include <sys/epoll.h>
#include "sanitise.h"
#include "random.h"
#include "utils.h"
#include "compat.h"

//fix missing def in case of android 19
#ifndef EPOLLRDHUP
#define EPOLLRDHUP       0x00002000
#define EPOLLONESHOT     0x40000000
#endif

static const unsigned long epoll_flags[] = {
	EPOLLIN, EPOLLOUT, EPOLLRDHUP, EPOLLPRI,
	EPOLLERR, EPOLLHUP, EPOLLET, EPOLLONESHOT,
	EPOLLWAKEUP,
};

static void sanitise_epoll_ctl(struct syscallrecord *rec)
{
	struct epoll_event *ep;

	ep = zmalloc(sizeof(struct epoll_event));
	ep->data.fd = get_random_fd();
	ep->events = set_rand_bitmask(ARRAY_SIZE(epoll_flags), epoll_flags);
	rec->a4 = (unsigned long) ep;
}

static void post_epoll_ctl(struct syscallrecord *rec)
{
	free((void *)rec->a4);
}

struct syscallentry syscall_epoll_ctl = {
	.name = "epoll_ctl",
	.num_args = 4,
	.arg1name = "epfd",
	.arg1type = ARG_FD,
	.arg2name = "op",
	.arg2type = ARG_OP,
	.arg2list = {
		.num = 3,
		.values = { EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL },
	},
	.arg3name = "fd",
	.arg3type = ARG_FD,
	.arg4name = "event",
	.rettype = RET_ZERO_SUCCESS,
	.flags = NEED_ALARM,
	.sanitise = sanitise_epoll_ctl,
	.post = post_epoll_ctl,
};
