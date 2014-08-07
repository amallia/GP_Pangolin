/*
 * orqi.h
 *
 *  Created on: July 2nd, 2014
 *      Author: qi
 */

#ifndef ORQI_H_
#define ORQI_H_

#include "PostingOriented_BMW.h"

class orqi{
private:
	unsigned int* pages;

public:
	orqi(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, const int topk, QpResult* r);
};

#endif /* ORQI */
