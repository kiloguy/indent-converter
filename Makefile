all: indent-converter
indent-converter: indent-converter.c
	gcc -o indent-converter -Wall -Werror -Wextra indent-converter.c
