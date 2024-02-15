/*
CS-210 Programming Languages
Project 2 - Airgead Banking App
Maria Herring
February 4, 2022
*/


#include "banking.hpp"
#include <iomanip>
#include <iostream>
using namespace std;
//Sets values for variables using instances
Savings::Savings(double t_investment, double t_monthDeposit, double t_annualRate,
	int t_years) {
	this->m_initialDep = t_investment;
	this->m_monthlyDep = t_monthDeposit;
	this->m_intRate = t_annualRate;
	this->m_numYears = t_years;
}

//Displays report without monthly deposits
void Savings::WithoutMonthlyDeposits() {
	//Prints Statement for user without montly deposits with parameters for year, balance, and interest
	cout << " Balance and Interest without additional Monthly Deposits" << endl;
	cout <<
		"==========================================================================" <<
		endl;
	cout << setw(10) << "Year"
		<< setw(20) << "Year End Balance"
		<< setw(35) << "Year End Earned Interest Rate" << endl;
	cout <<
		"--------------------------------------------------------------------------" <<
		endl;
	//variables for current year and year-end balance 
	int currentYear = 1;
	double yearEndBal = this->m_initialDep;
	//Loop to calculate annual interest earned
	while (currentYear <= this->m_numYears) {
		//Calculates interest
		double intEarned = yearEndBal * this->m_intRate / 100;
		//Adds it to yearEndBalance total
		yearEndBal += intEarned;
		//Prints report
		cout << right << setw(10) << currentYear << fixed << setprecision(2)
			<< setw(20) << yearEndBal
			<< setw(35) << intEarned << endl;
		//Continually adds a year until current year is reached
		currentYear++;
	}
}
//Report with monthly deposits
void Savings::WithMonthlyDeposits() {
	//Prints Statement for user without monthly deposits with parameters for year, balance, and interest 
	cout << " Balance and Interest without additional Monthly Deposits" << endl;
	cout <<
		"==========================================================================" <<
		endl;
	cout << setw(10) << "Year"
		 << setw(20) << "Year End Balance"
		 << setw(35) << "Year End Earned Interest Rate" << endl;
	cout <<
		"--------------------------------------------------------------------------" <<
		endl;
	//variables for current year and year-end balance 
	int currentYear = 1;
	double yearEndBal = this->m_initialDep;
	//Loop to calculate annual earned interest
	while (currentYear <= this->m_numYears) {
		//Calulates monthly and annual interest
		int month = 1;
		double intEarned = 0.0;
		double monthEndBal = yearEndBal;
		while (month <= 12) {
			//Adds monthly deposit
			monthEndBal += this->m_monthlyDep;
			//Calculates annual interest, dividing by 100 converts rate % to a decimal
			double monthlyInt = monthEndBal * this->m_intRate / (100 * 12);
			//Adds the monthly interest to yearly interest earned
			intEarned += monthlyInt;
			//Adds interest to month end balance
			monthEndBal += monthlyInt;
			//Increases the month by 1 until reaches 12
			month++;
		}
		//After 12 months
		yearEndBal = monthEndBal;
		//Prints statement with annual year end balance and annual interest
		cout << right << setw(10) << currentYear << fixed << setprecision(2)
			<< setw(20) << yearEndBal
			<< setw(35) << intEarned << endl;
		//Continually adds a year until current year is reached
		currentYear++;
	}
}
