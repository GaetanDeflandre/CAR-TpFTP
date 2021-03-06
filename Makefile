# -------------------------------------------------------------
# Makefile using to compile the source of the server FTP.
# -------------------------------------------------------------


###------------------------------
### Compile options
###------------------------------------------------------------
CC	= gcc
CFLAGS	= -Wall -Werror
CFLAGS	+= -ansi -pedantic
CFLAGS  += -D_XOPEN_SOURCE=500
CFLAGS	+= -O3 -pipe -std=gnu11
CFLAGS	+= -g `xml2-config --cflags`

LDFLAGS = `xml2-config --libs`

TRASHFILE = *.gch *~ \#*\# nul
RM	  = rm -f


###------------------------------
### Paths
###------------------------------------------------------------
.SUFFIXES: .c .o

SRCDIR	= src
INCDIR	= include
TESTDIR = test
OBJDIR	= obj 
BINDIR	= bin
ETCDIR	= etc
DOCDIR	= doc  

SOURCES	 = servFTP.c clientHandler.c command.c database.c dtp.c communication.c
TESTS 	 = testDatabase.c
BINARIES = servFTP testDatabase 
HEADERS  = ${SOURCE:.c=.h}
OBJECTS  = ${SOURCE:.c=.o}


SRCPATHS = ${addprefix $(SRCDIR)/, $(SOURCES)}
INCPATHS = ${addprefix $(INCDIR)/, $(HEADERS)}
TESTPATHS= ${addprefix $(TESTDIR)/, $(TESTS)}
OBJPATHS = ${addprefix $(OBJDIR)/, $(OBJECTS)}
BINPATHS = ${addprefix $(BINDIR)/, $(BINARIES)}

RESSOURCES = etc/database.xml


###------------------------------
### Main targets 
###------------------------------------------------------------
all: $(BINPATHS) $(OBJPATHS) $(RESSOURCES)


###------------------------------
### Make binaries
###------------------------------------------------------------
bin/servFTP: obj/servFTP.o obj/clientHandler.o obj/command.o obj/database.o obj/dtp.o obj/communication.o | $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/testDatabase: \
	obj/testDatabase.o obj/database.o | $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)


###------------------------------
### Make binaries directory
###------------------------------------------------------------
bin:
	mkdir -p $(BINDIR)


###------------------------------
### Compile source
###------------------------------------------------------------
obj/servFTP.o:		src/servFTP.c include/servFTP.h obj/database.o
obj/clientHandler.o:	src/clientHandler.c include/clientHandler.h
obj/command.o:		src/command.c include/command.h 
obj/communication.o:		src/communication.c include/communication.h 
obj/database.o:		src/database.c include/database.h
obj/dtp.o:		src/dtp.c include/dtp.h

obj/testDatabase.o:	test/testDatabase.c src/database.c include/database.h


obj/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ -c $<

obj/%.o: test/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ -c $<


###------------------------------
### Make objects directory
###------------------------------------------------------------
obj:
	mkdir -p $(OBJDIR)


###------------------------------
### Makefly checker
###------------------------------------------------------------
check-syntax:
	$(CC) $(CFLAGS) -I$(INCDIR) -o nul -S $(CHK_SOURCES) 


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean cleantrash realclean doc cleandoc

cleantrash:
	${foreach trash,$(TRASHFILE),\
		find . -name "$(trash)" -type f -delete;\
	}

clean: cleantrash
	$(RM) -R $(OBJDIR)

cleanbin:
	$(RM) -R $(BINDIR)

cleandoc:
	$(RM) -R $(DOCDIR)

realclean: clean cleanbin

doc: $(ETCDIR)/doxygen.ini
	doxygen $<
