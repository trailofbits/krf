SRCS=krf.c
KMOD=krf

.include <bsd.kmod.mk>

.PHONY insmod
insmod:
	kldload -v ./krf.ko
