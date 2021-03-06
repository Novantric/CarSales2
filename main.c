
#pragma region Declarations
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdio.h> //basic input output libraries
	#include <string.h> //string manipulation tools
	#include <stdlib.h> //file manipulaiton
	#include <errno.h> //error code handling
	#include "main.h" //added automatically for some reason
	#include <ctype.h> //checking if numbers are in a string

	#define bool unsigned short //to save on memory rather than being an int, increases compatability/reduces warnings

	FILE* file;
#pragma endregion Declarations

#pragma region Constants
	#define MAX_SALES 10 //max number of customers
	
	//car discounts
	#define DISCOUNT_MIN_AGE 18
	#define DISCOUNT_MAX_AGE 23
	#define DISCOUNT_PERCENTAGE 0.25f //25% off if condictions met

	//menu options
	#define MENU_BUY 'a'
	#define MENU_VIEW 'b'
	#define MENU_EXIT 'x'

	//boolean values
	#define TRUE 1
	#define FALSE 0

	//files
	#define CSV_FILE_LOCATION "CarSales2Data.csv" //ORIGINAL: D:\\data.csv

	#define FILE_OPENED 0
	#define FILE_CLOSED 1
	#define FILE_ERROR 2

	//colours!
	#define NORMAL  "\x1B[0m"
	#define RED  "\x1B[31m"
	#define GREEN  "\x1B[32m"
	#define YELLOW  "\x1B[33m"
	#define BLUE  "\x1B[34m"
	#define MAGENTA  "\x1B[35m"
	#define CYAN  "\x1B[36m"
	#define WHITE  "\x1B[37m"
#pragma endregion Universal Constants

#pragma region Variables
	//arrays, used throughout the program and store the transaction history
	int numberOfTransactions = 0; //INDEX

	float totalSales[MAX_SALES]; //holds value of each transaction
	unsigned short numCarsSold[MAX_SALES]; //the number of cars a user buys
	unsigned short discountGivenPerSale[MAX_SALES]; //tracks if a discount was given
	char customerNames[MAX_SALES][101]; //stores customer names up to 100 chars long
	unsigned short typeOfCarSale[MAX_SALES]; //tracks the type of car sold per sale

	float carPrices[] = { 5999.99f, 10000.0f, 15999.99f, 65000.50 }; //price of each car
	char carTypes[][10] = { "Ford", "Fiat", "Subaru", "Mazda" }; //name of each car

	//The following keep track of basic metrics that aren't covered by arrays
	unsigned short carsAvalible = 20, totalNumCarsSold = 0;
	float totalProfit = 0;
	int numberOfCarsInStock = sizeof(carPrices) / sizeof(float); //calculate the amount of car types

	//files
	unsigned char currentFileStatus = FILE_CLOSED;
	
	//Error handling
	extern int errno;
#pragma endregion Universal Variables

#pragma region Functions
	#pragma region Misc
		void clearScreen()
		{
			system("cls");
		}
		void waitForInput(unsigned short userChoice)
		{
			//1 = one getChar; BUY, file operation errors
			//2 = two getChars; MENU
			//3 = menu; return to
			//4 = records menu; return to
			//5 = menu; exit

			switch (userChoice)
			{
			case 1:
				printf("\n>>> ");
				(void)getchar();
				break;
			case 2:
				printf("\n>>> ");
				(void)getchar();
				(void)getchar();
				break;
			case 3:
				printf("Press enter to return to the main menu.\n>>> ");
				(void)getchar();
				break;
			case 4:
				printf("Press enter to return to the records menu.\n>>> ");
				(void)getchar();
				break;
			case 5:
				printf("Thanks for using the program!\nPress enter to quit.\n>>> ");
				(void)getchar();
				break;
			}
		}
		void swapUnsShort(unsigned short* a, unsigned short* b)
		{
			unsigned short temp;
			temp = *a;
			*a = *b;
			*b = temp;
		}
		void swapBool(bool* a, bool* b)
		{
			bool temp;
			temp = *a;		
			*a = *b;
			*b = temp;
		}
		void setColour(char colour)
		{
			switch (colour)
			{
				case '0':
					printf("%s", NORMAL);
					break;
				case 'r':
					printf("%s", RED);
					break;
				case 'm':
					printf("%s", MAGENTA);
					break;
				case 'y':
					printf("%s", YELLOW);
					break;
				case 'g':
					printf("%s", GREEN);
					break;
				case 'b':
					printf("%s", BLUE);
					break;
				case 'c':
					printf("%s", CYAN);
					break;
				case 'w':
					printf("%s", WHITE);
					break;
			}
		}
		bool hasNumbersOnly(const char* s)
		{
			//Source: https://stackoverflow.com/questions/14422775/how-to-check-a-given-string-contains-only-number-or-not-in-c
			while (*s) {
				if (isdigit(*s++) == 0) return FALSE;
			}
			return TRUE;
		}
		void printNewLine()
		{
			printf("\n");
		}
	#pragma endregion Misc
	#pragma region Inputs
			char getCharInput(char message[201])
			{
				char charResult;
				char userInputStr[201];
				int userInputStrLen;

				printf("%s\n>>> ", message);
				do
				{
					fgets(userInputStr, sizeof(userInputStr), stdin);
					userInputStrLen = (int)(strlen(userInputStr) - (size_t)1);

					if (userInputStr[0] != '\n' && userInputStrLen != 0)
					{
						charResult = userInputStr[0];
						return charResult;
					}
					else
					{
						printf("\n>>> ");
					}
				} while (1);
			}	
			unsigned short getUnsShortInput(char message[201], unsigned short maxValue, unsigned short minValue)
			{
				char checkString[201];
				unsigned short checkShort;

				printf("%s\n>>> ", message);
				do
				{
					(void)scanf("%s", checkString);
					if (hasNumbersOnly(checkString) == TRUE)
					{
						checkShort = (unsigned short)(atoi(checkString));

						if (checkShort > maxValue)
						{
							printf("Please enter a value lower than or equal to ");
							setColour('r');
							printf("%hd", maxValue);
							setColour('0');
							printf(".\n>>> ");
						}
						else if (checkShort < minValue)
						{
							printf("Please enter a value greater than or equal to ");
							setColour('r');
							printf("%hd", minValue);
							setColour('0');
							printf(".\n>>> ");
						}
						else
						{
							return checkShort;
						}
					}
					else
					{
						printf("Please enter a valid number.\n>>> ");
					}
				} while (TRUE);
			}
			void getStringInput(char message[201], char *userInput)
			{
				int strLen;

				printf("%s\n>>> ", message);
				do
				{
					fgets(userInput, sizeof(userInput), stdin);
					if (userInput[0] != '\n')
					{
						//start of input validation
						if ((strstr(userInput, "string") != NULL))
						{
							printf("Please enter a valid name.\n>>> ");
						}
						else
						{
							strLen = (int)(strlen(userInput) - (size_t)1); //prevents warning about not converting from int to size_t
							userInput[strLen] = '\0'; //sets the last char as null instead of \n
							break;
						}	
					}
					else
					{
						printf(">>> "); //always runs at least once
					}
				} while (1);
			}
	#pragma endregion Inputs
	#pragma region Files
		FILE *createFile(char fileName[201])
		{
			file = fopen(fileName, "w");

			//if succeeds, closes the file
			if (file != NULL)
			{
				fclose(file);
			}
			return file;
		}
		void openFile(char fileName[201], char operationMode[4])
		{
			file = fopen(fileName, operationMode);

			if (file == NULL)
			{
				//if cannot open file, make a new file with the filename provided
				if (createFile(fileName) == NULL)
				{
					currentFileStatus = FILE_ERROR;
					setColour('r');
					printf("Error opening %s: %s", fileName, strerror(errno));
					setColour('0');
				}
				else
				{
					openFile(fileName, operationMode);
				}
			}
			else
			{
				currentFileStatus = FILE_OPENED;
			}
		}
		void closeFile()
		{
			//close the file only if it's open
			if (currentFileStatus == FILE_OPENED)
			{
				fclose(file);
				currentFileStatus = FILE_CLOSED;
			}
		}
		void readFile()
		{
			int fileLineCounter = 0;

			while (1)
			{
				//reset the vars after every loop
				unsigned short carAmountPerSaleValue = 0, typeOfCarPerSaleValue = 0, discountGivenPerSaleValue = 0;
				char customerNameValue[201] = "";

				int scanResult = fscanf(
					file, //file stream
					"%hd,%hd,%hd,%[^\n]s", //format of the csv line
					&carAmountPerSaleValue, //vars, one for each placeholder in the format above
					&typeOfCarPerSaleValue,
					&discountGivenPerSaleValue,
					&customerNameValue
					);

				//when the file ends
				if (scanResult == EOF) 
				{
					break;
				}

				//save the read data into the correct arrays
				numCarsSold[fileLineCounter] = carAmountPerSaleValue;
				typeOfCarSale[fileLineCounter] = typeOfCarPerSaleValue;
				discountGivenPerSale[fileLineCounter] = (bool)discountGivenPerSaleValue;
				strcpy(customerNames[fileLineCounter], customerNameValue);

				//Updating total profit (price of car type * how many sold * discount
				if (discountGivenPerSale[fileLineCounter] == TRUE)
				{
					totalProfit += carPrices[typeOfCarSale[fileLineCounter]] * numCarsSold[fileLineCounter] * (1 - DISCOUNT_PERCENTAGE);
				}
				else
				{
					totalProfit += carPrices[typeOfCarSale[fileLineCounter]] * numCarsSold[fileLineCounter];
				}

				carsAvalible -= numCarsSold[fileLineCounter];

				//increment and iterate
				fileLineCounter++;
			}

			//update number of transactions
			numberOfTransactions = fileLineCounter;
			
			printf("\nFile read successfully!");
		}
		void getFileData()
		{
			printf("Opening data file...");
			openFile(CSV_FILE_LOCATION, "r");

			if (currentFileStatus == FILE_OPENED) {
				
				printf("\nFile found, reading file...");
				readFile();
			}
			else if (currentFileStatus == FILE_ERROR)
			{
				setColour('r');
				printf("There was an error trying to open the file at: %s\nIf you're seeing this, this file likely doesn't exist yet.", CSV_FILE_LOCATION);
				setColour('0');
				waitForInput(1);
			}
			closeFile();
		}
		void writeDataToFile() {
			//FORMAT:
			// #cars sold, carType, Discount?, Name 
			// 2,0,1,Harry

			for (int i = 0; i < numberOfTransactions; i++) {

				char line[201]; //saved to the file
				char data[50]; //temporary, changed for each variable

				//number of cars sold
				_itoa((int)numCarsSold[i], data, 10); //convert from deanary to string, add to data				
				strcpy(line, data); //copy to the line
				strcat(line, ","); //seperate vars on the line

				//car type
				_itoa((int)typeOfCarSale[i], data, 10);
				strcat(line, data);
				strcat(line, ",");

				//discount given
				_itoa((int)discountGivenPerSale[i], data, 10);
				strcat(line, data);
				strcat(line, ",");

				//customer name
				strcat(line, customerNames[i]);

				//save the line
				fprintf(file, line);

				//if this is the last value
				if (i < numberOfTransactions - 1) {
					fprintf(file, "\n");
				}
			}
		}
		void saveFile()
		{
			openFile(CSV_FILE_LOCATION, "w");

			if (currentFileStatus == FILE_OPENED) {
				writeDataToFile();
			}
			else if (currentFileStatus == FILE_ERROR)
			{
				setColour('r');
				printf("There was an error when writing to the file at: %s\n", CSV_FILE_LOCATION);
				setColour('0');
			}
			closeFile();
		}
	#pragma endregion Files
	#pragma region Sorting Arrays
		void sortArrays_NoCarsSoldPerSale() {
			//perform a bubble sort
			for (int i = 0; i < numberOfTransactions - 1; i++) {
				for (int j = i + 1; j < numberOfTransactions; j++) {

					// check if the value at position i is greater than the value at position j
					if (numCarsSold[i] > numCarsSold[j]) {

						//swap if true, ticketAmountPerSale
						swapUnsShort(&numCarsSold[i], &numCarsSold[j]);

						//swap typeOfTicketPerSale
						swapUnsShort(&typeOfCarSale[i], &typeOfCarSale[j]);

						//swap discountGivenPerSale 
						swapBool(&discountGivenPerSale[i], &discountGivenPerSale[j]);

						//swap customerNames
						char temp[201];
						strcpy(temp, customerNames[i]);
						strcpy(customerNames[i], customerNames[j]);
						strcpy(customerNames[j], temp);

					}
				}
			}
		}
		void sortArrays_PrintAtPos(int position)
		{
			int typeOfCar = typeOfCarSale[position];
			float price = numCarsSold[position] * carPrices[typeOfCar];
			char discountGivenText[4]; //allows to print if a disount was given as text

			// if a discount was given, then...
			if (discountGivenPerSale[position] == TRUE) {
				strcpy(discountGivenText, "Yes");
				price *= (1 - DISCOUNT_PERCENTAGE);
			}
			else
			{
				strcpy(discountGivenText, "No");
			}

			printf("%d\t%s\t\t%s\t\t%d\t", position + 1, customerNames[position], carTypes[typeOfCar], numCarsSold[position]);
			printf("%c%.2f\t%c%.2f\t", 156, price, 156, carPrices[typeOfCar]); //Matching format as rest of program

			if (discountGivenPerSale[position] == TRUE)
			{
				setColour('g');
				printf("%s\n", discountGivenText);
			}
			else
			{
				setColour('r');
				printf("%s\n", discountGivenText);
			}
			setColour('0');
		}
		void sortArrays_PrintHeadings()
		{
			setColour('c');
			printf("ID\tName\t\tCar Type\t#Cars\tCost\t\tCost/Car\tDiscount?\n");
			setColour('0');
		}
		void sortArrays_BetweenMinMax(unsigned short min, unsigned short max) {
			bool foundValue = FALSE;

			for (int i = 0; i < numberOfTransactions; i++) {

				if (min <= numCarsSold[i] && numCarsSold[i] <= max)
				{
					//only prints headings if no prior results have been found
					if (foundValue == FALSE) 
					{
						setColour('c');
						printf("\nShowing data in the range %hd - %hd:\n", min, max);
						setColour('0');
						sortArrays_PrintHeadings();
					}
					sortArrays_PrintAtPos(i);
					foundValue = TRUE;
				}
			}
			if (foundValue == FALSE)
			{
				setColour('r');
				printf("No data was found.\n");
				setColour('0');
			}
			printNewLine();
		}
		void sortArrays_LookForFirstChar(char searchChar)
		{
			bool foundValue = FALSE;

			for (int i = 0; i < numberOfTransactions; i++)
			{

				if (customerNames[i][0] == searchChar)
				{
					//only prints headings if no prior results have been found
					if (foundValue == FALSE)
					{
						setColour('c');
						printf("\nShowing matching data:\n");
						setColour('0');
						sortArrays_PrintHeadings();
					}
					sortArrays_PrintAtPos(i);
					foundValue = TRUE;
				}
			}
			if (foundValue == FALSE)
			{
				setColour('r');
				printf("No data was found.\n");
				setColour('0');
			}
			printNewLine();
		}
	#pragma endregion Sorting Arrays
	#pragma region Menu Options
		#pragma region Buy
			bool buy_CheckForDiscount(unsigned short userAge)
			{
				if (userAge >= DISCOUNT_MIN_AGE && userAge <= DISCOUNT_MAX_AGE)
				{
					(void)getchar(); //prevents not being able to see the recipt if true
					return TRUE; //gives a discount if the user's age falls within a specified value
				}
				else
				{
					char hasNUSCard;
					(void)getchar();
					do
					{
						hasNUSCard = getCharInput("Do you own a NUS card? y/n"); //can only have one discount
						if (hasNUSCard == 'y' || hasNUSCard == 'Y')
						{
							return TRUE;
						}
						else if (hasNUSCard == 'n' || hasNUSCard == 'N')
						{
							return FALSE;
						}
						else
						{
							printf("Please enter a valid value.\n");
						}
					} while (1);					
				}
			}
			float buy_ApplyDiscount(float currentPrice)
			{
				return currentPrice * (1 - DISCOUNT_PERCENTAGE);
			}
			void buy_PrintDiscount(unsigned short discountGiven)
			{
				unsigned short discountInt = (unsigned short)(DISCOUNT_PERCENTAGE * 100);

				switch (discountGiven)
				{
				case TRUE:
					printf("\nCongrats, you're eligible for a discount of %d Percent!\n", discountInt);
					break;
				default:
					printf("\nYou were not eligible for a discount.\n");
					break;
				}
			}
			void buy_Recipt(float finalPrice, unsigned short numCarsBrought, unsigned short discountGiven)
			{
				setColour('g');
				printf("~~~~~~~~~~~Thank you for your business!~~~~~~~~~~~\n");
				setColour('0');
				printf("The final sum is %c%.2f", 156, finalPrice);
				buy_PrintDiscount(discountGiven);
				printf("\nThere are %hd remaining Cars that can be purchased.", carsAvalible);
				printf("\nHave a good day, my good man. Please come again.");
				setColour('b');
				printf("\n\nYou obtained %hd Cars!!\n", numCarsBrought);
				setColour('0');

				waitForInput(3);
				clearScreen();
			}
			void buy_showCarTypes()
			{
				printf("Car Types:\n");
				setColour('c');
				printf("ID\tName\tPrice\n");
				setColour('0');
				for (int i = 0; i < numberOfCarsInStock; i++)
				{
					printf("%d\t%s\t%c%.2f\n", i, carTypes[i], 156, carPrices[i]);
				}

			}
			unsigned short buy_returnNumberCarTypes()
			{
				unsigned short carCounter = 0;
				for (int i = 0; i < numberOfCarsInStock; i++)
				{
					carCounter += 1;
				}
				return carCounter - 1; //first ID is always 0
			}
			void menuChoice_BuyTickets()
			{
				//Vars
				unsigned short numCarsBrought = 0, userAge = 0, carType;
				unsigned short discountGiven = FALSE;
				float finalSumPrice = 0.0;
				unsigned char hasNUSCard = 0;

				//check if cars are availible
				if (carsAvalible <= 0 )
				{
					setColour('r');
					printf("Sorry, but we're all sold out! Please come again!\n");
					setColour('0');
					//(void)getchar();
					waitForInput(3);
					return; //sends you back to the menu
				}

				//Name and cars needed
				getStringInput("What is your Name?", &customerNames[numberOfTransactions]);

				//Enough Cars?
				printf("There are ");
				setColour('r');
				printf("%hd", carsAvalible);
				setColour('0');
				printf(" cars currently availble.\n");

				unsigned short carTemp = 0;
				carTemp = getUnsShortInput("Please enter how many you would like to purchase.\nAlternatively, enter 0 to return to the main menu.", carsAvalible, 0);

				if (carTemp == 0)
				{
					(void)getchar(); //catches the newline character so it isnt passed into the menu
					return;
				}
				else
				{
					numCarsBrought = carTemp;
				}

				//Car Wanted
				buy_showCarTypes();
				unsigned short numCarsCounted = buy_returnNumberCarTypes();
				carType = getUnsShortInput("Please enter an ID from the list above!", numCarsCounted, 0);

				//Discount and price
				finalSumPrice = numCarsBrought * carPrices[carType]; //calculate the price

				userAge = getUnsShortInput("How old are you?", 130, 10);
				discountGiven = buy_CheckForDiscount(userAge);

				if (discountGiven == TRUE)
				{
					finalSumPrice = buy_ApplyDiscount(finalSumPrice);
				}
				discountGivenPerSale[numberOfTransactions] = discountGiven; //record wether or not the user got a discount

				//update array stuff for checking later
				typeOfCarSale[numberOfTransactions] = carType;
				carsAvalible -= numCarsBrought; //update the number of cars that can be bought
				totalProfit += finalSumPrice; //tracks the profit earnt
				totalNumCarsSold += numCarsBrought; //tracks the amount of cars sold
				totalSales[numberOfTransactions] = finalSumPrice; //tracks the money spent by this user
				numCarsSold[numberOfTransactions] = numCarsBrought; //tracks how many cars this user brought
				numberOfTransactions++; //tracks the number of transactions

				//Finish
				buy_Recipt(finalSumPrice, numCarsBrought, discountGiven);
			}
		#pragma endregion Buy
		#pragma region Records
			void menuChoice_viewSalesData()
			{
				sortArrays_NoCarsSoldPerSale();

				//tracks the number of cars sold
				unsigned int numberOfCarsCounter = 0;

				if (numberOfTransactions == 0)
				{
					setColour('r');
					printf("No data was found.\n");
					setColour('0');
				}
				else
				{
					sortArrays_PrintHeadings();
					for (int i = 0; i < numberOfTransactions; i++)
					{
						int typeOfTicket = typeOfCarSale[i];
						float price = numCarsSold[i] * carPrices[typeOfTicket];
						sortArrays_PrintAtPos(i);

						numberOfCarsCounter += numCarsSold[i]; //track number of cars sold
					}

					printf("\nTotal profit: %c%.2f.", 156, (double)totalProfit);
					printf("\n%hd cars have been sold, and there are %hd cars remaining.\n", numberOfCarsCounter, carsAvalible);
				}
				waitForInput(4);
			}
			void menuChoice_viewSalesBetween()
			{
				sortArrays_NoCarsSoldPerSale();

				printf("Please enter the desired number of cars sold to filter by.\n");
				unsigned short minTicketsSold = getUnsShortInput("Minimum:", numberOfCarsInStock - 1, 0);
				unsigned short maxTicketsSold = getUnsShortInput("Maximum: (Enter 0 for no limit)", numberOfCarsInStock, 0);
				if (maxTicketsSold == 0)
				{
					maxTicketsSold = 65535; //max value an unsigned short can take without overflowing
				}
				sortArrays_BetweenMinMax(minTicketsSold, maxTicketsSold);
				waitForInput(4);
				(void)getchar();
			}
			void menuChoice_viewSalesNames()
			{
				char filterChar = getCharInput("Please enter the first letter that you want to filter by.");
				sortArrays_LookForFirstChar(filterChar);
				waitForInput(4);
			}
		#pragma endregion Records
	#pragma endregion Menu Options	
	#pragma region Menu
			void menu_greeting()
			{
				printf("Welcome to Noven's used Car Emporium!\n");
			}
			char menu_menu()
			{
				setColour('y');
				printf("Please choose what service you'd like!\n");
				setColour('0');

				printf("a - Buy a Car!\n");
				printf("b - View Sales Statistics\n");
				printf("x - Quit the Program");

				char menuChoice = getCharInput("\0");
				clearScreen();
				return menuChoice;
			}
			void menu_records()
			{
				bool loop = TRUE;
				char recordsChoice;
				do
				{
					clearScreen();

					setColour('y');
					printf("How would you like to view the data?\n");
					setColour('0');
					printf("a - View all data\n");
					printf("b - Sort by number of cars sold\n");
					printf("c - Sort by name of customer\n");
					printf("x - Return to the main menu");

					recordsChoice = getCharInput("\0");
					clearScreen();

					switch (recordsChoice)
					{
					case 'a':
						menuChoice_viewSalesData();
						break;
					case 'b':
						menuChoice_viewSalesBetween();
						break;
					case 'c':
						menuChoice_viewSalesNames();
						break;
					case 'x':
						loop = FALSE;
						break;
					default:
						setColour('r');
						waitForInput(3);
						setColour('0');
						break;
					}
				} while (loop == TRUE);
			}
	#pragma endregion Menu
#pragma endregion Functions

void main()
{
	char menuChoice = 0;

	getFileData();
	menu_greeting(); //gets hidden by something
	do
	{
		clearScreen();
		menuChoice = menu_menu();

		//Menu Options
		switch (menuChoice)
		{
			case MENU_BUY:
				menuChoice_BuyTickets();
				break;
			case MENU_VIEW:
				menu_records();
				break;
			case MENU_EXIT:
				saveFile();
				waitForInput(5);
				break;
			default:
				setColour('r');
				waitForInput(3);
				setColour('0');
				break;
		}
	} while (menuChoice != MENU_EXIT);
}