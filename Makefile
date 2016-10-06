all:
	gcc -O3 -march=native src/main.c lib/ini/ini.c lib/parson/parson.c -o main
