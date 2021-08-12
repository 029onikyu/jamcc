.RECIPEPREFIX=>

CFLAGS= -O0 -g -static -Wall -Wextra

jamcc: jamcc.c utility.c tokenizer.c parser.c

test: jamcc
>./test.sh

clean:
>rm -f jamcc *.o *~ tmp*

rebuild: clean jamcc

.PHONY: test clean rebuild
