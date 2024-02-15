/*
CS-210 Programming Languages
Project 2 - Airgead Banking App
Maria Herring
February 4, 2022
*/

#include <iostream>
#include <string>
#include "banking.hpp"
using namespace std;

int main() {
	while (1) {
		//Prints Display Menu
		cout << "*************************************" << endl;
		cout << "************ Data Input *************" << endl;
		cout << "Initial Investment Amount: " << endl;
		cout << "Monthly Deposit: " << endl;
		cout << "Annual Interest: " << endl;
		cout << "Number of years: " << endl;
		//system pause to wait for user input
		system("PAUSE");
		cout << endl;
		cout << "*************************************" << endl;
		cout << "************ Data Input *************" << endl;
		//Asks for user input(s) for initial investment, monthly dep., and annual interest
		cout << "Initial Investment Amount: $";
		double princInv, monthlyDep, intAnnual;
		int years;
		cin >> princInv;
		cout << "Monthly Deposit: $";
		cin >> monthlyDep;
		cout << "Annual Interest: %";
		cin >> intAnnual;
		cout << "Number of years: ";
		cin >> years;

		//system pause to wait for user input
		system("PAUSE");
		//Creates savings object using the given variables
		Savings mySavings = Savings(princInv, monthlyDep, intAnnual,
			years);
		cout << endl;
		//Method for report without deposits
		mySavings.WithoutMonthlyDeposits();
		cout << endl;

		//Method for report monthly report with deposits
		if (monthlyDep > 0) {
			mySavings.WithMonthlyDeposits();
		}

		//Option for user to continue
		cout << endl << "Do you wish to continue? (y/n): ";
		string userInput;
		cin >> userInput;
		//breaks loop if user finished, prints goodbye statement
		if (userInput != "y") {
			cout << "Thank you for choosing Airgead Banking." << endl;
			break;
		}
		cout << endl;
	}
	return 0;
}