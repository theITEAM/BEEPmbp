// This code generate a hierarchical representation of the houses at different levels of spatial scale

#include <iostream>
#include <algorithm>

#include "math.h"

using namespace std;

#include "utils.hh"
#include "poptree.hh"
#include "consts.hh"
#include "data.hh"

struct POS{
	long c;
	double dist;
};
bool ordpos(POS lhs, POS rhs) { return lhs.dist < rhs.dist; }
		
// Initialises a tree of levels in which the entire population is subdivied onto a finer and finer scale
void POPTREE::init(DATA &data, short core)
{
	long h, l, i, imax, j, jmax, ii, jj, k, kmax, m, mmax, num, po, c, cmax, cc, ccc, par, s, L, n, flag, fi, pop;
	double av, x, y, xx, yy, grsi, dd, sum, fac, val;
	NODE node;
	
	const short posmax = 100;   // This gives the maximum number of possibilities for a given level in M 
	vector <POS> listpos;
	POS pos;
	
	vector <vector <double> > Mval_sr_temp;
	vector <vector <double> > Mval_lr_temp;
	vector <vector <long> > Mnoderef_temp;
	vector <vector <long> > addnoderef_temp;
	vector <long> housex1, housex2;
	vector <double> grsize;
	vector <long> grL;
	vector< vector< vector <long> > > grid;
																							           		  // Here a "node" represents a collection of houses
	lev.push_back(LEVEL ());                                                // The first level contains a single node
	for(h = 0; h < data.nhouse; h++) node.houseref.push_back(h);            // with all the houses
	node.parent = -1;
	lev[0].node.push_back(node);

	l = 0;
	do{
		lev.push_back(LEVEL ());
		
		flag = 0;
		
		 // The next level contains four child nodes 
		 // These are generated by taking the houses in the current node and 
		 // divivding them first horizontally and then vertically to generate four sub groups 
		for(c = 0; c < lev[l].node.size(); c++){  
			data.sortX(lev[l].node[c].houseref);
			num = lev[l].node[c].houseref.size();
			housex1.clear(); housex2.clear();
			for(j = 0; j < num/2; j++) housex1.push_back(lev[l].node[c].houseref[j]);
			for(j = num/2; j < num; j++) housex2.push_back(lev[l].node[c].houseref[j]);
		
			data.sortY(housex1);
			num = housex1.size(); if(num > 2) flag = 1;
			
			node.houseref.clear(); node.child.clear(); 
			for(j = 0; j < num/2; j++) node.houseref.push_back(housex1[j]);

			if(node.houseref.size() > 0){
				node.parent = c;
				lev[l].node[c].child.push_back(lev[l+1].node.size());	
				lev[l+1].node.push_back(node);
			}

			node.houseref.clear(); node.child.clear();
			for(j = num/2; j < num; j++) node.houseref.push_back(housex1[j]);
			if(node.houseref.size() > 0){
				node.parent = c;
				lev[l].node[c].child.push_back(lev[l+1].node.size());	
				lev[l+1].node.push_back(node);
			}
			
			data.sortY(housex2);
			num = housex2.size(); if(num > 2) flag = 1;
			
			node.houseref.clear(); node.child.clear(); 
			for(j = 0; j < num/2; j++) node.houseref.push_back(housex2[j]);
			if(node.houseref.size() > 0){
				node.parent = c;
				lev[l].node[c].child.push_back(lev[l+1].node.size());	
				lev[l+1].node.push_back(node);
			}

			node.houseref.clear(); node.child.clear();
			for(j = num/2; j < num; j++) node.houseref.push_back(housex2[j]);
			if(node.houseref.size() > 0){
				node.parent = c;
				lev[l].node[c].child.push_back(lev[l+1].node.size());	
				lev[l+1].node.push_back(node);
			}
		}
		l++;
		if(lev[l].node.size() >= areamax) break;
	}while(flag == 1);
 
	level = l+1;

	Cfine = lev[l].node.size();
	if(core == 0) cout << level << " Number of levels" << endl << Cfine << " Number of regions" << endl;

	cmax = lev[l].node.size();                    // On each node store all descendent nodes on the fine scale
	for(c = 0; c < cmax; c++) lev[l].node[c].fine.push_back(c);
	
	for(l = level-2; l >= 0; l--){
		cmax = lev[l].node.size();
		for(c = 0; c < cmax; c++){
			jmax = lev[l].node[c].child.size();
			for(j = 0; j < jmax; j++){
				cc = lev[l].node[c].child[j];
				kmax = lev[l+1].node[cc].fine.size();
				for(k = 0; k < kmax; k++) lev[l].node[c].fine.push_back(lev[l+1].node[cc].fine[k]);
			}
		}
	}
	
	l = level-1;
	for(c = 0; c < Cfine; c++){                    // Calculates positions and populations of each node
		x = 0; y = 0; pop = 0;
		imax = lev[l].node[c].houseref.size();
		for(i = 0; i < imax; i++){
			h = lev[l].node[c].houseref[i];
			pop += data.house[h].ind.size();
			x += data.house[h].x;
			y += data.house[h].y;
		}
		lev[l].node[c].population = pop;
		lev[l].node[c].x = x/imax;
		lev[l].node[c].y = y/imax;
		lev[l].node[c].done = 0;
	}
	
	for(l = level-2; l >= 0; l--){
		for(c = 0; c < lev[l].node.size(); c++){
			x = 0; y = 0; pop = 0;
			jmax = lev[l].node[c].child.size();
			for(j = 0; j < jmax; j++){
				cc = lev[l].node[c].child[j];
				pop += lev[l+1].node[cc].population;
				x += lev[l+1].node[cc].x;
				y += lev[l+1].node[cc].y;
			}
			lev[l].node[c].population = pop;
			lev[l].node[c].x = x/jmax;
			lev[l].node[c].y = y/jmax;
			lev[l].node[c].done = 0;
		}
	}
	
	grsi = finegridsize;                               // Creates grids that maps the locations of nodes at different levels
	grsize.resize(level); grL.resize(level); grid.resize(level);
	for(l = level-1; l >= 0; l--){
		L = long(1.0/(2*grsi)); if(L < 1) L = 1;
		grsize[l] = grsi;
		grL[l] = L;
		cmax = lev[l].node.size();
		grid[l].resize(L*L);
		for(c = 0; c < cmax; c++){
			i = long(lev[l].node[c].x*L);
			j = long(lev[l].node[c].y*L);	
			grid[l][j*L+i].push_back(c);
		}
		grsi *= 2;
	}
	
	nMval = new long*[Cfine]; Mval = new float**[Cfine]; Mnoderef = new long**[Cfine];
	naddnoderef = new long*[Cfine]; addnoderef = new long**[Cfine]; 
	
	for(c = 0; c < Cfine; c++){                          // Generates the interaction matrix M
		nMval[c] = new long[level]; Mval[c] = new float*[level]; Mnoderef[c] = new long*[level];
		naddnoderef[c] = new long[level]; addnoderef[c] = new long*[level]; 
		
		Mval_sr_temp.clear(); Mval_sr_temp.resize(level);
		Mval_lr_temp.clear(); Mval_lr_temp.resize(level);
		Mnoderef_temp.clear(); Mnoderef_temp.resize(level);
		addnoderef_temp.clear(); addnoderef_temp.resize(level);
		
		if(core == 0 && c%1000 == 0) cout << c << " / "  << Cfine << " Constructing matrix M" << endl;
		l = level-1;
		
		x = lev[l].node[c].x;
		y = lev[l].node[c].y;
	
		for(l = level-1; l >= 0; l--){
			flag = 0;
			if(l < level-1){
				kmax = lev[l+1].donelist.size();
				for(k = 0; k < kmax; k++){
					cc = lev[l+1].donelist[k];
					par = lev[l+1].node[cc].parent; if(par == -1) emsg("Poptree: EC1");
					if(lev[l].node[par].done == 0){
						lev[l].node[par].done = 1;
						lev[l].donelist.push_back(par);
										
						mmax = lev[l].node[par].child.size();
						for(m = 0; m < mmax; m++){
							ccc = lev[l].node[par].child[m];
							if(lev[l+1].node[ccc].done == 0){
								xx = lev[l+1].node[ccc].x;
								yy = lev[l+1].node[ccc].y;
						
								dd = sqrt((xx-x)*(xx-x) + (yy-y)*(yy-y));
				
								Mnoderef_temp[l+1].push_back(ccc);
								Mval_sr_temp[l+1].push_back(1.0/(1+pow(dd/a,b)));
								if(dd < a) dd = a;
								Mval_lr_temp[l+1].push_back(1.0/dd);
								flag = 1;
							}
						}
					}
				}
			}

			L = grL[l];	grsi = grsize[l];
			i = long(L*x+0.5)-1;
			j = long(L*y+0.5)-1;

			listpos.clear();
			for(ii = i; ii <= i+1; ii++){
				for(jj = j; jj <= j+1; jj++){
					if(ii >= 0 && ii < L && jj >= 0 && jj < L){
						n = jj*L+ii;
						kmax = grid[l][n].size();
						for(k = 0; k < kmax; k++){
							cc = grid[l][n][k];
							if(lev[l].node[cc].done == 0){
								xx = lev[l].node[cc].x;
								yy = lev[l].node[cc].y;
		
								dd = sqrt((xx-x)*(xx-x) + (yy-y)*(yy-y));
								if(dd < grsi && dd < ddmax){
									pos.c = cc; pos.dist = dd;
									listpos.push_back(pos);
								}
							}
						}
					}
				}
			}
							
			if(listpos.size() > posmax){  // Makes sure fine scale does not extend too far
				sort(listpos.begin(),listpos.end(),ordpos);
				listpos.resize(posmax);
			}
			
			for(po = 0; po < listpos.size(); po++){
				cc = listpos[po].c;
				dd = sqrt(listpos[po].dist);
				Mnoderef_temp[l].push_back(cc);
				Mval_sr_temp[l].push_back(1.0/(1+pow(dd/a,b)));
				if(dd < a) dd = a;
				Mval_lr_temp[l].push_back(1.0/dd);
				flag = 1;
				
				lev[l].node[cc].done = 1;
				lev[l].donelist.push_back(cc);
			}
		}		

		sum = 0;                             // Normalises short range M contribution to 1
		for(l = 0; l < level; l++){
			kmax = Mval_sr_temp[l].size(); 
			for(k = 0; k < kmax; k++) sum += Mval_sr_temp[l][k]*lev[l].node[Mnoderef_temp[l][k]].population;
		}
		
		fac = 0.8/sum;
		for(l = 0; l < level; l++){
			kmax = Mval_sr_temp[l].size(); for(k = 0; k < kmax; k++) Mval_sr_temp[l][k] *= fac;
		}	
	
		sum = 0;                              // Normalises long range M contribution to 0.2
		for(l = 0; l < level; l++){
			kmax = Mval_lr_temp[l].size(); 
			for(k = 0; k < kmax; k++) sum += Mval_lr_temp[l][k]*lev[l].node[Mnoderef_temp[l][k]].population;
		}
		
		fac = 0.2/sum;
		for(l = 0; l < level; l++){
			kmax = Mval_lr_temp[l].size(); for(k = 0; k < kmax; k++) Mval_lr_temp[l][k] *= fac;
		}
		
		for(l = 0; l < level; l++){           // Works out how to add up contributions to the root
			kmax = Mnoderef_temp[l].size(); 
			for(k = 0; k < kmax; k++) lev[l].node[Mnoderef_temp[l][k]].done = 0;
			
			jmax = lev[l].donelist.size(); 
			for(j = 0; j < jmax; j++){
				cc = lev[l].donelist[j];
				if(lev[l].node[cc].done == 1) addnoderef_temp[l].push_back(cc);
			}
		}			
		
		for(l = 0; l < level; l++){           // Stores the results
			kmax = Mval_sr_temp[l].size();
			nMval[c][l] = kmax; Mval[c][l] = new float[kmax];	Mnoderef[c][l] = new long[kmax];
			for(k = 0; k < kmax; k++){
				Mval[c][l][k] = Mval_sr_temp[l][k] + Mval_lr_temp[l][k];
				Mnoderef[c][l][k] = Mnoderef_temp[l][k];
			}
			
			kmax = addnoderef_temp[l].size();
			naddnoderef[c][l] = kmax; addnoderef[c][l] = new long[kmax];
			for(k = 0; k < kmax; k++){
				addnoderef[c][l][k] = addnoderef_temp[l][k];
			}
		}
		
		for(l = 0; l < level; l++){
			kmax = lev[l].donelist.size(); for(k = 0; k < kmax; k++) lev[l].node[lev[l].donelist[k]].done = 0;
			lev[l].donelist.clear();
		}
		
		if(checkon == 1){
			pop = 0;
			for(l = 0; l < level; l++){
				kmax = nMval[c][l];
				for(k = 0; k < kmax; k++) pop += lev[l].node[Mnoderef[c][l][k]].population;
			}
			if(pop != data.popsize) emsg("Poptree: EC2");
		
			for(l = 0; l < level; l++){
				cmax = lev[l].node.size();
				for(cc = 0; cc < cmax; cc++) if(lev[l].node[cc].done != 0) emsg("Poptree: EC3");
			}
		}
	}	
	
	ind.resize(data.popsize);	                         // Associates indiividuals with regions. this is done for simulation
	for(c = 0; c < Cfine; c++){                    // purposes, but with the real data these will be health board areas.
		kmax = lev[level-1].node[c].houseref.size();
		for(k = 0; k < kmax; k++){	
			h = lev[level-1].node[c].houseref[k];
			for(j = 0; j < data.house[h].ind.size(); j++){
				i = data.house[h].ind[j];
				ind[i].noderef = c;
				ind[i].houseref = h;
			  ind[i].X.resize(nfix);
				for(fi = 0; fi < nfix; fi++){
					switch(fi){
						case 0:  // Uses log of the population density 
							ind[i].X[fi] = log(data.house[h].density);
							break;
						/*
						case 0:  // Uses sex as a simple fixed effect
							ind[i].X[fi] = short(2*ran());
						  break;
							*/
					}
				}
			}
			
		}		
	}

	for(fi = 0; fi < nfix; fi++){                 // Shifts X to give a population average of 0
		av = 0;
		for(i = 0; i < ind.size(); i++) av += ind[i].X[fi];
		av /= ind.size();
		for(i = 0; i < ind.size(); i++) ind[i].X[fi] -= av;
	}
	
	for(l = 0; l < level; l++){
		cmax = lev[l].node.size();
		for(c = 0; c < cmax; c++) lev[l].add.push_back(0); 
	}
	
	subpop.resize(Cfine);                           // Defines the populations of individuals in the finescale nodes 
	for(c = 0; c < Cfine; c++){
		kmax = lev[level-1].node[c].houseref.size();
		for(k = 0; k < kmax; k++){			
			h = lev[level-1].node[c].houseref[k];
			for(i = 0; i < data.house[h].ind.size(); i++) subpop[c].push_back(data.house[h].ind[i]);
		}
	}
}
  
/// Defines the relative susceptibility of individuals
void POPTREE::setsus(MODEL &model)     
{
	long i, fi, l, c, cc, j, jmax;
	double val, sum;
	
	for(i = 0; i < ind.size(); i++) ind[i].sus = 0;
	
	for(fi = 0; fi < nfix; fi++){
		val = model.param[model.fix_sus_param[fi]].val;
		for(i = 0; i < ind.size(); i++) ind[i].sus += ind[i].X[fi]*val;
	}
	
	for(i = 0; i < ind.size(); i++) ind[i].sus = exp(ind[i].sus);
	
	l = level-1;                                  // Places the sumes susceptibilities onto the nodes
	for(c = 0; c < Cfine; c++){
		sum = 0; for(j = 0; j < subpop[c].size(); j++) sum += ind[subpop[c][j]].sus;
		lev[l].node[c].sussum = sum;
	}
	
	for(l = level-2; l >= 0; l--){
		for(c = 0; c < lev[l].node.size(); c++){
			sum = 0;
			jmax = lev[l].node[c].child.size();
			for(j = 0; j < jmax; j++){
				cc = lev[l].node[c].child[j];
				sum += lev[l+1].node[cc].sussum;
			}
			lev[l].node[c].sussum = sum;
		}
	}
}

/// Defines the relative infectivity of individuals
void POPTREE::setinf(MODEL &model)    
{
	long i, fi;
	double val;
	
	for(i = 0; i < ind.size(); i++) ind[i].inf = 0;
	
	for(fi = 0; fi < nfix; fi++){
		val = model.param[model.fix_inf_param[fi]].val;
		for(i = 0; i < ind.size(); i++) ind[i].inf += ind[i].X[fi]*val;
	}
	
	for(i = 0; i < ind.size(); i++) ind[i].inf = exp(ind[i].inf);
}