/*
 * DataAns.h
 *
 *  Created on: May 14, 2014
 *      Author: Qi
 */

#ifndef DATAANS_H_
#define DATAANS_H_

#include "PostingOriented_BMW.h"

using namespace std;

struct info{
	int freq;
	int rank;
	float impact;
};

struct fr{
	int did;
	int freq;
	float impact;
};

class DataAns{
private:
	unsigned int* pages;
	map< string, map <int, info> > cache;


public:
	DataAns(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, int qn);
};

#endif /* DATAANS_H_ */
