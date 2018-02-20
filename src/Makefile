# O (Output) is the build directory
ifeq '$O' ''
O = .
endif
# V (Verbosity) is 0 (quiet) or 1 (verbose)
ifeq '$V' ''
V = 0
endif

# files
SRCS = $(wildcard *.c)
DEPS = $(SRCS:%.c=$O/%.d)
OBJS = $(SRCS:%.c=$O/%.o)
BINS = $O/clunc-open

# options
override CPPFLAGS +=
override CFLAGS += -Wall -Wextra -Werror
override LDFLAGS +=

# first rule (default)
all :

# dependencies
ifneq '$(MAKECMDGOALS)' 'clean'
-include $(DEPS)
endif

# rules verbosity
ifneq '$V' '0'
P = @ true
E =
else
P = @ echo
E = @
endif

# rules

$O :
	$P '  MKDIR   $@'
	$E mkdir -p $O

$O/%.d : %.c | $O
	$P '  DEP     $(notdir $@)'
	$E $(CC) -MM -MT '$@ $(basename $@).o' $(CPPFLAGS) $< -MF $@ || rm -f $@

$O/%.o : %.c
	$P '  CC      $(notdir $@)'
	$E $(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(BINS) : $(OBJS)
	$P '  LD      $(notdir $@)'
	$E $(CC) $(LDFLAGS) $^ -o $@

.PHONY : all
all : $(BINS)

.PHONY : clean
clean :
	$P '  RM      deps objs bins'
	$E - rm -f $(DEPS) $(OBJS) $(BINS)
