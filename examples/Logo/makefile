all: prep exe

prep:
	cp ../../bin/logo.h .
	cp ../../bin/pico_ql.png .

exe: main.o
	gcc -W -g main.o -o exe

main.o: main.c logo.h
	gcc -W -g -c main.c

main.c: pico_ql.png
	echo '#include <stdio.h>' >main.c
	echo '#include "logo.h"' >>main.c
	echo >>main.c
	echo 'const char logo_bytes[] = {' >>main.c
	od -vb pico_ql.png | sed 's/^.......//;/^$$/d;s/^   /0/;s/  //g;s/$$/,/;s/ /,0/g;' >>main.c
	echo '};' >>main.c
	echo 'int len = sizeof(logo_bytes);' >>main.c
	echo >>main.c
	echo 'int main() {' >>main.c
	echo '  FILE *f;'>>main.c
	echo '  f = fopen("foo.png", "w");' >>main.c
	echo '  fwrite(logo_bytes, len, 1, f);' >>main.c
	echo '  fclose(f);' >>main.c
	echo '  printf("%s\\n", logo_bytes);' >>main.c
	echo '  printf("%i\\n", len);' >>main.c
	echo '}' >>main.c
