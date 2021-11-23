
#pragma region Declarations
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdio.h> //basic input output libraries
	#include <string.h> //string manipulation tools
	#include <stdlib.h> //file manipulaiton
	#include <errno.h> //error code handling

	#define bool int

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
#pragma endregion Universal Constants

#pragma region Variables
	//arrays, used throughout the program and store the transaction history
	int numberOfTransactions = 0; //INDEX

	float totalSales[MAX_SALES]; //holds value of each transaction
	int customerCarsSold[MAX_SALES]; //the number of cars a user buys
	unsigned short discountGivenPerSale[MAX_SALES]; //tracks if a discount was given
	char customerNames[MAX_SALES][101]; //stores customer names up to 100 chars long
	unsigned short typeOfCarSale[MAX_SALES]; //tracks the type of car sold per sale

	float carPrices[] = { 5999.99f, 10000.0f, 15999.99f }; //price of each car
	char carTypes[][10] = { "Ford", "Fiat", "Subaru" }; //name of each car

	//The following keep track of basic metrics that aren't covered by arrays
	unsigned short carsAvalible = 20, totalNumCarsSold = 0;
	float totalProfit = 0;
	int numberOfCarsInStock = sizeof(carPrices) / sizeof(float); //calculate the amount of car types

	//files
	unsigned char currentFileStatus = FILE_CLOSED;

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
			//4 = menu; exit

			switch (userChoice)
			{
			case 1:
				printf("\n>>> ");
				getchar();
				break;
			case 2:
				printf("\n>>> ");
				getchar();
				getchar();
				break;

			case 3:
				printf("Press enter to return to the main menu.\n>>> ");
				getchar();
				getchar();
				break;
			case 4:
				printf("Press enter to quit the program.\n>>> ");
				getchar();
				getchar();
				break;
			}
		}
	#pragma endregion Misc
	#pragma region Inputs
			char getCharInput(char message[201])
			{
				char charResult;
				char userInputStr[201];
				int userInputStrLen;

				printf("%s\n", message);
				do
				{
					fgets(userInputStr, sizeof(userInputStr), stdin);
					userInputStrLen = strlen(userInputStr) - 1;

					if (userInputStr[0] != '\n' && userInputStrLen != 0)
					{
						charResult = userInputStr[0];
						return charResult;
					}
					else
					{
						printf(">>> ");
					}
				} while (1);
			}
			unsigned short getUnsShortInput(char message[201])
			{
				unsigned short userInput;
				int noOfValues;

				printf("%s\n>>> ", message);
				do
				{
					noOfValues = scanf("%hd", &userInput);
					

					if (noOfValues != 0)
					{
						return userInput;
					}
					else
					{
						printf(">>> ");
					}

				} while (1);

			}
			void getStringInput(char message[201], char *userInput)
			{
				int strLen;

				printf("%s\n", message);
				do
				{
					fgets(userInput, sizeof(userInput), stdin);
					if (userInput[0] != '\n')
					{
						strLen = strlen(userInput) - 1;
						userInput[strLen] = '\0'; //sets the last char as null instead of \n
						break;
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
				customerCarsSold[fileLineCounter] = carAmountPerSaleValue;
				typeOfCarSale[fileLineCounter] = typeOfCarPerSaleValue;
				discountGivenPerSale[fileLineCounter] = (bool)discountGivenPerSaleValue;
				strcpy(customerNames[fileLineCounter], customerNameValue);

				//Updating total profit (price of car type * how many sold * discount
				if (discountGivenPerSale[fileLineCounter] == TRUE)
				{
					totalProfit = carPrices[typeOfCarSale[fileLineCounter]] * customerCarsSold[fileLineCounter] * (1 - DISCOUNT_PERCENTAGE);
				}
				else
				{
					totalProfit = carPrices[typeOfCarSale[fileLineCounter]] * customerCarsSold[fileLineCounter];
				}

				carsAvalible -= carAmountPerSaleValue;

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
			else if (currentFileStatus == FILE_ERROR) {
				printf("There was an error trying to open the file at: %s\nIf you're seeing this, this file likely doesn't exist yet.", CSV_FILE_LOCATION);
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
				_itoa((int)customerCarsSold[i], data, 10); //convert from deanary to string, add to data				
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
			else if (currentFileStatus == FILE_ERROR) {
				printf("There was an error when writing to the file at: %s\n", CSV_FILE_LOCATION);
				//waitForInput(1); (not needed because of the exit message in main)
			}

			closeFile();
		}
	#pragma endregion Files
	#pragma region Menu
		void menu_greeting()
		{
			printf("Welcome to Noven's used Car Emporium!\n");
		}
		char menu_menu()
		{
			printf("Please choose what service you'd like!\n");
			printf("a - Buy a Car\n");
			printf("b - View Sales Statistics\n");
			printf("x - Quit the Program\n>>> ");
			char menuChoice = getchar();
			return menuChoice;
		}
	#pragma endregion Menu
	#pragma region Menu Options
		#pragma region Buy
			unsigned short buy_CheckForDiscount(unsigned short userAge)
			{
				if (userAge >= DISCOUNT_MIN_AGE && userAge <= DISCOUNT_MAX_AGE)
				{
					getchar(); //prevents not being able to see the recipt if true
					return TRUE; //gives a discount if the user's age falls within a specified value
				}
				else
				{
					char hasNUSCard;
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
				printf("~~~~~~~~~~~Thank you for your business!~~~~~~~~~~~\n");
				printf("The final sum is %c%.2f", 156, finalPrice);
				buy_PrintDiscount(discountGiven);
				printf("\nThere are %hd remaining Cars that can be purchased.", carsAvalible);
				printf("\nHave a good day, my good man. Please come again.");
				printf("\n\nYou obtained %hd Cars!!", numCarsBrought);

				waitForInput(1);
				clearScreen();
			}
			void buy_showCarTypes()
			{
				printf("Car Types:\n");
				printf("ID\tName\tPrice\n");
				for (int i = 0; i < numberOfCarsInStock; i++)
				{
					printf("%d\t%s\t%c%.2f\n", i, carTypes[i], 156, carPrices[i]);
				}
			}
			void menuChoice_BuyTickets()
			{
				//Vars
				unsigned short numCarsBrought = 0, userAge = 0, carType;
				unsigned short discountGiven = FALSE;
				float finalSumPrice = 0.0;
				unsigned char hasNUSCard = 0;

				//check if cars are availible
				if (carsAvalible == 0)
				{
					printf("Sorry, but we're all sold out! Please come again!");
					waitForInput(2);
					clearScreen();
					return; //sends you back to the menu
				}

				//Name and cars needed
				getStringInput("What is your Name?", &customerNames[numberOfTransactions]);
				//printf("What is your Name?\n>>> ");
				//scanf("\n%[^\n]s", &customerNames[numberOfTransactions]);

				unsigned short carTemp = 0;
				carTemp = getUnsShortInput("How many car(s) would you like, good sir?");
				numCarsBrought = carTemp;

				//Enough Cars?
				if (carsAvalible < numCarsBrought) {
					printf("Sorry, but we don't have enough of the cars you requested. Please come again!\n>>> ");
					clearScreen();
					return;
				}

				//Car Wanted
				buy_showCarTypes();
				carType = getUnsShortInput("Please choose an option from the list above");

				//Discount and price
				finalSumPrice = numCarsBrought * carPrices[carType]; //calculate the price

				userAge = getUnsShortInput("How old are you?");
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
				customerCarsSold[numberOfTransactions] = numCarsBrought; //tracks how many cars this user brought
				numberOfTransactions++; //tracks the number of transactions

				//Finish
				buy_Recipt(finalSumPrice, numCarsBrought, discountGiven);
			}
		#pragma endregion Buy
		#pragma region Records
			void menuChoice_ViewRecords()
			{
				float totalSalesValue = 0; //the counter that tracks the number of transactions
				int typeOfCar;
				float price; //Used to show the price of a car after discounts applied. Used instead of getting a car's value from an array
				int numberOfCarsCounter = 0;

				switch (numberOfTransactions)
				{
					case 0:
						printf("There have been no cars sold.");
						waitForInput(3);
						return;
						break;
					default:
						printf("ID\tName\tType\t#Cars\tCost\t\tCost/Car\tDiscount?\n");
						for (int i = 0; i < numberOfTransactions; i++) //loops through the number of sales
						{
							typeOfCar = typeOfCarSale[i];
							price = typeOfCarSale[i] * carPrices[typeOfCar]; //the price of an induvidual car

							char discountGivenAsText[4];
							if (discountGivenPerSale[i] == TRUE)
							{
								strcpy(discountGivenAsText, "Yes");
								price *= (1 - DISCOUNT_PERCENTAGE);
								totalSales[i] = carPrices[typeOfCar] * customerCarsSold[i] * (1 - DISCOUNT_PERCENTAGE);
							}
							else
							{
								strcpy(discountGivenAsText, "No");
								totalSales[i] = carPrices[typeOfCar] * customerCarsSold[i];
							}

							//Split over two lines for easier mangement
							printf("%d\t%s\t%s\t%d\t", (i + 1), customerNames[i], carTypes[typeOfCar], customerCarsSold[i]); 
							printf("%c%.2f\t%c%.2f\t%s\n", 156, totalSales[i], 156, carPrices[typeOfCar], discountGivenAsText); //156 displays the £ symbol

							totalSalesValue += totalSales[i]; //sets the number of sales to the current transaction's number
							numberOfCarsCounter += 1;
						}
						printf("\nTotal profit: %c%.2f.", 156, (double)totalProfit);
						printf("\n%hd cars have been sold, and there are %hd cars remaining.\n", numberOfCarsCounter, carsAvalible);
						waitForInput(3);
						break;
				}
			}
		#pragma endregion Records
	#pragma endregion Menu Options	
	
#pragma endregion Functions

void main()
{
	char menuChoice = 0;

	getFileData();
	menu_greeting();
	do
	{
		clearScreen();
		menuChoice = menu_menu();
		clearScreen();

		//Menu Options
		switch (menuChoice)
		{
			case MENU_BUY:
				menuChoice_BuyTickets();
				break;
			case MENU_VIEW:
				menuChoice_ViewRecords();
				break;
			case MENU_EXIT:
				saveFile();
				waitForInput(4);
				break;
			default:
				waitForInput(3);
				break;
		}
	} while (menuChoice != MENU_EXIT);
}