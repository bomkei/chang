TARGET		= chang

SUFFIX		= cpp

BUILD			= build
INCLUDES	= include
SOURCES		= src
SUBDIRS		= \
	Builtin \
	Debug \
	Driver \
	Error \
	Evaluater \
	Evaluater/Parts \
	Interpreter \
	Lexer \
	Parser \
	Types

COMMONFLAGS		= -O2
CXXFLAGS			= $(COMMONFLAGS) $(INCLUDE) -std=c++20
LDFLAGS				= -Wl,--gc-sections

ifeq ($(OS),Windows_NT)
@echo you can't build in this OS.

else

%.o: %.$(SUFFIX)
	@echo $(notdir $<)
	@clang++ -MP -MMD -MF $*.d $(CXXFLAGS) -c -o $@ $<

ifneq ($(notdir $(CURDIR)), $(BUILD))

SRCDIRS		= $(SOURCES) $(foreach dir,$(SUBDIRS),$(SOURCES)/$(dir))
CXXFILES	= $(foreach dir,$(SRCDIRS),$(notdir $(wildcard $(dir)/*.$(SUFFIX))))

export VPATH		= $(CURDIR)/$(SOURCES) $(foreach dir,$(SUBDIRS),$(CURDIR)/$(SOURCES)/$(dir))
export OFILES		= $(CXXFILES:.$(SUFFIX)=.o)
export OUTPUT		= $(CURDIR)/$(TARGET)
export INCLUDE	= $(foreach dir,$(INCLUDES),-I $(CURDIR)/$(dir))

all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

debug: $(BUILD)
	@$(MAKE) --no-print-directory COMMONFLAGS="-O0 -g" LDFLAGS="" -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@rm -rf $(BUILD)
	@rm -f $(TARGET)

re: clean all

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

else

DEPENDS	= $(OFILES:.o=.d)

$(OUTPUT): $(OFILES)
	@echo linking...
	@clang++ $(LDFLAGS) -o $@ $^

-include $(DEPENDS)

endif

endif