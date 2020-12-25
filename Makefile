PROJECT := raytracer
BUILDDIR := $(CURDIR)/build
OUT := $(BUILDDIR)/$(PROJECT)

SOURCES += \
	$(wildcard src/*.cpp)

CUDA_SOURCES += \
	$(wildcard src/*.cu)

INCLUDES += \
	include/

ifeq ($(OS),WINDOWS)
CFLAGS += \
	-static-libstdc++ -g
endif

main: $(BUILDDIR)
	g++ -o $(OUT) $(SOURCES) $(addprefix -I, $(INCLUDES)) $(addprefix -D, $(DEFINES)) $(CFLAGS)

ifeq ($(OS),WINDOWS)
$(BUILDDIR):
	mkdir $(subst /,\,$@)
else
$(BUILDDIR):
	mkdir -p $@
endif