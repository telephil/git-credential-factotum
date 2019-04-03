CC=9c
LD=9l
CFLAGS=-Wall
LDFLAGS=

TARG=git-credential-factotum

OFILES=git-credential-factotum.o

all:V:	$TARG

$TARG:	$OFILES
	$LD -o $target $prereq $LDFLAGS

%.o:	%.c
	$CC $CFLAGS $stem.c

clean:V:
	rm $OFILES $TARG
