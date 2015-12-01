// File name: Source.cpp
// Author: Hunter Manhart
// Userid: manharhm
// Email: Hunter.M.Manhart@vanderbilt.edu
// Description: Performs the revised simplex method on a given Standard Form Linear 
//				Program
// Last Changed: 11/20/2015

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Matrix definition and constants(limits)
typedef vector<vector <double>> Matrix;
const int MAX_ITERATIONS = 100;
const int MAX_VARS = 20;
const int MAX_RESTRICT = 20;

// -----Helper Functions-----
// printX: prints in format { x1 x2 x3 } of vector <1, 2, 3>
// printVector: prints in format { 1 2 3 }
// determineEnter: determines entering variable from cbar
// ratioLeave: determines leaving variable from ratios & prints them
// vectorMult: multiplies two vectors and returns result (double)
// determVar: gets final value of a variable by searching xb and xn

void printX(vector<int> x) {
	cout << "{";
	for (size_t j = 0; j < x.size(); j++) {
		cout << " x" << x[j];
	}
	cout << " }";
}

void printVector(vector<double> v) {
	for (size_t f = 0; f < v.size(); f++) {
		cout << v[f] << "\t";
	}
	cout << endl;
}

int determineEnter(vector<double> cbar) {
	int result = -1;
	double highest = 0;

	for (size_t i = 0; i < cbar.size(); i++) {
		if (cbar[i] > highest) {
			highest = cbar[i];
			result = i;
		}		
	}

	return result;
}

int ratioLeaving(vector<double> b,vector<double> aj, vector<int> xb){
	int result = -1;
	double lowest = 0;
	double ratio;
	bool first = true;
	
	cout << "ratio";
	for (size_t j = 0; j < b.size(); j++) {
		if (aj[j] > 0) {
			ratio = b[j] / aj[j];
			cout << "\tx" << xb[j] << " " << ratio;
			
			if (ratio < lowest || first) {				
				lowest = ratio;
				result = j;
				first = false;
			}
		}
	}
	cout << endl;

	return result;
}

// pre: vectors must be same size
double vectorMult(vector<double> a, vector<double> b) {
	double result = 0;	
	for (size_t h = 0; h < b.size(); h++) {
		result += a[h] * b[h];
	}	

	return result;
}

double determVar(int number, vector<int> xb, vector<int> xn, 
	vector<double> bbar) {	

	for (size_t i = 0; i < xn.size(); i++) {
		if (xn[i] == number)
			return 0;
	}
	for (size_t j = 0; j < xb.size(); j++) {
		if (xb[j] == number) {
			if (abs(bbar[j]) > 0.001) {
				return bbar[j];
			}
			else {
				return 0;
			}
		}
	}

	return -1; // Will be obvious if a mistake
}


int main()
{	
	// Header
	cout << "RevisedSimplex 0.10 <Hunter Manhart, 20 November 2015>" << endl;
	cout << "Input >> ";  
	

	// Input
	size_t m;
	size_t n;
	size_t mn;

	cin >> m >> n;

	//ofstream out("out1.txt");
	//streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	//cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

	if (m > MAX_RESTRICT || n > MAX_VARS) {
		string str;
		cout << "Linear program is too large" << endl;
		cout << "Press enter to exit" << endl;
		getline(cin, str);
		cin.get();  // get the key press
		exit(1);
	}

	mn = m + n;

	// Matricies are in j,i form to simplify pulling column vectors 
	vector<double> c(mn); vector<double> cbar(n);				// c
	Matrix a(mn, vector<double>(m)); vector<double> abarj(m);	// A
	Matrix ab(m, vector<double>(m));							// B
	Matrix e(m, vector<double>(m));								// E
	Matrix etaFile(MAX_ITERATIONS, vector<double>(m + 1));//Eta File
	vector<double> b(m); vector<double> bbar(m);				// b
	vector<double> y(m);										// y
	vector<int> xb(m); vector<int> xn(n);						// x
	double t;									// for getting bbar


	for (size_t j = 0; j < mn; j++) { // c & x
		if (j < n) {
			cin >> c[j];
			cbar[j] = c[j];
			xn[j] = j + 1;
		}
		else {
			xb[j - n] = j + 1;
		}
	}	

	for (size_t i = 0; i < m; i++) { // A & b
		for (size_t j = 0; j < n + 1; j++) {
			if(j != n)
				cin >> a[j][i];
			else
				cin >> b[i];
		}

		a[i + n][i] = 1;
		ab[i][i] = 1;
	}	
	bbar = b;

	
	// Pre-lim output
	cout << endl << "--- output ---" << endl << endl;
	cout << "m = " << m << "\tn = " << n << endl;

	cout << "c = ";
	printVector(c);

	cout << "b = ";
	printVector(b);

	cout << "A =" << endl;
	for (size_t i = 0; i < m; i++) {
		for (size_t j = 0; j < mn; j++) {
			cout << a[j][i] << "\t";
		}
		cout << endl;
	}
	cout << endl << endl;


	// Work
	bool finished = false;
	size_t iteration = 1;
	int enter, leave, col;

	while (!finished && iteration <= 5	) {		
		cout << "N = ";				// print null vars
		printX(xn);
		cout << "\tB = ";			// print basic vars
		printX(xb);

		cout << endl << "bbar =\t";	// print bbar
		printVector(bbar);

		cout << endl << "y =\t";	// print y
		printVector(y);

		cout << "cbar\t";			// print cbar
		for (size_t f = 0; f < n; f++) {
			cout << "x" << xn[f] << " " << cbar[f] << "\t";
		}
		
		// Get entering variable
		enter = determineEnter(cbar);
		if (enter == -1) { // if none, we're done here
			double z = vectorMult(y, b);
			cout << endl << endl << "Optimal value of ";
			cout << z << " has been reached." << endl;

			cout << "Original:";
			for (size_t i = 1; i <= n; i++) {
				cout << "\tx" << i << "=";
				cout << determVar(i, xb, xn, bbar);
			}

			cout << endl << "Slack:\t";
			for (size_t i = n+1; i <= mn; i++) {
				cout << "\tx" << i << "=";
				cout << determVar(i, xb, xn, bbar);
			}
			cout << endl;

			finished = true;
			break;
		}		
		cout << endl <<"Entering variable is x" << xn[enter] << endl;

		// update d (abarj)
		abarj = a[xn[enter] - 1];
		double vp;
		for (int g = 0; g < (int)iteration-1; g++) {
			col = (int)etaFile[g][m];
			vp = abarj[col] / etaFile[g][col];
			abarj[col] = vp;			

			for (size_t i = 0; i < m; i++) {
				if (i != col) {
					abarj[i] -= etaFile[g][i] * abarj[col];
				}
			}
		}		
		cout << "abarj =\t";
		printVector(abarj);

		// Determine leaving variable (where 7/10 people mess up)
		leave = ratioLeaving(bbar, abarj, xb);
		if (leave == -1) {
			finished = true;
			cout << "Solution is unbounded" << endl;			
			break;
		}
		t = bbar[leave] / abarj[leave];

		cout << "Leaving variable is x" << xb[leave] << endl;

		// Print new E col
		cout << "E" << iteration << " = column " << leave+1 << ":\t";
		printVector(abarj);

		// Update eta file (first input is E1 not E0)
		for (size_t i = 0; i < abarj.size(); i++) {
			etaFile[iteration - 1][i] = abarj[i];
		}
		etaFile[iteration - 1][m] = leave;
			
		swap(xb[leave], xn[enter]); // Swap entering and leaving

		// Update y
		for (size_t i = 0; i < m; i++) { 
			y[i] = c[xb[i] - 1];
		}		
		
		for (int g = iteration - 1; g >= 0; g--) {	
			col = (int)etaFile[g][m];			
			for (size_t i = 0; i < m; i++) {
				if (i != col) {
					y[col] -= etaFile[g][i] * y[i];
				}
			}

			y[col] /= etaFile[g][col];			
		}

		// Update bbar		
		for (size_t i = 0; i < m; i++) {			
			bbar[i] -= t * abarj[i];
			if (abs(bbar[i]) < 0.001) {
				bbar[i] = 0;
			}
			if (bbar[i] < 0) { // Sign constraints
				finished = true;
				cout << "The linear program is infeasible:" << endl;
				cout << "bbar: ";
				printVector(bbar);
				break;
			}
		}
		bbar[leave] = t;			

		// Update cbar		
		for (size_t j = 0; j < n; j++) {
			cbar[j] = c[xn[j] - 1] - vectorMult(y, a[xn[j] - 1]);			
		}		

		// Increase iteration for y and d updates and max iterations
		iteration++; 
	}	
	
	// If we didn't find an optimal solution
	if (!finished) {
		cout <<"Max number of iterations reached ("<< MAX_ITERATIONS;
		cout << ")" << endl << "No optimal solution found" << endl;
	}

	//cout.rdbuf(coutbuf); //reset to standard output again

	// Politely kick the user from the program
	string str;
	cout << "Press enter to end program" << endl;
	getline(cin, str);
	cin.get();  // get the key press
	exit(1);
}
