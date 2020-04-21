# Generic Makefile prototype

UNAME_S != uname -s
UNAME_P != uname -p

PWD_SHOW=@echo -e `echo 'In: '; pwd`

P_NIX != if [ $(UNAME_S) = Linux ] || \
    [ $(UNAME_S) = FreeBSD ] || \
    [ $(UNAME_S) = OpenBSD ] || \
    [ $(UNAME_S) = NetBSD ]; then \
        echo $(uname_s); \
    else \
        echo Unrecognized; \
    fi

P_BSD != if [ $(UNAME_S) = FreeBSD ] || \
    [ $(UNAME_S) = OpenBSD ] || \
    [ $(UNAME_S) = NetBSD ]; then \
        echo Y; \
    else \
        echo N; \
    fi

P_GNULINUX != if [ $(UNAME_S) = Linux ]; then \
        echo Y; \
    else \
        echo N; \
    fi

MAKEFILE != if [ $(UNAME_S) = Linux ]; then \
        echo "GNUmakefile"; \
	elif [ $(UNAME_S) = FreeBSD ] || \
		[ $(UNAME_S) = OpenBSD ] || \
		[ $(UNAME_S) = NetBSD ]; then \
        echo "BSDmakefile"; \
    else \
        echo "Bail. Unsupported platform need makefile."; \
    fi

# pmake might add -J (private)
FLAGS=${.MAKEFLAGS:C/\-J ([0-9]+,?)+//W}

# Redirect all targets to their platform makefile implementations.
all: .GENERIC
.GENERIC:
	##########################################################
	##########################################################
	@echo "Entering generic Makefile."
	${PWD_SHOW}
	@echo "MakeFlags: "${.MAKEFLAGS} " Flags: " ${.FLAGS} " Targets: " ${.TARGETS}
	@echo ${UNAME_S} ${UNAME_P} " BSD: "${P_BSD}" GNU: "${P_GNULINUX}" Making... ["${MAKEFILE}"]"
	${MAKE} -f ${MAKEFILE} ${.TARGETS}

simple:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}
Debug:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}
Release:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}
cleanDebug:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}
cleanRelease:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}
clean:
	${PWD_SHOW}
	- ${MAKE} -f ${MAKEFILE} ${.TARGETS}
	- cd ../ && ${MAKE} -f ${MAKEFILE} ${.TARGETS} # BSD make always chdirs. 
options:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}
help:
	${MAKE} -f ${MAKEFILE} ${.TARGETS}

.PHONY: all .GENERIC simple Debug Release cleanDebug cleanRelease clean options help
