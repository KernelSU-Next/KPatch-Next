
CFLAGS = -std=c11 -Wall -Wextra -Wno-unused -Wno-unused-parameter

SRC += kpatch.c
SRC += kpm.c
SRC += kpexclude.c


OBJS := $(SRCS:.c=.o)

all: kpatch.a kpatch

kpatch: main.o ${OBJS}
	${CC} -o $@ $^

kpatch.a: ${OBJS}
	${AR} rcs $@ $^

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf build
	rm -rf uapi
	rm -f kpatch
	rm -f *.a 
	find . -name "*.o" | xargs rm -f