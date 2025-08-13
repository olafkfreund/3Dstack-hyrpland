PLUGIN_NAME = stack3d
CXX = g++
CXXFLAGS = -shared -fPIC -g -std=c++23 -Wall -Wextra
INCLUDES = -Iinclude `pkg-config --cflags pixman-1 libdrm hyprland`
LIBS = `pkg-config --libs pixman-1 libdrm hyprland`

SRCDIR = src
INCDIR = include
SOURCES = $(wildcard $(SRCDIR)/*.cpp) main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(PLUGIN_NAME).so

$(PLUGIN_NAME).so: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(PLUGIN_NAME).so

install: $(PLUGIN_NAME).so
	cp $(PLUGIN_NAME).so ~/.config/hypr/plugins/

.PHONY: all clean install