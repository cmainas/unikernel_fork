#include <sys/syscallargs.h>

#define VMCALL_ID 100

static inline uint32_t inl(uint16_t port)
{
	uint32_t rv;
	__asm__ __volatile__("inl %1, %0" : "=a"(rv) : "d"(port));
	return rv;
}

//static void outb(uint8_t value, uint16_t port) 
//{
//        asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
//}

int sys_my_fork(struct lwp *l, const void *v, register_t *retval)
{
	printf("hello from my fork\n");
	/* vm exit with io in port ffdc writing 77 means fork */
	//outb(77, 0xffdc);
	/* start migration */
	unsigned int ret; 
	ret = inl(0xffdd);
	/* wait until migration is over */
	ret = inl(0xffdc);
	while (ret == 0) {
		ret = inl(0xffdc);
	}
	printf(" phra: %d\n", ret);
	if (ret == 1)
		*retval = inl(0xffdc);
	else 
		*retval = 0;
	return 0;
}
