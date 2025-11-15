CXX = g++

CPPFLAGS := -DDEBUG -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code \
-Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe \
-fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers \
-Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo \
-Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

CPPSRC := main.cpp tree.cpp Akinator.cpp stack.cpp check_errors.cpp string_functions.cpp onegin.cpp TXLib.cpp

CPPOBJ := $(CPPSRC:%.cpp=build/%.o) 

HEADER_DEPENDS := $(CPPOBJ:%.o=%.d)

EXECUTABLE := tree_prog.exe

.PHONY: all
all: $(EXECUTABLE)

$(CPPOBJ): build/%.o: %.cpp
	@$(CXX) $(CPPFLAGS) -MP -MMD -c $< -o $@

$(EXECUTABLE): $(CPPOBJ)
	@$(CXX) $^ -o $@ $(CPPFLAGS)

-include $(HEADER_DEPENDS)

.PHONY: clean_windows
clean_windows:
	del build\*.o /a /s
	del build\*.d /a /s

.PHONY: clean_linux
clean_linux:
	rm -f build/*.o
	rm -f build/*.d