// CS 499 - Computer Science Capstone - Milestone Two - Software Design and Engineering
//=============================================================================
// Created By : Maria Herring      Email: maria.herring@snhu.edu
// File Name : Enhancement of CS 210 Programming Languages Final Project
// Description : The banking class includes the functionality to
// manage the calucation functions for the applicaition
//=============================================================================


#include "banking.hpp"
#include <iomanip>
#include <iostream>

using namespace std;

// Sets values for variables using instances
Savings::Savings(double t_investment, double t_annualRate, int t_years)
	: m_initialDep(t_investment)
	, m_intRate(t_annualRate)
	, m_numYears(t_years)
{}

Savings::Savings(double t_investment, double t_monthlyDeposit, double t_annualRate, int t_years)
	: m_initialDep(t_investment)
	, m_monthlyDep(t_monthlyDeposit)
	, m_intRate(t_annualRate)
	, m_numYears(t_years)
{}

// Function to display report without monthly deposits
void Savings::WithoutMonthlyDeposits() const {

	// Prints Statement for user without monthly deposits with parameters for year, interest, and balance
	cout << "Balance and Interest without additional Monthly Deposits" << endl;
	cout << fixed << setprecision(2) << setw(40) << "Initial Investment Amount: $" << this->m_initialDep << endl;
	cout << fixed << setprecision(2) << setw(40) << "Annual Interest: $" << this->m_intRate << endl;
	cout << fixed << setprecision(2) << setw(40) << "Number of years: $" << this->m_numYears << endl;
	cout << "==========================================================================" << endl;
	cout << setw(10) << "Year"
		<< setw(20) << "Year End Balance"
		<< setw(35) << "Year End Earned Interest Rate" << endl;
	cout <<
		"--------------------------------------------------------------------------" <<
		endl;

	// Variables for current year and year-end balance 
	int currentYear = 1;
	double yearEndBal = this->m_initialDep;

	// Loop to calculate annual interest earned
	while (currentYear <= this->m_numYears) {

		// Calculates interest
		const double intEarned = yearEndBal * this->m_intRate / 100;

		// Adds interest to year end balance
		yearEndBal += intEarned;

		// Prints report
		cout << right << setw(10) << currentYear << fixed << setprecision(2)
			<< setw(20) << yearEndBal
			<< setw(35) << intEarned << endl;

		// Continually adds a year 
		currentYear++;
	}
}
// Function to display report with monthly deposits
void Savings::WithMonthlyDeposits() const
{

	// Prints Statement for user with monthly deposits with parameters for year, interest, monthly deposit, and balance 
	cout << " Balance and Interest with additional Monthly Deposits" << endl;
	cout << fixed << setprecision(2) << setw(40) << "Initial Investment Amount: $" << this->m_initialDep << endl;
	cout << fixed << setprecision(2) << setw(40) << "Monthly Deposit Amount: $" << this->m_monthlyDep << endl;
	cout << fixed << setprecision(2) << setw(40) << "Annual Interest: $" << this->m_intRate << endl;
	cout << fixed << setprecision(2) << setw(40) << "Number of years: $" << this->m_numYears << endl;
	cout << "==========================================================================" << endl;
	cout << setw(10) << "Year"
		<< setw(20) << "Year End Balance"
		<< setw(35) << "Year End Earned Interest Rate" << endl;
	cout <<
		"--------------------------------------------------------------------------" <<
		endl;

	// Variables for current year and year-end balance 
	int currentYear = 1;
	double yearEndBal = this->m_initialDep;

	// Loop to calculate annual earned interest
	while (currentYear <= this->m_numYears) {

		// Calulates monthly and annual interest
		int month = 1;
		double intEarned = 0.0;
		double monthEndBal = yearEndBal;
		while (month <= 12) {

			// Adds a monthly deposit
			monthEndBal += this->m_monthlyDep;

			// Calculates annual interest, dividing by 100 converts rate % to a decimal
			const double monthlyInt = monthEndBal * this->m_intRate / (1200);

			// Adds the monthly interest to yearly interest earned
			intEarned += monthlyInt;

			// Adds interest to monthly end balance
			monthEndBal += monthlyInt;

			// Increases the month by 1
			month++;
		}

		yearEndBal = monthEndBal;

		// Prints statement with annual year end balance and annual interest
		cout << right << setw(10) << currentYear << fixed << setprecision(2)
			<< setw(20) << yearEndBal
			<< setw(35) << intEarned << endl;

		// Continually adds a year until the current year is reached
		currentYear++;
	}
}
