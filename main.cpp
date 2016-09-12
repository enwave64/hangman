/*
Programmer: Elliott Watson
Date: 12/16/13
Project 2
This program is a hangman game
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
using namespace std;

//hangman structure//
struct hangman
{
	static const char head = 'O';
	static const char neck =  '\'';
	static const char body = '|';
	static const char leftLimb = '/';
	static const char rightLimb = '\\';
};
	
//function prototypes//
bool openFileIn(fstream &, string);
void displayBadFileLoad();
void intro();
void initUsed(bool *, int);
void loadArray(fstream &, string *, int);
bool playAnotherGame();
void playGame(const string *, bool *, int);
int randWord (const bool *, int);
void letterScored(char, char *, string, unsigned int &);
void letterFailed(unsigned int  &);
void displayGallows(char *, unsigned int, int);

//main function//
int main()
{

	const int SIZE = 20;
	fstream dataFile;
	string secretWord[SIZE]; //an array for the list of words
	bool usedWord[SIZE] = {false, false, false, false, false, //parallel array holding T or F’s
						   false, false, false, false, false, //used to discard word after each iteration.
						   false, false, false, false, false, 
						   false, false, false ,false, false};
	
	

	if (openFileIn(dataFile, "C:\\Users\\Elliott\\Desktop\\hangman.txt"))
	{
		do
		{
			loadArray(dataFile, secretWord, SIZE);

			intro();
			playGame(secretWord, usedWord, SIZE);

			dataFile.close();
		}
		while(playAnotherGame());
		cout << "\nThanks for playing!\n\n";
	}
	else
		displayBadFileLoad();

	return 0;
}

//modular functions//

//******************************************
//openFileIn is a function that accepts a
//reference to an fstream object. The file
//is opened for input. The function returns
//true upon success, false upon failure.
//******************************************
bool openFileIn(fstream &file, string name)
{
	file.open(name.c_str(), ios::in); //opens the file for input; string is converted to c-string
	if (file.fail())//tests for file's existence
		return false;
	else
		return true;
}

//*************************************************
//dipslayBadFileLoad just displays an error message
//if there is a problem opening the file
//************************************************
void displayBadFileLoad()
{
	cout << "Error loading file!\n";
}
//**************************************************
//intro; banner and intro for game
//**************************************************
void intro()
{
	cout << " _    _                   __  __             \n"
			<< "| |  | |                 |  \\/  |         \n"    
			<< "| |__| | __ _ _ __   __ _| \\  / | __ _ _ __\n"  
			<< "|  __  |/ _` | '_ \\ / _` | |\\/| |/ _` | '_ \\ \n"
			<< "| |  | | (_| | | | | (_| | |  | | (_| | | | |\n"
			<< "|_|  |_|\\__,_|_| |_|\\__, |_|  |_|\\__,_|_| |_|\n"
			<< "                     __/ |  \n"                 
			<< "                    |___/   \n";   
	cout << "\n===== Welcome to Elliott's hangman program =====\n";
	cout << "-You only get 7 incorrect guesses in Elliott's hangman. Good Luck.\n";
}

//******************************************
//loadArray is a function that receives an 
//fstream reference as its arguement. Uses
//a loop to read a number of lines from a file
//and stores each in a string array index
//******************************************
void loadArray(fstream &file, string *array, int size)
{
	for(int counter = 0; counter < size; counter++)
	{
		/*this line gets the next line from the file
		and stores in in the current array position
		the newline character is the delimiter*/
		getline(file, array[counter], '\n');
	}
}
//****************************************	
//playAnotherGame gives I/O for playing
//the game again. Returns a boolean value.
//****************************************
bool playAnotherGame()
{
	char answer;
	cout << "Would you like to play another game?\n";
	cout  << "enter Y for yes N for no\n";

	cin >> answer;
	cin.ignore(100, '\n');

	while (toupper(answer) != 'Y' && toupper(answer) != 'N') //validates input
	{
		cout << "Please enter Y or N only\n";
		cin >> answer;
		cin.ignore(100, '\n');
	}
	if (toupper(answer) == 'Y')
		return true;
	else 
		return false;
}

//*******************************************************
//playGame is the heart of the game. It recieves 
//the secretWord array, the usedWord array, and the const
//size. playGame runs on a do while loop that calls all
//of its sub-functions and handles letter input
//*******************************************************
void playGame(const string *secret, bool *used, int size)
{
	initUsed(used, size);//resets usedWord
	int draw = randWord(used, size); //stores the position value for the secret word
	
	string word, //stores the word for the current game
		   alreadyUsed, //stores each used letter
		   incorrect;  //stores a list of incorrect guesses
	char letter; //for the user's letter choice
	unsigned int scored = 0; //initializes the counter for correct letters
	unsigned int failed = 0; //initializes the counter for incorrect letters
	

	word.insert(0, secret[draw].c_str()); //inserts the word from the array into the word variable
	used[draw] =  true; //flags current word to be skipped for future games
	

	int wordLength = word.length();
	char *wordDisplay;
	wordDisplay = new char[wordLength]; //dynamic array creation, for visual output of the current word
	
	for (unsigned int index = 0; index < word.length(); index++) //initializes with underscores
		wordDisplay[index] = '_';

	//game loop
	do
	{
		displayGallows(wordDisplay, failed, wordLength);
		
		
		//Input and validation block.
		cout << "Please enter a letter\n";
		cout << "If you enter a word or sentence, the first letter will be used.\n";
		cin >> letter;
		cin.ignore(100, '\n');
		while (alreadyUsed.find(tolower(letter)) != std::string::npos || !isalpha(letter))
		{
			if (alreadyUsed.find(tolower(letter)) != std::string::npos) //validates for new letter choice
			{
				cout << "You already used that letter. Try again.\n";
				cin >> letter;
				cin.ignore(100, '\n');
			}
			else if (!isalpha(letter)) //validates for actual letters
			{
				cout << "That is not a letter. Try again.\n";
				cin >> letter;
				cin.ignore(100, '\n');
			}
		}
		
		alreadyUsed.append(1, tolower(letter)); //appends the letter to the used list

	
		//Checks to see if the letter is in the secret word.
		if (word.find(tolower(letter)) != std::string::npos)
			letterScored(letter, wordDisplay, word, scored); //calls letter scored function
		else
		{
			if (failed >= 1) //adds a comma where necessary
				incorrect.append(", ");
			incorrect.append(1, letter); //adds failed letter to incorrect list
			letterFailed(failed); //calls letter failed function
			
		}
		cout << "\nIncorrect: " << incorrect << endl; //displays the incorrect string
	}
	while(scored < word.length() &&  (failed < 7)); //loops until word is complete or man is hung
	displayGallows(wordDisplay, failed, wordLength); //calls the displayGallows function
	
	if(scored == word.length())//game over message
		cout << "\nCongratulations, you won!\n";
	else
		cout << "\nSorry, you lost. Better luck next time.\n";
	delete [] wordDisplay; //erases the dynamically allocated array

}
	

//playGame sub –functions//

//********************************************
//initUsed resets the Used parallel array
//recieves the usedWord array and SIZE
//********************************************
void initUsed(bool *arr, int size)
{
for (int count = 0; count < size; count ++)
	arr[count] = false;
}


//***********************************************
//randWord generates a random integer between 1-19
//and returns the result. Used to randomly draw
//a word from secretWord
//***********************************************
int randWord (const bool *used, int size) //takes const usedWord, size; draws a number 1-19
{
	unsigned seed = time(0); //seeds based on current time
	int roll;
	srand(seed);
	
	do
	{
		roll = rand() % size; //random roll 0-19
	}
	while(used[roll]); //this loop ensures the integer returned is new
	return roll;
	
}
		
//****************************************************
//letterScored handles a correct letter choice. it 
//increments the score variable, and updates the 
//wordDisplay array.
//****************************************
void letterScored(char letter, char *wordDisplay, string word, unsigned int &score)
{
	cout << "\n\n\n\n\n\n\n\n\nNice job, you scored a letter.  The man lives for another day\n";
	for(unsigned int index = 0; index < (word.length()); index++) // checks each array position of the word for the letter
	{
		if (word.c_str()[index] == letter) 
		{
			wordDisplay[index] = letter; //updates wordDisplay and score
			score++;
		}
	}
	

}

//******************************************
//letterFailed hanldes an incorrect choice
//increments fail variable
//*******************************************
void letterFailed(unsigned int  &fail)
{
	cout << "\n\n\n\n\n\n\n\n\n\nUh-oh, things aren't looking good...\n";
	fail++;
}
	
//*********************************************
//displayGallows displays the gallows according
//to the value of the fail variable; also
//displays the wordDisplay array
//***********************************************
void displayGallows(char *wordDisplay, unsigned int fail, int size)
{
	hangman man;
	switch(fail) //switch branches according to value of fail
	{
			case 0 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|\n"
					 << "|\n"           
					 << "|\n|\n|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 1 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|\n"           
					 << "|\n|\n|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 2 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|           " << man.neck << "\n"        
					 << "|\n|\n|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 3 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|          " << man.leftLimb << man.neck << "\n"        
					 << "|\n"
					 << "|\n|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 4 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|          " << man.leftLimb << man.neck << man.rightLimb << "\n"        
					 << "|\n"
					 << "|\n|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 5 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|          " << man.leftLimb << man.neck << man.rightLimb << "\n"        
					 << "|           " << man.body << "\n"
					 << "|\n|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 6 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|          " << man.leftLimb << man.neck << man.rightLimb << "\n"        
					 << "|           " << man.body << "\n"
					 << "|          " << man.leftLimb << "\n"
					 <<"|\n|\n|\n|"
					 << "-------\n" ;
					break;
			case 7 : cout << "-------------\n"         
					 << "|           |\n"
					 << "|           " << man.head << "\n"
					 << "|          " << man.leftLimb << man.neck << man.rightLimb << "\n"        
					 << "|           " << man.body << "\n"
					 << "|          " << man.leftLimb << " " << man.rightLimb << "\n"
					 <<"|\n|\n|\n|"
					 << "-------\n" ;
					break;
			default: cout << "Look Dave, I can see you're really upset about this.\n";
					break;
	}
	cout << "Word:  ";
	for (int i = 0; i < size; i++)
		cout << wordDisplay[i] << " ";
	cout << endl;
}


