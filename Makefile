all:
	gcc -O3 -march=native main.c ini.c parson.c -o main
