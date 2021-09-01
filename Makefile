.PHONY : all clean

CFG ?= debug

CFG_DIR := $(CFG)/
BIN_DIR := bin/$(CFG_DIR)
INT_DIR := bin_int/$(CFG_DIR)
SRC_DIR := src/
OBJ_DIR := $(INT_DIR)obj/
DEP_DIR := $(INT_DIR)dep/

SRC_EXT := .c
OBJ_EXT := .o
DEP_EXT := .d

SRC := $(shell find $(SRC_DIR) -name *$(SRC_EXT))
OBJ := $(SRC:$(SRC_DIR)%$(SRC_EXT)=$(OBJ_DIR)%$(OBJ_EXT))
DEP := $(SRC:$(SRC_DIR)%$(SRC_EXT)=$(DEP_DIR)%$(DEP_EXT))
OUT := $(BIN_DIR)shader-testbed.exe

CC := gcc
CFLAGS += \
	-std=c99 \
	-Werror \
	-Wabi \
	-Waddress \
	-Wall \
	-Walloc-zero \
	-Walloca \
	-Wbool-compare \
	-Wbool-operation \
	-Wbuiltin-declaration-mismatch \
	-Wbuiltin-macro-redefined \
	-Wcast-qual \
	-Wchar-subscripts \
	-Wclobbered \
	-Wcomment \
	-Wconversion \
	-Wcpp \
	-Wdangling-else \
	-Wdate-time \
	-Wdeprecated \
	-Wdesignated-init \
	-Wdiscarded-array-qualifiers \
	-Wdiscarded-qualifiers \
	-Wdiv-by-zero \
	-Wdouble-promotion \
	-Wduplicate-decl-specifier \
	-Wduplicated-branches \
	-Wduplicated-cond \
	-Wempty-body \
	-Wendif-labels \
	-Wenum-compare \
	-Wexpansion-to-defined \
	-Wextra \
	-Wfloat-conversion \
	-Wfloat-equal \
	-Wformat-contains-nul \
	-Wformat-extra-args \
	-Wformat-nonliteral \
	-Wformat-security \
	-Wformat-signedness \
	-Wformat-y2k \
	-Wformat-zero-length \
	-Wframe-address \
	-Wignored-attributes \
	-Wignored-qualifiers \
	-Wimplicit \
	-Wimplicit-function-declaration \
	-Wimplicit-int \
	-Wincompatible-pointer-types \
	-Winit-self \
	-Wint-conversion \
	-Wint-in-bool-context \
	-Wint-to-pointer-cast \
	-Winvalid-pch \
	-Wjump-misses-init \
	-Wlogical-not-parentheses \
	-Wlogical-op \
	-Wlong-long \
	-Wmain \
	-Wmaybe-uninitialized \
	-Wmemset-elt-size \
	-Wmemset-transposed-args \
	-Wmisleading-indentation \
	-Wmissing-braces \
	-Wmissing-declarations \
	-Wmissing-field-initializers \
	-Wmissing-include-dirs \
	-Wmissing-parameter-type \
	-Wmissing-prototypes \
	-Wmultichar \
	-Wnarrowing \
	-Wnested-externs \
	-Wnonnull \
	-Wnonnull-compare \
	-Wold-style-declaration \
	-Wold-style-definition \
	-Wopenmp-simd \
	-Woverlength-strings \
	-Woverride-init \
	-Woverride-init-side-effects \
	-Wpacked-bitfield-compat \
	-Wparentheses \
	-Wpedantic \
	-Wpointer-arith \
	-Wpointer-compare \
	-Wpointer-sign \
	-Wpointer-to-int-cast \
	-Wpragmas \
	-Wpsabi \
	-Wredundant-decls \
	-Wrestrict \
	-Wreturn-type \
	-Wsequence-point \
	-Wshift-count-negative \
	-Wshift-count-overflow \
	-Wshift-negative-value \
	-Wsign-compare \
	-Wsign-conversion \
	-Wsizeof-array-argument \
	-Wsizeof-pointer-memaccess \
	-Wstrict-prototypes \
	-Wsuggest-attribute=format \
	-Wswitch \
	-Wswitch-bool \
	-Wswitch-default \
	-Wsync-nand \
	-Wtautological-compare \
	-Wtrigraphs \
	-Wundef \
	-Wuninitialized \
	-Wunknown-pragmas \
	-Wunsuffixed-float-constants \
	-Wunused-function \
	-Wunused-local-typedefs \
	-Wunused-macros \
	-Wunused-result \
	-Wunused-variable \
	-Wvarargs \
	-Wvla \
	-Wvolatile-register-var \
	-Wwrite-strings

ifeq ($(CFG),debug)
	CFLAGS += -g
else
	CFLAGS += -DNDEBUG -O3
endif

LDFLAGS := -lgdi32 -lopengl32

TARGET_OBJ_TO_DEP = $(@:$(OBJ_DIR)%$(OBJ_EXT)=$(DEP_DIR)%$(DEP_EXT))

all : $(OUT)

clean :
	$(RM) -r $(BIN_DIR) $(INT_DIR)

$(OUT) : $(OBJ)
	$(shell mkdir -p $(dir $@))
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)%$(OBJ_EXT) : $(SRC_DIR)%$(SRC_EXT)
	$(shell mkdir -p $(dir $@) $(dir $(TARGET_OBJ_TO_DEP)))
	$(CC) -c $< -o $@ $(CFLAGS) -MMD -MP -MF $(TARGET_OBJ_TO_DEP)

-include $(DEP)
