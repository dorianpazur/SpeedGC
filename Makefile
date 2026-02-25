#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	Speed
BUILD		:=	build
DISCDIR		:=	$(CURDIR)/$(BUILD)/Speed
SOURCES		:=	
DATA		:=	data
INCLUDES	:=
TOOLSDIR	:=	$(CURDIR)/../tools

#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

ifneq ($(BUILD),$(notdir $(CURDIR)))
BASEDIR := $(CURDIR)
else
BASEDIR := $(CURDIR)/../
endif

include $(BASEDIR)/Packages/libogc2/gamecube_rules

#---------------------------------------------------------------------------------
# include platform-independent code
#---------------------------------------------------------------------------------
include $(wildcard ./src/Indep/sources.mk)

#---------------------------------------------------------------------------------
# include gamecube-specific code
#---------------------------------------------------------------------------------
include $(wildcard ./src/GC/sources.mk)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS		:= -std=c++23 -g -O4 -Wall -mcpu=750 $(MACHDEP) $(INCLUDE) $(CFLAGS) -Wno-tautological-compare -Wno-deprecated-literal-operator -Wno-return-type -Wno-unused-variable -Wno-strict-aliasing
CXXFLAGS	:= $(CFLAGS)

LDFLAGS		= -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lax -liso9660 -lfat -logc -lm -lBullet3Common -lBulletDynamics -lBulletCollision -lLinearMath

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CURDIR)/Packages/BulletPhysics/ $(CURDIR)/Packages/ax/

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BASEDIR 	:=	$(CURDIR)

export BRANCH   := $(shell git branch --show-current)
export REVISION := $(shell git rev-list --count HEAD)
export HASH     := $(shell git rev-parse --short HEAD)
export REVISION_CPP	:=	$(BASEDIR)/src/Indep/Revision/Revision.cpp
REVISION_CPP_NOTDIR :=  $(notdir $(REVISION_CPP))
export REVISION_O	:=	$(REVISION_CPP_NOTDIR:.cpp=.o)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
ifeq (,$(findstring $(REVISION_O),$(OFILES_SOURCES)))
export OFILES_SOURCES := $(OFILES_SOURCES) $(REVISION_O)
endif
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD) \
			-I$(LIBOGC_INC) \
			-I$(CURDIR)/Packages/EABase/local/include/Common

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	-L$(LIBOGC_LIB) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export OUTPUT	:=	$(CURDIR)/$(BUILD)/$(TARGET)
export REVISION_OBJ_OUTPUT	:=	$(CURDIR)/$(BUILD)/$(REVISION_O)
.PHONY: $(BUILD) resources clean revision $(REVISION_CPP)

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@[ -d $(DISCDIR) ] || mkdir -p $(DISCDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
discs: ntsc_u.iso
ntsc_u.iso: resources $(OUTPUT).dol
	$(TOOLSDIR)/dollz3 $(OUTPUT).dol Speed/Speed.dol -m
	$(TOOLSDIR)/mkisofs -R -J -G $(TOOLSDIR)/gbi.hdr -no-emul-boot -b Speed.dol -o ntsc_u.iso Speed/
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

resources:
	-cp -f -r ../Resources/Indep/** Speed/
	-cp -f -r ../Resources/GC/** Speed/

$(OFILES_SOURCES) : $(HFILES)

$(REVISION_O): revision $(REVISION_CPP)

revision:
	@rm -fr $(REVISION_O)
	@echo "Revision $(BRANCH)_r$(REVISION) ($(HASH))"
	@echo "const char *Revision = \"$(BRANCH)_r$(REVISION) ($(HASH)) \" __DATE__ \" \" __TIME__;" > $(REVISION_CPP)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	%_jpg.h :	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
