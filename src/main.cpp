#include <cstdlib>
#include <exception>
#include <iostream>

#include "OgreApp.h"

using namespace std;

// Ogre
OgreApp ogreApp;

int main(int argc, char** argv)
{

	if (argc == 3 && strcmp(argv[1], "-kernel") == 0) {
		cout << "Calculating kernel with size " << argv[2] << '.' << endl << endl;
		int kernel_size = stoi(argv[2]);
		KernelLoader loader;
		loader.load(kernel_size);
		cout << endl << "Replace the kernel in src/shaders/subsurface.cg with this one." << endl;
		return EXIT_SUCCESS;
	}

	try {
		// Init and start Ogre app
		ogreApp.initApp();
		ogreApp.getRoot()->startRendering();
	}
	catch (const exception& e) {
		cout << e.what() << endl;
	}
	return EXIT_SUCCESS;
}