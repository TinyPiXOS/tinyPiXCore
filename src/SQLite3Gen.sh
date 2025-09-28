gcc -c ./src_p/TpExtUtils/sqlite3.c -lpthread -ldl -o sqlite3.o
ar -r ./depend_lib/static/x86_64/libsqlite3.a sqlite3.o
rm -rf sqlite3.o