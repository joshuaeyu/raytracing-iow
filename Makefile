GLM_FLAGS = -I./glm/include/ -L./glm/lib/ -lglm
main: main.cpp .FORCE
	g++ main.cpp -o main -g -std=c++11 -L./glm/include/ $(GLM_FLAGS)

.FORCE: