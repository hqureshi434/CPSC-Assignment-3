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
#include <sstream>

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
		//Old Variables
		lexeme tool;
		int countWord = 0;
		char currentChar = ' ';
		int col = Ignore;
		int currentState = Ignore;
		int prevState = Ignore;
		string currentWord = "";

		//New Varialbles
		bool print = false, printline = false, test = false;
		char operators[] = "+-*/%=", separators[] = "'(){}[],.:;!";
		int i;

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
							fileWriter << Operators();
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
		string str, filtered;
		char operators[] = "+-*/%";
		//	bool rightHandSide = true, firstOfLHS = true;


			//KEEP THIS CODE
		int i = 0;
		bool found = false;
		while (testWord[i] != NULL) { //filter out $
			if (testWord[i] != '$')
				filtered += testWord[i];
			i++;
		}

		//find if testWord already exists in idList
		i = 0;
		while (!found && i < idList.size()) {
			if (idList.at(i) == filtered) {
				found = true;
			}
			i++;
		}
		if (!found) {//if it doesn't exist in the list, add it
			if (prevType == "")
				error("No declaration given");
			idList.push_back(filtered);
			memoryList.push_back(Memory_Address);
			Memory_Address++;
			typesList.push_back(prevType);
		}


		if (prevVar == "") {
			prevVar = testWord;
		}
		else if (nextVar == "") {
			nextVar = testWord;
		}

		//adds id to equation
		i = 0;
		while (!isblank(testWord[i]) && i < 23) {
			equation[iteration][i] = testWord[i];
			i++;
		}
		iteration++;

		return str;
	}

	string Separators() { //Separators - syntaxSep
		string s = " ";

		char openers[5] = { "([{'" }, closers[] = { ")]}'" }; //Contain the opening and closing separators in their own character arrays
		
		return s;
	}

	string Keywords() { //Keywords - syntaxKey
		string str;

		char parenthesisWords[10][10] = { "if", "while", "for", "forend","function", "main" }; //Keywords that have parenthesis next to them
		
		for (int z = 0; z < 7; z++) {
			if (strcmp(testWord, parenthesisWords[z]) == 0)
				status = true;
		}

		char importantKeys[6][10] = { "int", "bool", "float", "double", "char", "boolean" };
		for (int z = 0; z < 6; z++) {
			if (strcmp(testWord, importantKeys[z]) == 0)
				prevType = testWord;
		}

		for (int a = 0; a < 20; a++) {
			if (strcmp(testWord, keywords[a]) == 0) {
				string key = (string)keywords[a];
			}
		}

		return str;
	}

	string numbers() { //Numbers - syntaxNum
		delay = "";
		for (int i = 0; i < 23; i++) {
			equation[iteration][i] = testWord[i];
		}
		iteration++;
		return "";
	}

	string Operators() { //Operators - syntaxOp
		string s;

		lastOp = testChar;

		bool foundFirst = false;
		bool foundSecond = false;

		//only if operator is an = sign then delay must occur
		if (testChar == '=') {

			delay = "POPM";
			isEquation = true;
		}
		else {//all other operators
			for (int i = 0; i < idList.size(); i++) {
				if (!foundFirst && idList.at(i) == prevVar) {
					assemble("PUSHM", (5000 + i));
					foundFirst = true;
				}
			}
		}

		equation[iteration][0] = testChar;
		iteration++;

		return s;
	}

	int getAddr(string id) {
		for (int i = 0; i < idList.size(); i++) {
			if (id == idList.at(i))
				return memoryList.at(i);
		}
	}

	bool isNumber(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}

	void getInstruction(string str) {
		if (isNumber(str)) {
			int val = 0;
			stringstream num(str);
			num >> val;
		}

		if (testChar == '*') {

			assemble("MULT");
		}
		else if (testChar == '/') {
			assemble("DIV");
		}
		else if (testChar == '+') {
			assemble("ADD");
		}
		else if (testChar == '-') {
			assemble("SUB");
		}
		else if (str == "=") {
			//skip
		}
		else if (isNumber(str)) {
			int val = 0;
			stringstream num(str);
			num >> val;
			assemble("PUSHI", val);
		}
		else {//is an id
			assemble("PUSHM", getAddr(str));
		}
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
