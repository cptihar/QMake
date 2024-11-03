#include "Execute.h"
int main(int argc, char** argv) {
	qmake::Execute ex(argv, argc);
	ex.prepare();
	ex.execute();
	return 0;
}