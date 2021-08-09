.RECIPEPREFIX=>

CFLAGS=-std=c11 -g -static

jamcc: jamcc.c

test: jamcc
>./test.sh

clean:
>rm -f jamcc *.o *~ tmp*

.PHONY: test clean
