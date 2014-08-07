/*
 * orqi.cpp
 *
 *  Created on: July 2nd, 2014
 *      Author: Qi
 */

#include "orqi.h"
#include "globals.h"
#include "profiling.h"
#include "utils.h"
using namespace std;

void orqi::operator() (lptrArray& lps, const int topK, QpResult* res) {
	// initial sorting by did
	lps.sort();

	int smallest_did = lps[0]->did;
	float final_score = 0;

	// for(int k = 0; k<size; k++){
	// 	terms[k] = lps[k]->term;
	// }

	// initialize results heap
	for ( int i = 0; i < topK; ++i)  {
		res[i].did = -1;
		res[i].score = -1.0;
	}

	while(true) {
		// set round's threshold
		const float threshold = res[topK-1].score;

		string terms[2];
		float scores[2];
		int freq[2];

		scores[0] = 0;
		scores[1] = 0;
		freq[0] = 0;
		freq[1] = 0;

		float frequency = 0;
		float score = 0;


		// check for termination condition
		if (smallest_did >= CONSTS::MAXD)
			break;

		// initialize final score
		final_score = 0.0f;

		// evaluate all dids with did == smallest_did
		for (int i=0; i<lps.size(); ++i){

			terms[i] = lps[i]->term;

			if (lps[i]->did == smallest_did) {
				//PROFILER(CONSTS::EVAL);
				//PROFILER(CONSTS::GETFREQ);
				frequency = lps[i]->getFreq();
				score = lps[i]->calcScore(frequency,pages[smallest_did]);
				scores[i] = score;
				freq[i] = frequency;
				final_score += score;

				// move safely to the next did
				lps[i]->did = lps[i]->nextGEQ( smallest_did + 1 );
//				cout<<"next did: "<<lps[i]->did<<endl;
				//PROFILER(CONSTS::NEXTGEQ);
			}
		}

		// if calculated score more than threshold, heapify
		if (Fcompare(final_score, threshold)==1) {
			//PROFILER(CONSTS::HEAPIFY);
			int j;
			for (j = topK-2; (j >= 0) && (Fcompare(final_score, res[j].score)==1); j--)
				res[j+1]=res[j];
			res[j+1].setRQi(smallest_did,final_score,terms,scores,freq);
			// int a;
			// cin>>a;
			// res[j+1].setR(smallest_did,final_score);
		}

		// sort list by did
		lps.sort();

		// set new smallest did for the next round
		smallest_did = lps[0]->did;
	} //end while
}
