/*
 * PostingOriented_BMW.h
 *
 *  Created on: July 2nd, 2014
 *      Author: qi
 */

#ifndef _BMF_H_
#define _BMF_H_

#include "pfor.h"
#include "ListIterator.h"
#include <string.h>

// Result structure
// struct QpResult {
// 	unsigned int did;
// 	float score;

// 	inline QpResult& operator=(const QpResult& rhs) {
// 		did=rhs.did;
// 		score=rhs.score;
// 		return *this;
// 	}

// 	inline bool operator>(const QpResult& rhs) const { return score>rhs.score; }
// 	inline bool operator<(const QpResult& rhs) const { return score<rhs.score; }
// 	QpResult(unsigned int d=CONSTS::MAXD+1, float s=-1.0) { did=d; score=s;}
// 	inline void setR(unsigned int d, float s) { did=d; score=s;}
// };

struct QpResult {
	unsigned int did;
	float score;
	string terms[2];
	float scores[2];
	int freq[2];


	inline QpResult& operator=(const QpResult& rhs) {
		did=rhs.did;
		score=rhs.score;
		for(int i = 0; i<2; i++){
			terms[i] = rhs.terms[i];
			scores[i] = rhs.scores[i];
			freq[i] = rhs.freq[i];
		}

		return *this;
	}

	inline bool operator>(const QpResult& rhs) const { return score>rhs.score;}
	inline bool operator<(const QpResult& rhs) const { return score<rhs.score;}
	QpResult(unsigned int d=CONSTS::MAXD+1, float s=-1.0) { did=d; score=s;}
	inline void setR(unsigned int d, float s) { did=d; score=s;}
	inline void setRQi(unsigned int d, float s, string terms_[2], float scores_[2], int freq_[2]) { 

		did=d; 
		score=s;
		
		// for(int i = 0; i<2; i++){
		// 	terms[i] = terms_[i];
		// 	scores[i] = scores_[i];
		// 	freq[i] = freq_[i];
		// }
		if(terms_[0].compare(terms_[1])<0){
			terms[0] = terms_[0];
			terms[1] = terms_[1];
			scores[0] = scores_[0];
			scores[1] = scores_[1];
			freq[0] = freq_[0];
			freq[1] = freq_[1];
		}else{
			terms[0] = terms_[1];
			terms[1] = terms_[0];
			scores[0] = scores_[1];
			scores[1] = scores_[0];
			freq[0] = freq_[1];
			freq[1] = freq_[0];
		}

	}

};

class PostingOriented_BMW {
private:
	unsigned int* pages;
	inline bool pickCandidate(lptrArray& lps, const float threshold, const int numOfTermsInQ, int& pivot, int& smallestMaxDid, float& sumOfBLockMaxesUptoPivot) MYNOINLINE;
	inline void handleReal(lptrArray& lps, const int pivot, float sumOfBLockMaxesUptoPivot,QpResult* res, const float threshold, const int topK) MYNOINLINE;
	inline bool handleFake(lptrArray& lps, const int pivot, const int smallestMaxDid, const int numOfTermsInQ) MYNOINLINE;

public:
	PostingOriented_BMW(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, const int k, QpResult* r);
};

#endif
