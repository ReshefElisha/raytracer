PROJECT := raytracer
BUILDDIR := $(CURDIR)/build
OUT := $(BUILDDIR)/$(PROJECT)

SOURCES += \
	$(wildcard src/*.cpp)

INCLUDES += \
	include/

FLAG = -
CC   = g++

ifeq ($(OS),WINDOWS)
ifeq ($(COMPILER),cl)
CC = cl
SHELL = cmd
CFLAGS = /EHsc /Fo"$(BUILDDIR)\\"
FLAG = /

CUDA_SOURCES += \
	$(wildcard src/*.cu)

ADDTL_DEPS = cuda_intermediates
else
CFLAGS += \
	-static-libstdc++
endif
endif

main: $(BUILDDIR) $(ADDTL_DEPS)
	$(CC) $(FLAG)o $(OUT) $(SOURCES) $(addprefix $(FLAG)I, $(INCLUDES)) $(addprefix $(FLAG)D, $(DEFINES)) $(CFLAGS)

cuda_intermediates:
	nvcc $(CUDA_SOURCES) -o $(BUILDDIR)/cuda_inter -allow-unsupported-compiler

ifeq ($(OS),WINDOWS)
$(BUILDDIR):
	mkdir $(subst /,\,$@)
else
$(BUILDDIR):
	mkdir -p $@
endif