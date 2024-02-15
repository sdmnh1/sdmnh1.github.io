/*
CS-210 Programming Languages
Project 2 - Airgead Banking App
Maria Herring
February 4, 2022
*/

#pragma once
#ifndef banking_hpp
#define banking_hpp
#include <stdio.h>
class Savings {
public:
	//public class savings
	Savings(double t_investment, double t_monthDeposit, double t_annualRate,
		int t_years);
	void WithoutMonthlyDeposits();
	void WithMonthlyDeposits();

	//private class savings
private:
	double m_initialDep;
	double m_monthlyDep;
	double m_intRate;
	int m_numYears;
};
#endif 