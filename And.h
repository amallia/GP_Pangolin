/*
 * exhaustiveOR.h
 *
 *  Created on: Aug 12nd, 2014
 *      Author: Qi
 */

#ifndef AND_H_
#define AND_H_

#include "PostingOriented_BMW.h"

class ExhaustiveOR{
private:
	unsigned int* pages;

public:
	And(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, const int topk, QpResult* r);
};

#endif /* AND_H_ */
