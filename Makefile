CXX		=	g++ -g
EXE		=	a.exe
OBJ_DIR	=	objs

all: $(EXE)

$(EXE): main.cpp
	$(CXX) $^ -o $@

obj_dir:
	mkdir -p $(OBJ_DIR)


clean:
	rm -rf $(OBJ_DIR) $(EXE)
	cd ..
