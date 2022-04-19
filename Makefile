.PHONY: tests clean

APP = calc
SOURCEs = $(wildcard *.cpp)
OBJECTs = $(addprefix build/,$(SOURCEs:.cpp=.o))

CXXFLAGS += -g -Wall -std=c++17

tests: $(APP)
	@echo "run tests"
	./calc "with a: 3*a" | llc -filetype=obj -o=expr.o
	clang -o expr expr.o rtcalc.c
	echo 4 | ./expr

include $(wildcard deps/*.dep)

build/%.o: %.cpp
	@echo "c++ $@"
	@mkdir -p build deps
	@$(CXX) $(CXXFLAGS) -I`llvm-config --includedir` -c $(notdir $(@:.o=.cpp)) -o $@ -MMD -MF deps/$(notdir $(@:.o=.dep))

$(APP): $(OBJECTs)
	@echo "link $@"
	@$(CXX) $(CXXFLAGS) $^ -o $@ `llvm-config --libs support`

clean:
	@echo "clean"
	@rm -Rf $(APP) build deps
