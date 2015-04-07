ODIR    = obj
SDIR    = src
HDIR    = inc
DEBUG   = 3

OUT     = httpd
INC     = -Iinc
CFLAGS  = -fPIC -g
LDFLAGS = -lpthread

CXX     = g++
ECHO    = /bin/echo
MKDIR   = mkdir -p

OBJS    = $(patsubst $(SDIR)/%.cc, $(ODIR)/%.o, $(shell find $(SDIR) -type f -name "*.cc"))

all: $(OUT)

$(ODIR)/%.o: $(SDIR)/%.cc
	@$(ECHO) -n Compiling $(@F)...
	@$(MKDIR) $(@D)
	@$(CXX) $(CFLAGS) -DDEBUG=$(DEBUG) -o $@ -c $< -I$(HDIR) -I$(patsubst $(ODIR)/%,$(HDIR)/%,$(@D))
	@$(ECHO) Success!

$(OUT): $(OBJS)
	@$(ECHO) -n Linking $(@F)...
	@$(CXX) $(LDFLAGS) -o $(OUT) $(OBJS)
	@$(ECHO) Done!
