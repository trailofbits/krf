SYSCALL_C_FILES!=ls syscalls/*.gen.c
SRCS=krf.c syscalls.c ../config.c ../krf.c ${SYSCALL_C_FILES}
KMOD=krf

.include <bsd.kmod.mk>