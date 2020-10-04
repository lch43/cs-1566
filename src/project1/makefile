CC       = gcc 
CFLAGS   = -O3 -Wall 
LIBS      = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL
OBJDIR   = ../libraries
OBJS     = $(OBJDIR)/initShader.o $(OBJDIR)/vandmlib.o

project1: project1.c $(OBJS)
	$(CC) -o project1 project1.c $(OBJS) $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c %.h
	$(CC) -c @< -o $@ $(CFLAGS)

