gcc -c ./src_p/ExternUtils/sqlite3.c -lpthread -ldl -o sqlite3.o
ar -r ./depend_lib/libsqlite3.a sqlite3.o
rm -rf sqlite3.o