all:
	gcc -O3 -march=native main.c lib/ini/ini.c lib/parson/parson.c -o main
