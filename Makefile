ODIR    = obj
SDIR    = src
HDIR    = inc
TDIR    = tmp
DEBUG   = 3

OUT     = httpd
INC     = -Iinc
CFLAGS  = -fPIC -g
LDFLAGS = -lpthread

CXX     = g++
ECHO    = /bin/echo
MKDIR   = mkdir -p


# --------------------------------------------------------------------------- #
# Compilation and linking of the httpd executable.                            #
# --------------------------------------------------------------------------- #

SRCS    = $(shell find $(SDIR) -type f -name "*.cc")
OBJS    = $(patsubst $(SDIR)/%.cc, $(ODIR)/%.o,   $(SRCS))
DEPS    = $(patsubst $(SDIR)/%.cc, $(ODIR)/%.dep, $(SRCS))

all: $(OUT)

.deps: $(DEPS)

$(ODIR)/%.dep: $(SDIR)/%.cc
	@$(CXX) $(CFLAGS) -I$(HDIR) -I$(patsubst $(ODIR)/%,$(HDIR)/%,$(@D)) -MM $^ >> .deps

-include .deps

$(OUT): $(OBJS)
	@$(ECHO) -n Linking $(@F)...
	@$(CXX) $(LDFLAGS) -o $(OUT) $(OBJS)
	@$(ECHO) Done!

$(ODIR)/%.o: $(SDIR)/%.cc
	@$(ECHO) -n Compiling $(@F)...
	@$(MKDIR) $(@D)
	@$(CXX) $(CFLAGS) -DDEBUG=$(DEBUG) -o $@ -c $< -I$(HDIR) -I$(patsubst $(ODIR)/%,$(HDIR)/%,$(@D))
	@$(ECHO) Success!



# --------------------------------------------------------------------------- #
# Conversion of binary files into embeddable resource headers.                #
# --------------------------------------------------------------------------- #

RDIR    = $(SDIR)/app.httpd/res
RHDIR   = $(HDIR)/app.httpd/res
RES     = $(patsubst %, $(RHDIR)/%.h, $(basename $(shell ls $(RDIR))))

.PHONY: resources
resources: clean-res $(RES)

$(RHDIR)/%.h: $(TDIR)/%
	@echo Building $(@F)...
	@$(MKDIR) $(RHDIR)
	@$(ECHO) \#ifndef\ RES_$(shell echo $(basename $(@F)) | tr a-z A-Z) > $@
	@$(ECHO) \#define\ RES_$(shell echo $(basename $(@F)) | tr a-z A-Z)\ \\ >> $@
	@hexdump -e '24/1 "_x%02X" "\n"' $^ | sed 's/_/\\/g; s/.*/    "&"/; s/\\x  //g; s/$$/\\/g;' >> $@
	@$(ECHO) >> $@
	@$(ECHO) \#endif >> $@

$(TDIR)/%: $(RDIR)/%.jade
	@$(MKDIR) tmp
	@jade < $^ > $@

$(TDIR)/%: $(RDIR)/%.css
	@$(MKDIR) tmp
	@cssmin $^ > $@

$(TDIR)/%: $(RDIR)/%.less
	@$(MKDIR) tmp
	@lessc $^ | cssmin > $@

$(TDIR)/%: $(RDIR)/%.svg
	@$(MKDIR) tmp
	@cp $^ $@


# --------------------------------------------------------------------------- #
# Cleanup.                                                                    #
# --------------------------------------------------------------------------- #
.PHONY: clean clean-res clean-deps
clean-res:
	@rm -f $(RHDIR)/*.h

clean-deps:
	@rm -f .deps

clean: clean-deps
	@rm -rf obj tmp
	@rm -f httpd
