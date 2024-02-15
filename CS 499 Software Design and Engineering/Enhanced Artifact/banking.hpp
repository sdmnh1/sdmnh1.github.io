// CS 499 - Computer Science Capstone - Milestone Two - Software Design and Engineering
//=============================================================================
// Created By : Maria Herring   Email: maria.herring@snhu.edu
// File Name : Enhancement of CS 210 Programming Languages Final Project
// Description : The banking header file includes the functionality to
// define classesand varibles for the applicaition
//=============================================================================


#pragma once
#ifndef banking_hpp
#define banking_hpp
#include<iostream>

class Savings {
public:
	// Public class savings
	Savings(double t_investment, double t_annualRate, int t_years);

	Savings(double t_investment, double t_monthlyDeposit, double t_annualRate,
		int t_years);
	void WithoutMonthlyDeposits() const;
	void WithMonthlyDeposits() const;

	// Private class savings
private:
	double m_initialDep;
	double m_monthlyDep{};
	double m_intRate;
	int m_numYears;
};
#endif

