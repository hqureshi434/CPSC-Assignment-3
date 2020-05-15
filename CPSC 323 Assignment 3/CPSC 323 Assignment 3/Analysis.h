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
	int flag = 0;
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

	void error(string str)
	{
		//Clear output file
		myfile.close();
		myfile.open("Output.txt", ios::out | ios::trunc);
		// Output error message
		myfile << "Error: " << str << " at line: " << lineNumber;
		exit(0);
	}

	//***********Main Code**********************//
	Analysis() {}; //Default Contructor
	Analysis(string outfileName) { //Takes in the name of the output file
		myfile.open(outfileName);  
		myfile << "            Output" << endl;
		myfile << "----------------------------------------------------" << endl;

		if (!file.is_open()) {
			myfile << "Exited";
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
				lineNumber++;
				for (int i = 0; i < 20; i++)
					testCharList[i] = '\0';
				i = 0;


				if (isEquation) {
					for (int x = iteration - 1; x > 0; x--) {
						getInstruction((string)equation[x]);
					}
					Assembly("POPM", getAddr(equation[0]));
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
		if (stackindex != 0) {
			error("Closing argument not found");
		}

		file.close();
		myfile.close();

		symbolTable();
	}

	//***********SYMBOL TABLE CODE**********************//
	/*for instructions that require an input for
	integer value or memory location*/
	void Assembly(string assemblyIns, int val) {
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
	void Assembly(string assemblyIns) {
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
		myfile << "\n\n\t\tSymbol Table\nIdentifier\tMemoryLocation\tType\n";

		for (size_t i = 0; i < idList.size(); i++) {
			myfile << idList.at(i) << "\t\t" << memoryList.at(i) << "\t\t" << typesList.at(i) << endl;
		}
		myfile << endl << endl;
	}

	//------------------------------------------------------------------------------------------------------------

	void lexer(int& j) {
		bool print = false, printline = false, test = false;
		char operators[] = "+-*/%=", separators[] = "'(){}[],.:;!";
		int i;

		//discard all spaces
		while (testChar == ' ')
			testChar = file.get();

		//check if comment
		if (testChar == '!') {
			testChar = file.get();
			while (testChar != '!') {
				testChar = file.get();
			}
			return;
		}

		//Check operator
		for (i = 0; i < 6; ++i)
		{
			if (testChar == operators[i]) {
				//cout << testChar << " is operator\n";
				Operators();
				flag = 1;
				return;
			}
		}

		//Check seprator
		for (int m = 0; m < 11; m++)
		{
			if (testChar == separators[m]) {
				//cout << testChar << " is separator\n";
				Separators();
				flag = 1;
				return;
			}
		}

		//Check the char to see if it is a number
		if (isdigit(testChar))
		{
			char str[5];
			int x = 0;
			testWord[j] = '\0';
			j = 0;
			str[x++] = testChar;

			while (isdigit(file.peek())) {
				testChar = file.get();
				str[x] = testChar;
				x++;
			}

			if (x == 0) {}
			else {
				for (int u = 0; u < x; u++) {
					testWord[j] = '\0';
				}
			}
			flag = 0;
			numbers(str);
			return;
		}

		//If the first character is not operator nor the seperator, get all the rest of word until reach space
		if (isalnum(testChar) || testChar == '$')
		{
			//cout << "IS ALNUM: " << testChar << "\tj = " << j << endl;
			bool stop = false;
			char next;
			if (testChar == ' ' || testChar == '\n')
				stop = true;
			while (!stop) {
				testWord[j++] = testChar;
				if (strcmp(testWord, "endif$") == 0)//competes program @ endif statement
				{
					file.close();
					myfile.close();
					symbolTable();
					exit(EXIT_SUCCESS);
					return;
				}
				next = file.peek();
				for (i = 0; i < 11; ++i)
				{
					if (next == separators[i]) {
						stop = true;
					}
				}
				for (i = 0; i < 6; ++i)
				{
					if (next == operators[i]) {
						stop = true;
					}
				}
				if (!stop)
					testChar = file.get();
				if (testChar == ' ' || testChar == '\n')
					stop = true;
			}

			test = true;
			//Make the rest of c string become null
			testWord[j] = '\0';
			//reset new index of testWord for next testWord
			j = 0;

			if (isKeyword(testWord)) {
				myfile << Keywords();
			}
			else {
				myfile << Identifiers();
			}
			flag = 0;
		}
	}

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

		size_t i = 0;
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

	string numbers(char s[]) { //Numbers - syntaxNum
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
			for (size_t i = 0; i < idList.size(); i++) {
				if (!foundFirst && idList.at(i) == prevVar) {
					Assembly("PUSHM", (5000 + i));
					foundFirst = true;
				}
			}
		}

		equation[iteration][0] = testChar;
		iteration++;

		return s;
	}

	int getAddr(string id) {
		for (size_t i = 0; i < idList.size(); i++) {
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

			Assembly("MULT");
		}
		else if (testChar == '/') {
			Assembly("DIV");
		}
		else if (testChar == '+') {
			Assembly("ADD");
		}
		else if (testChar == '-') {
			Assembly("SUB");
		}
		else if (str == "=") {
			//skip
		}
		else if (isNumber(str)) {
			int val = 0;
			stringstream num(str);
			num >> val;
			Assembly("PUSHI", val);
		}
		else {//is an id
			Assembly("PUSHM", getAddr(str));
		}
	}


	//-------------------OLD CODE--------------------------//
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
