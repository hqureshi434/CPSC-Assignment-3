#include "SymbolGenerator.h"

int main() {

	myfile.open("FinalOutput.txt");    //makes file named Final Output
	myfile << "            Final Output" << endl;
	myfile << "----------------------------------------------------" << endl;

	if (!file.is_open()) {
		//cout << "Error while opening the file, please change file name to SampleInputFile\n";
		myfile << "EXITED";
		exit(0);
	}

	int i = 0, j = 0;
	while (!file.eof())
	{

		testChar = file.get();
		testCharList[i] = testChar;
		i++;
		if (testChar == '\n')
		{
			prevType = "";
			prevVar = "";
			nextVar = "";
			lineNum++;
			for (int i = 0; i < 20; i++)
				testCharList[i] = '\0';
			i = 0;


			if (isEquation) {
				for (int x = iteration - 1; x > 0; x--) {
					getInstruction((string)equation[x]);
				}
				assemble("POPM", getAddr(equation[0]));
			}

			//reset equation values
			for (int z = 0; z < iteration; z++) {
				memset(equation[iteration], 0, sizeof(equation[iteration]));
			}
			iteration = 0;
			isEquation = false;
		}
		lexer(j);
	}
	if (stackindex != 0)
		error("Closing argument not found");

	file.close();
	myfile.close();

	symbolTable();

	return 0;
}
