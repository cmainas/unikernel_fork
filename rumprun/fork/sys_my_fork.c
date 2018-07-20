#include <sys/syscallargs.h>

#define VMCALL_ID 100

static void outb(uint8_t value, uint16_t port) {
        asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

int sys_my_fork(struct lwp *l, const void *v, register_t *retval)
{
	printf("hello from my fork\n");
	/* vm exit with io in port ffdc writing 77 means fork */
	outb(77, 0xffdc);
	*retval = 0;
	return 0;
}
