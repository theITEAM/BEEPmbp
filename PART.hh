#pragma once

#include <vector>

using namespace std;

#include "model.hh"
#include "poptree.hh"
#include "data.hh"

struct NEV {                               // Information about the immediate next events
  int type; double t;
};

static bool compNEV(NEV lhs, NEV rhs)
{
	return lhs.t < rhs.t;
};

class PART                                 // Stores all the things related to a particle 
{
	public:
	PART(DATA &data, MODEL &model, POPTREE &poptree);

	int pst;                                // The number of the particle
	
 	double Li;                              // The observation likelihood
 	
	vector <vector <int> > indinf;          // Lists all infected individuals 	
		
	vector <vector <double> > sussum;       // The total susceptibility for nodes on different levels 
	vector <vector <double> > sussumst;     // The total susceptibility for nodes on different levels 
	vector <vector <double> > Rtot;         // The total infection rate for nodes on different levels
	
	vector <vector <double> > Qmap;         // The infectivity acting on different areas and age groups
	
	vector <vector <double> > susage;       // The total susceptibility for different age / area combination
	vector <vector <double> > susagest;     // The total susceptibility for different age / area combination
	
	int fediv;
	vector < vector <FEV> > fev;            // Stores all compartmental transitions
	
	vector < vector <FEV> > indev;          // For initialising MBP it stores the individual events
	
	vector <int> N;                         // The number of individuals in different compartments

	int sett;                               // Index used to track time changes in beta

	int tdnext, tdfnext;                    // Stores when the next future compartmental transition will occur
		
	DATA &data;
	MODEL  &model;

	vector <TRANS> &trans;
	vector <COMP> &comp;

	POPTREE &poptree;
	vector <LEVEL> &lev;
		
	public: 
		void gillespie(double ti, double tf, int outp);
		void partinit(int p);
		void dofe();
		int nextinfection(int type);
		void addinfc(int c, double t);
		void addfev(FEV fe, double period, double tnow);
		void copy(const PART &other, int fedivmin);
		void check(int num, double t);
		
		void partpack(int fedivmin);
		void partunpack(int fedivmin);
};
