#	$NetBSD: Makefile,v 1.4 2015/11/16 23:27:08 pooka Exp $
#

RUMPTOP=${TOPRUMP}

.PATH:	${RUMPTOP}/../dev/pci

LIB=	rumpdev_ivshmem
COMMENT=ivshmem

IOCONF=	IVSHMEM.ioconf
RUMP_COMPONENT=ioconf

SRCS+=	ivshmem.c
##SRCS+=	ivshmem_component.c

CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern
   
.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
