// Omar Ramirez, Francisco Ramirez, and Hammad Qureshi
// CPSC 323
// Assignment 3
//Creating a Syntax Analyzer with the help of Lexical Analyzer.

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <stack>

using namespace std;

//Declaring enums to use to transition through state Table.
enum FSMTransitions {
	Ignore = 0,
	Integer,
	Real,
	String,
	Operator,
	Unknown,
	Space
};
//State Table
int stateTable[7][7] = { {0,  Integer, Real, String, Operator, Unknown, Space},
		     {Ignore, Integer, Real, Ignore, Ignore, Ignore, Ignore},
		     {Real, Real, Unknown, Ignore, Ignore, Ignore, Ignore},
			   {String, String, Ignore, String, String, Unknown, Ignore},
		     {Operator, Ignore, Ignore, String, Ignore, Ignore, Ignore},
			   {Unknown, Unknown, Unknown, Unknown, Unknown, Unknown, Ignore},
			   {Space, Ignore, Ignore, Ignore, Ignore, Ignore, Ignore}};
//Array of key words
char keywords[20][15] = { "int", "float", "bool", "true", "false", "if", "else", "then", "endif", "while",
				  "whileend", "do", "doend", "for", "forend", "input", "output", "and", "or" , "not" };

//Grabbing Input.txt file
ifstream file("Input.txt");
ofstream myfile;

//Struct
struct lexeme
{
	string lex;
	int lexNumber = 0;
	string token; //This will hold text coming from the file
};
//Class structure
class Analysis {
private:
	//For Lexical Analyzer
	lexeme *lexArr = new lexeme[1000000]; //Create a type lexeme array to store data from the text file
	int countWord; //Acts as the index for the lexArr array

	//For Syntax Analyzer
	int stackindex = 0;
	int lineNumber = 1;
	char testChar = ' ';
	char	theStack[20] = " ";
	char testWord[20];
	char testCharList[20];
	bool status; //conditionSet

	//For Symbol Table
	int Memory_Address = 5000;
	string prevType = "";
	vector<string> idList, typesList;
	vector<int> memoryList;
	string lastID = "";
	string prevVar = "", nextVar = "";
	char lastOp;

	string instructions[1000];
	int instructionLine = 1;
	stack <int> s;
	string delay = "";
	char equation[10][23];
	int iteration = 0;
	bool isEquation = false;

public:
	Analysis() {}; //Default Contructor

	Analysis(string filename, string outputFile) {
		//Variables
		lexeme tool;
		int countWord = 0;
		char currentChar = ' ';
		int col = Ignore;
		int currentState = Ignore;
		int prevState = Ignore;
		string currentWord = "";


		//File objects
		fstream file(filename, ios::in); //This will read in the file
		ofstream fileWriter; //Created so we can write the output to a separate file

		fileWriter.open(outputFile); //This will create a new file to write the output to
		fileWriter << "*******Syntax Analyzer*******\n";

		if (!file.is_open()) {
			cout << "Cannot open file";
		}
		else {
			while (!file.eof()) {

				file >> lexArr[countWord].token; /*Stores each word and character as a string from the file
										 into the struct lexeme under the variable*/
				file >> ws;
				fileWriter << "\n";

				currentWord = lexArr[countWord].token;//Gets the word from the struct array and sets it to a string

				for (size_t i = 0; i < currentWord.length(); i++) {
					currentChar = currentWord[i]; //Grab each character from the word that came from the array
					col = getCharState(currentChar); //This will return the transition type for the current character

					currentState = stateTable[currentState][col]; //Get the current state from the current word

					if (currentState != Ignore) {
						tool.token = currentWord;

						if (prevState == Ignore) {
							tool.lexNumber = currentState;
						}
						else {
							tool.lexNumber = prevState;
						}

						tool.lex = lexName(tool.lexNumber);
						fileWriter << "Token: " <<  tool.lex  << "		" << "Lexeme: " << tool.token  << "\n"; //Write the results to the text file
						string compareWord = tool.lex;

						//Have a series of if statements that compares currentWord.
						if(compareWord.compare("String") == 0) //If the lexer identifies a token as a String then use the separators function
						{
							fileWriter << Separators();


						}
						else if(compareWord.compare("Operator") == 0) //If the lexer identifies a token as an operator then use the oparators function
						{
							fileWriter << operators();
						}
						else if(compareWord.compare("Space") != 0) //If the lexer identifies a token as a Space then use the separators function
						{
							compareWord = Separators();
							fileWriter << compareWord;
						}
						else if(compareWord.compare("Integer") != 0) //If the lexer identifies a token as an integer then use the numbers function
						{ 
							compareWord = numbers();
							fileWriter << compareWord;
						}
						currentWord = "";
					}
					else {
						currentWord += currentChar;
						i++;
					}
					prevState = currentState;
				}
				countWord++;
			}
		}
		fileWriter.close();
		file.close();
	}

	void error(string str)
	{
		//Clear output file
		myfile.close();
		myfile.open("Output.txt", ios::out | ios::trunc);
		// Output error message
		myfile << "Error: " << str << " at line: " << lineNumber;
		exit(0);
	}

	//***********SYMBOL TABLE CODE**********************//
	/*for instructions that require an input for
	integer value or memory location*/
	void assemble(string assemblyIns, int val) {
		myfile << instructionLine;
		if (assemblyIns == "PUSHI") {
			myfile << "	PUSHI	";
		}
		else if (assemblyIns == "PUSHM") {
			myfile << "	PUSHM	";
		}
		else if (assemblyIns == "POPM") {
			myfile << "	POPM		";
		}
		else if (assemblyIns == "JUMPZ") {
			myfile << "	JUMPZ	";
		}
		else if (assemblyIns == "JUMP") {
			myfile << "	JUMP		";
		}
		else {
			error("Assembly Instruction not valid");
		}
		instructionLine++;
		myfile << val << endl;
	}


	//for instructions that require no input
	void assemble(string assemblyIns) {
		myfile << instructionLine;
		if (assemblyIns == "STDOUT") {
			myfile << "	STDOUT";
		}
		else if (assemblyIns == "STDIN") {
			myfile << "	STDIN";
		}
		else if (assemblyIns == "ADD") {
			myfile << "	ADD";
		}
		else if (assemblyIns == "SUB") {
			myfile << "	SUB";
		}
		else if (assemblyIns == "MUL") {
			myfile << "	MUL";
		}
		else if (assemblyIns == "DIV") {
			myfile << "	DIV";
		}
		else if (assemblyIns == "GRT") {
			myfile << "	GRT";
		}
		else if (assemblyIns == "LES") {
			myfile << "	LES";
		}
		else if (assemblyIns == "EQU") {
			myfile << "	EQU";
		}
		else if (assemblyIns == "NEQ") {
			myfile << "	NEQ";
		}
		else if (assemblyIns == "GEQ") {
			myfile << "	GEQ";
		}
		else if (assemblyIns == "LEQ") {
			myfile << "	LEQ";
		}
		else if (assemblyIns == "LABEL") {
			myfile << "	LABEL";
		}
		else {
			error("Assembly Instruction not valid");
		}
		instructionLine++;
		myfile << endl;
	}

	void symbolTable()
	{
		myfile << "\n\n\t\tSYMBOL TABLE\nIdentifier\tMemoryLocation\tType\n";

		for (int i = 0; i < idList.size(); i++) {
			myfile << idList.at(i) << "\t\t" << memoryList.at(i) << "\t\t" << typesList.at(i) << endl;
		}
		myfile << endl << endl;
	}

	//------------------------------------------------------------------------------------------------------------


	//***********SYNTAX ANALYZER CODE**********************//
	//Does a comparison with character to the keyword array to see if there is a keyword

	bool isKeyword(char input[]) { //If a word is a keyword return true
		for (int i = 0; i < 32; ++i) {
			if (strcmp(keywords[i], input) == 0) {
				return true;
			}
		}
		return false;
	}

	string Identifiers() { //Identifiers - syntaxID
		string a;
		char operators[] = "+-*/%";
		bool RHS = true; //rightHandSide
		bool firstOfLHS = true; //firstofLHS
		for (int i = 0; i < strlen(testCharList); i++)
		{
			if (testCharList[i] == '=') { RHS = false; }
		}

		if (!RHS)
		{
			for (int i = 0; i < strlen(testCharList); i++)
			{
				for (int j = 0; j < 5; j++) {
					if (testCharList[i] == operators[j]) {firstOfLHS = false;}
				}
			}
			if (firstOfLHS){ myfile << "<Expression> -> <Term> <Expression Prime>\n"; }

			myfile << "<Term> -> <Factor><TermPrime>\n";
			myfile << "<Factor> -> <Identifier>\n";
		}
		else
		{
			myfile << "<Statement> -> <Assign>\n";
			myfile << "<Assign> -> <Identifier> = <Expression>\n";
		}
		return a;
	}

	string Separators() { //Separators - syntaxSep
		string phrase = " <Separator> -> ";

		char openers[5] = { "([{'" }, closers[] = { ")]}'" }; //Contain the opening and closing separators in their own character arrays
		for (int a = 0; a < 4; a++) { //Will iterate through all the elements in both arrays
			if (testChar == openers[a] && theStack[stackindex] != openers[a]) {//know testChar is a closing separator
				stackindex++;
				theStack[stackindex] = testChar;//add separator to the stack

				if (testChar == '(') {
					phrase += " <Condition>\n";
					phrase += " <Condition> -> <StatementList>";
				}
				else {
					phrase += " <OpeningSeparator> <StatementList>\n";
					phrase += " <OpeningSeparator> -> " + testChar;
				}
				return phrase;
			}
			else if (testChar == closers[a]) {//know testChar is a closing separator
				if (openers[a] == theStack[stackindex]) {

					theStack[stackindex] = ' ';
					stackindex--;
					if (testChar == ')') {
						phrase += " <Condition>\n";
						phrase += " <Condition> -> <StatementList> )";
						phrase += "\n <StatementList> -> Epsilon";
					}
					else {
						phrase += " <StatementList> <ClosingSeparator>\n";
						phrase += " <ClosingSeparator> -> " + testChar;
					}
					return phrase;
				}
			}
		}
		phrase += " <EndSeparator>\n";
		return phrase;
	}

	string Keywords() { //Keywords - syntaxKey
		string phrase;

		char parenthesisWords[10][10] = { "if", "while", "for", "forend","function", "main" }; //Keywords that have parenthesis next to them
		for (int i = 0; i < 7; i++) {
			if (strcmp(testWord, parenthesisWords[i]) == 0)
				status = true;
		}

		for (int i = 0; i < 20; i++) {
			if (strcmp(testWord, keywords[i]) == 0) { //Compare the currently read in word to the keywords array
				string key = (string)keywords[i]; //Typecast the current index in keywords array as a string and set it to key 

				if (i < 3) { //is a variable
					phrase += " <KeyWord> -> <Variable>\n";
					phrase += " <Variable> -> <" + key + ">";
				}
				else if (i > 2 && i < 13) {//is a conditional
					phrase += " <KeyWord> -> <Conditional>\n";
					phrase += " <Conditional> -> <" + key + "> + <Separator>";
				}
				else {//is a function
					phrase += " <KeyWord> -> <Function>\n";
					phrase += " <Function> -> <" + key + ">";
				}
			}
		}
		return phrase;
	}

	string numbers() { //Numbers - syntaxNum
		string num;
		num = " <Number> -> <Assign>";
		num = " <Assign> -> <" + (string)testWord + ">";
		return num;
	}

	string operators() { //Operators - syntaxOp
		string ops;
		if (testChar == '*') {
			myfile << " <TermPrime> -> * <Factor> <TermPrime>\n";
			myfile << " <ExpressionPrime> -> <Empty>\n";
		}
		else if (testChar == '/') {
			myfile << " <TermPrime> -> / <Factor> <TermPrime>\n";
			myfile << " <ExpressionPrime> -> <Empty>\n";
		}
		else if (testChar == '+') {
			myfile << " <TermPrime> -> <Empty>\n";
			myfile << " <ExpressionPrime> -> + <Term> <ExpressionPrime>\n";
		}
		else if (testChar == '-') {
			myfile << " <TermPrime> -> <Empty>\n";
			myfile << " <ExpressionPrime> -> - <Term> <ExpressionPrime>\n";
		}
		myfile << " <Empty>->Epsilon\n";

		return ops;
	}

	//***********LEXICAL ANALYZER CODE*********************//
	//Notes: Group Keywords and Identifiers as strings in one loop then in a nested loop do a comparision with the strings to determine if they are
	//keywords with the array. If there are any strings left that are not keywords then they will be labeled as idenitifiers.

	//This function determines the state of the character being read
	int getCharState(char currentC) {
		//whitespace
		if (isspace(currentC))
		{
			return Space;
		}

		//Checks for integers
		else if (isdigit(currentC))
		{
			return Integer;
		}

		//Checks for real numbers
		else if (currentC == '.')
		{
			return Real;
		}

		//Checks for operators
		else if(ispunct(currentC))
		{
			return Operator;
		}

		//Check for Strings then do a comparison to see if it is a keyword or identifier
		else if(isalpha(currentC))
		{
			return String;
		}

		else
		{
			return Unknown;
		}
	}

	string lexName(int lexeme) { //String function that uses a switch statement to return what a certain token is
		switch (lexeme) {
		case String:
			return "String";
			break;
		case Real:
			return "Real";
			break;
		case Integer:
			return "Integer";
			break;
		case Operator:
			return "Operator";
			break;
		case Unknown:
			return "Unknown";
			break;
		case Space:
			return "Space";
			break;
		default:
			return "Error";
			break;
		}
	}
};
