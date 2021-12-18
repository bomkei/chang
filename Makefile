TARGET		= chang

BUILD			= build
SRCDIR		= src
SOURCES		= $(wildcard $(SRCDIR)/*.cc)
OFILES		= $(patsubst %.cc,$(BUILD)/%.o,$(notdir $(SOURCES)))
HEADER		= $(SRCDIR)/$(TARGET).h

COMMON		= -O2
CXXFLAGS	= $(COMMON) -std=c++20
LDFLAGS		= -Wl,--gc-sections

all: $(TARGET)

clean:
	@rm -drf $(BUILD)
	@rm -drf $(TARGET)

re: clean all

debug:
	@echo todo

install:
	@echo todo

$(BUILD)/%.o: $(SRCDIR)/%.cc
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	clang++ $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OFILES)
	clang++ $(LDFLAGS) -o $@ $^