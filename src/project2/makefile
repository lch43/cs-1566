CC       = gcc 
CFLAGS   = -O3 -Wall 
LIBS      = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL
OBJDIR   = ../libraries
OBJS     = $(OBJDIR)/initShader.o $(OBJDIR)/vandmlib.o

project2: project2.c $(OBJS)
	$(CC) -o project2 project2.c $(OBJS) $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c %.h
	$(CC) -c @< -o $@ $(CFLAGS)

