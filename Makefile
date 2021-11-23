# ******************************************************************************
# FLAGS
# ******************************************************************************
CC        :=  gcc
DEBUG     :=  -ggdb
WARNINGS  :=  -Wall #-Wextra
TEST      :=  -lcunit

LIBRARIES =
INCLUDES  =

CFLAGS    += $(DEBUG) $(WARNINGS) -std=c11
LDFLAGS   += $(LIBRARIES)


# ******************************************************************************
# DIRECTORIES & FILES
# ******************************************************************************
OUTDIR    = bin
OBJDIR    = obj
SRCDIR    = src

# Input files
SOURCES := main.c blockemulator.c
OBJECTS := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

# Final output name
OUT       := fs_test


# ******************************************************************************
# TARGET-DEPENDENT CHECKS
# ******************************************************************************

# ******************************************************************************
# PHONY
# ******************************************************************************
.PHONY: all prepare print obj clean run


# ******************************************************************************
# COMPILATION
# ******************************************************************************
all: prepare out

out: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(OUT) $^ $(LDFLAGS)

run:
	$(OUTDIR)/$(OUT)

# ******************************************************************************
# OBJECT COMPILATION
# ******************************************************************************

obj: $(OBJECTS)

# If we have a header file for the src file.
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	$(CC) $(CFLAGS) $(LIBRARIES) $(INCLUDES) $< -c -o $@
# If source file (like main) does not have a header file.
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(LIBRARIES) $(INCLUDES) $< -c -o $@


# ******************************************************************************
# PREPWORK
# ******************************************************************************

prepare:
	@-mkdir bin
	@-mkdir obj
	@-mkdir dbg


# ******************************************************************************
# CLEANING
# ******************************************************************************# Cleaning rules
clean:
	@-rm -rf $(OBJDIR)/*
	@-rm -rf $(OUTDIR)/*


# ******************************************************************************
# MAKEFILE DEBUGGING
# ******************************************************************************

print:
	@echo CC        = $(CC)
	@echo CFLAGS    = $(CFLAGS)
	@echo LDFLAGS   = $(LDFLAGS)
	@echo LIBRARIES = $(LIBRARIES)
	@echo INCLUDES  = $(INCLUDES)
	@echo OUTDIR    = $(OUTDIR)
	@echo SRCDIR    = $(SRCDIR)
	@echo OBJDIR    = $(OBJDIR)
	@echo SOURCES   = $(SOURCES)
	@echo OBJECTS   = $(OBJECTS)