TARGET		= chang

BUILD			= build
SRCDIR		= src
SOURCES		= $(wildcard $(SRCDIR)/*.cc)
OFILES		= $(patsubst %.cc,$(BUILD)/%.o,$(notdir $(SOURCES)))
HEADER		= $(SRCDIR)/$(TARGET).h

DEST		= /usr/local/bin

COMMON		= -O2
CXXFLAGS	= $(COMMON) -std=c++20
LDFLAGS		= -Wl,--gc-sections

all: $(TARGET)

clean:
	@rm -drf $(BUILD)
	@rm -drf $(TARGET)

re: clean all

debug:
	@$(MAKE) --no-print-directory CXXFLAGS="-g -O0 -std=c++20"

install: all
	@echo installing...
	@install -s $(TARGET) $(DEST)

uninstall:
	@echo uninstalling...
	@rm -f $(DEST)/$(TARGET)

$(BUILD)/%.o: $(SRCDIR)/%.cc $(HEADER)
	@echo $(notdir $<)
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@clang++ $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OFILES)
	@echo linking...
	@clang++ $(LDFLAGS) -o $@ $^