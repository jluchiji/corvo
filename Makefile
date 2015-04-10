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

all: $(OUT)

.PHONY: force
force: clean clean-res clean-cgi resources cgi $(OUT)
	@rm -rf $(TDIR)

# --------------------------------------------------------------------------- #
# Compilation and linking of the httpd executable.                            #
# --------------------------------------------------------------------------- #

SRCS    = $(shell find $(SDIR) -type f -name "*.cc")
OBJS    = $(patsubst $(SDIR)/%.cc, $(ODIR)/%.o,   $(SRCS))
DEPS    = $(patsubst $(SDIR)/%.cc, $(ODIR)/%.dep, $(SRCS))

.deps: $(DEPS)

$(ODIR)/%.dep: $(SDIR)/%.cc
	@printf "Analyzing  %-25s" $(notdir $<)...
	@$(CXX) $(CFLAGS) -I$(HDIR) -I$(patsubst $(ODIR)/%,$(HDIR)/%,$(@D)) -MM $^ >> .deps
	@printf "\033[1;32mDone!\033[0m\n"

-include .deps

$(OUT): $(OBJS)
	@printf "Linking    %-25s" $(@F)...
	@$(CXX) $(LDFLAGS) -o $(OUT) $(OBJS)
	@printf "\033[1;32mDone!\033[0m\n"

$(ODIR)/%.o: $(SDIR)/%.cc
	@printf "Compiling  %-25s" $(notdir $<)...
	@$(MKDIR) $(@D)
	@$(CXX) $(CFLAGS) -DDEBUG=$(DEBUG) -o $@ -c $< -I$(HDIR) -I$(patsubst $(ODIR)/%,$(HDIR)/%,$(@D))
	@printf "\033[1;32mDone!\033[0m\n"


# --------------------------------------------------------------------------- #
# Conversion of binary files into embeddable resource headers.                #
# --------------------------------------------------------------------------- #

RDIR    = $(SDIR)/app.httpd/res
RHDIR   = $(HDIR)/app.httpd/res
RES     = $(patsubst %, $(RHDIR)/%.h, $(basename $(shell ls $(RDIR))))

.PHONY: resources
resources: clean-res $(RES)

$(RHDIR)/%.h: $(TDIR)/%
	@printf "Generating %-25s" $(@F)...
	@$(MKDIR) $(RHDIR)
	@$(ECHO) \#ifndef\ RES_$(shell echo $(basename $(@F)) | tr a-z A-Z) > $@
	@$(ECHO) \#define\ RES_$(shell echo $(basename $(@F)) | tr a-z A-Z)\ \\ >> $@
	@hexdump -e '24/1 "_x%02X" "\n"' $^ | sed 's/_/\\/g; s/.*/    "&"/; s/\\x  //g; s/$$/\\/g;' >> $@
	@$(ECHO) >> $@
	@$(ECHO) \#endif >> $@
	@printf "\033[1;32mDone!\033[0m\n"

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
# Build loadable modules.                                                     #
# --------------------------------------------------------------------------- #

CGI_SDIR = http-root-dir/cgi-src
CGI_DDIR = http-root-dir/cgi-bin
CGI_LIST = hello
CGI_OBJS = $(patsubst %, $(CGI_DDIR)/%.so, $(CGI_LIST))

.PHONY: cgi
cgi: $(CGI_OBJS)

$(CGI_DDIR)/%.so: $(CGI_SDIR)/%.cc
	@printf "Compiling  %-25s" $(notdir $<)...
	@$(CXX) $(CFLAGS) -shared -o $@ $<
	@printf "\033[1;32mDone!\033[0m\n"

# --------------------------------------------------------------------------- #
# Cleanup.                                                                    #
# --------------------------------------------------------------------------- #
.PHONY: clean clean-res clean-deps
clean-res:
	@printf "Cleaning   %-25s" "resources"...
	@rm -f $(RHDIR)/*.h
	@printf "\033[1;32mDone!\033[0m\n"

clean-deps:
	@printf "Cleaning   %-25s" "dependencies"...
	@rm -f .deps
	@printf "\033[1;32mDone!\033[0m\n"

clean-cgi:
	@printf "Cleaning   %-25s" "cgi binaries"...
	@rm -f $(CGI_DDIR)/*.so
	@printf "\033[1;32mDone!\033[0m\n"

clean: clean-deps clean-cgi
	@printf "Cleaning   %-25s" "binaries"...
	@rm -rf obj tmp
	@rm -f httpd
	@printf "\033[1;32mDone!\033[0m\n"
