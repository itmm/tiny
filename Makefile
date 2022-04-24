.PHONY: tests clean

APP = tiny
SOURCEs = $(wildcard *.cpp)
OBJECTs = $(addprefix build/,$(SOURCEs:.cpp=.o))

CXXFLAGS += -g -Wall -std=c++17

tests: $(APP)
	@echo "run tests"
	./$(APP) Gcd.mod

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
	@rm -Rf $(APP) build deps expr expr.o
