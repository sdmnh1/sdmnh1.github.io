// CS 499 - Computer Science Capstone - Milestone Two - Software Design and Engineering
//=============================================================================
// Created By : Maria Herring      Email: maria.herring@snhu.edu
// File Name : Enhancement of CS 210 Programming Languages Final Project
// Description : The main class includes the functionality to 
// manage the menu of the application
//=============================================================================


#include <iostream>
#include "banking.hpp"

using namespace std;

void StartMenu()

{   // Prints Start Menu 
	cout << "*************************************" << endl;
	cout << "********** Airgead Banking **********" << endl;
	cout << "*********** Menu Options ************" << endl;
	cout << "1. Calculate Savings Without Deposits" << endl;
	cout << "2. Calculate Savings With Monthly Deposits" << endl;
	cout << "3. Exit" << endl;
	cout << ">>";
}

void WithoutMonthlyInvestment()

{   // Print statement for user input
	cout << "*************************************" << endl;
	cout << "Please enter the following information to calculate your savings:" << endl;

	// Asks for user input(s) for initial investment, monthly dep., and annual interest
	double princInv, intAnnual;
	int years;

	// Error checking for user input for inital investment
	// amount, annual interest, and the number of years
	// requiring positive numeric values

	cout << "Initial Investment Amount: $";
	while (!(cin >> princInv) || princInv < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cout << "Annual Interest: %";
	while (!(cin >> intAnnual) || intAnnual < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cout << "Number of years: ";
	while (!(cin >> years) || years < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	system("CLS");
	Savings mySavings = Savings(princInv, intAnnual, years);
	mySavings.WithoutMonthlyDeposits();
	system("PAUSE");
	system("CLS");
}

void WithMonthlyInvestment()

{   // Print statement for user input
	cout << "*************************************" << endl;
	cout << "Please enter the following information to calculate your savings:" << endl;

	// Asks for user input(s) for initial investment, monthly dep., and annual interest
	double princInv, monthlyDep, intAnnual;
	int years;

	// Error checking for user input for inital investment 
	// amount, monthly deposit, annual interest, and the number of years
	// requiring positive numeric values 

	cout << "Initial Investment Amount: $";
	while (!(cin >> princInv) || princInv < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cout << "Monthly Deposit: $";
	while (!(cin >> monthlyDep) || monthlyDep < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	cout << "Annual Interest: %";
	while (!(cin >> intAnnual) || intAnnual < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	cout << "Number of years: ";
	while (!(cin >> years) || years < 0) {
		cout << "Invalid input. Please enter a valid non-negative numeric value: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	system("CLS");
	Savings mySavings = Savings(princInv, monthlyDep, intAnnual, years);
	mySavings.WithMonthlyDeposits();
	system("PAUSE");
	system("CLS");
}
int main()
{   
	// Loop for the start menu 
	while (true)
	{
		int option;
		StartMenu();
		while (!(cin >> option)) {
			cout << "Invalid input. Please enter a valid numeric value: " << endl;
			cout << ">>";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		// Switch statement for user input  (with, without, exit, and a default case)
		system("CLS");
		switch (option)
		{
		case 1:
			WithoutMonthlyInvestment();
			break;
		case 2:
			WithMonthlyInvestment();
			break;
		case 3:
			cout << "Thank you for choosing Airgead Banking." << endl;
			return 0;
		default:
			cout << "Invalid choice. Please enter a valid option." << endl;
			system("PAUSE");
			system("CLS");
			break;
		}
	}
}