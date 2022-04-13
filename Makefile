CC	= clang
CXX	= clang++

TARGET	= chang

COMMONFLAGS	= -O2
CFLAGS		= $(COMMONFLAGS) -Wno-switch
CXXFLAGS	= $(CFLAGS) -std=c++20
LDFLAGS		= -Wl,--gc-sections

CFILES		= $(wildcard *.c)
CXXFILES	= $(wildcard *.cc)
OBJECTS		= $(CFILES:.c=.o) $(CXXFILES:.cc=.o)

%.o: %.c
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cc
	@echo $(notdir $<)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(TARGET)

clean:
	@rm -rf $(TARGET) *.d *.o

re: clean all

$(TARGET): $(OBJECTS)
	@echo linking...
	@$(CXX) $(LDFLAGS) -o $@ $^