all: prep exe

prep:
	cp ../../bin/SQLite_error_codes.html .
	cp ../../bin/error_page.h .

exe: main.o
	gcc -W -g main.o -o exe

main.o: main.c error_page.h
	gcc -W -g -c main.c

main.c: SQLite_error_codes.html
	echo '#include <stdio.h>' >main.c
	echo '#include "error_page.h"' >>main.c
	echo >>main.c
	echo 'const char error_page_bytes[] = {' >>main.c
	sed 's/\\/\\\\/g;s/"/\\"/g;s/^/"/;s/$$/\\n"/' SQLite_error_codes.html >>main.c
	echo '};' >>main.c
	echo 'int len = sizeof(error_page_bytes);' >>main.c
	echo >>main.c
	echo 'int main() {' >>main.c
	echo '  FILE *f;'>>main.c
	echo '  f = fopen("foo.html", "w");' >>main.c
	echo '  fwrite(error_page_bytes, len, 1, f);' >>main.c
	echo '  fclose(f);' >>main.c
	echo '  printf("%s\\n", error_page_bytes);' >>main.c
	echo '  printf("%i\\n", len);' >>main.c
	echo '}' >>main.c
