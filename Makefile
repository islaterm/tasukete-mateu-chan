# Para usar este Makefile es necesario definir la variable
# de ambiente NSYSTEM con el directorio en donde se encuentra
# la raiz de nSystem.  En csh esto se hace con:
#
#   setenv NSYSTEM ~cc41b/nSystem95
#
# Para compilar ingrese make APP=<ejemplo>
#
# Ej: make APP=iotest
#
# Elegir una entre los siguientes ejemplos
#
# msgprodcons iotest test term-serv
#

LIBNSYS= /mnt/c/Users/ASUS/Documents/Escuela/nsystem64-beta2/lib/libnSys.a

CFLAGS= -ggdb -I /mnt/c/Users/ASUS/Documents/Escuela/nsystem64-beta2/include -I /mnt/c/Users/ASUS/Documents/Escuela/nsystem64-beta2/src
LFLAGS= -ggdb

all: $(APP)

.SUFFIXES:
.SUFFIXES: .o .c .s

.c.o .s.o:
	gcc -c $(CFLAGS) $<

$(APP): $(APP).o $(LIBNSYS)
	gcc $(LFLAGS) $@.o -o $@ $(LIBNSYS)

clean:
	rm -f *.o *~

cleanall:
	rm -f *.o *~ iotest term-serv
