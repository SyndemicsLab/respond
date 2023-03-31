#include <iostream>
#include "config.h"
#include "src/Simulation.hpp"

int main(int argc, char **argv) {
	std::cout << "Hello World" << std::endl;
	std::cout << "Version " << RESPOND_VERSION_MAJOR << "." << RESPOND_VERSION_MINOR << std::endl;
	return 0;
}
