/*
 * And.cpp
 *
 *  Created on: Aug 12nd, 2014
 *      Author: Qi
 */

#include "And.h"
#include "globals.h"
#include "profiling.h"
#include "utils.h"
using namespace std;

void And::operator() (lptrArray& lps, const int topK, QpResult* res) {
	// initial sorting by did
	lps.sort();

	int smallest_did = lps[0]->did;
	int af = 1;
	float final_score = 0;

	// initialize results heap
	for ( int i = 0; i < topK; ++i)  {
		res[i].did = -1;
		res[i].score = -1.0;
	}

	while(true) {
		// cout<<"inhere?"<<endl;
		// set round's threshold
		const float threshold = res[topK-1].score;
		
		// string terms[2];
		// float scores[2];
		// int freq[2];

		// scores[0] = 0;
		// scores[1] = 0;
		// freq[0] = 0;
		// freq[1] = 0;

		float frequency = 0;
		float score = 0;

		// check for termination condition
		if (smallest_did >= CONSTS::MAXD)
			break;

		// initialize final score
		final_score = 0.0f;

		//check if all lists are in the intersection
		for (int i=0; i<lps.size(); ++i){
			if (lps[i]->did != smallest_did) {
				af = 0;
				break;	
			}
		}
		// cout<<"af: "<<af<<endl;
		// if not, advance all the lists, enter another round
		if(af == 0){
			for (int i=0; i<lps.size(); ++i){
			 	lps[i]->did = lps[i]->nextGEQ( smallest_did + 1 );
		 }
		 	lps.sort();
		 	smallest_did = lps[0]->did;
		 	// cout<<"smallest_did: "<<smallest_did<<endl;
		 	// int test;
		 	// cin>>test;
		 	af = 1;
		 	continue;
		}

		// evaluate all dids with did == smallest_did
		for (int i=0; i<lps.size(); ++i){

			// terms[i] = lps[i]->term;

			if (lps[i]->did == smallest_did) {
				//PROFILER(CONSTS::EVAL);
				//PROFILER(CONSTS::GETFREQ);
				const float frequency = lps[i]->getFreq();
				const float score = lps[i]->calcScore(frequency,pages[smallest_did]);
				// scores[i] = score;
				// freq[i] = frequency;
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
			// res[j+1].setRQi(smallest_did,final_score,terms,scores,freq);
			res[j+1].setR(smallest_did,final_score);
		}

		// sort list by did
		lps.sort();

		// set new smallest did for the next round
		smallest_did = lps[0]->did;
	} //end while
}


//  /*
//  * And.cpp
//  *
//  *  Created on: Aug 12nd, 2014
//  *      Author: Qi
//  */

// #include "And.h"
// #include "globals.h"
// #include "profiling.h"
// #include "utils.h"
// using namespace std;

// void And::operator() (lptrArray& lps, const int topK, QpResult* res) {
// 	// initial sorting by did
// 	lps.sort();

// 	int smallest_did = lps[0]->did;
// 	int af = 1; //whether the dids are found in all the lists
// 	float final_score = 0;

// 	// initialize results heap
// 	for ( int i = 0; i < topK; ++i)  {
// 		res[i].did = -1;
// 		res[i].score = -1.0;
// 	}

// 	const int size = lps.size();
// 	// cout<<"How many terms: "<<size<<endl;
// 	// string terms[size];
// 	// float scores[size];
// 	// int freq[size];
// 	int frequency = 0;
// 	float score = 0;


// 	while(true) {
// 		// cout<<"smallest did: "<<smallest_did<<endl;
// 		// set round's threshold
// 		const float threshold = res[topK-1].score;

// 		// check for termination condition
// 		if (smallest_did >= CONSTS::MAXD)
// 			break;

// 		// initialize final score
// 		final_score = 0.0f;

// 		//check if all lists are in the intersection
// 		for (int i=0; i<lps.size(); ++i){
// 			// cout<<"term: "<< lps[i]->term <<": "<<lps[i]->did<<endl;
// 			if (lps[i]->did != smallest_did) {
// 				af = 0;
// 				break;	
// 			}
// 		}
// 		// cout<<"af: "<<af<<endl;
// 		//if not, advance all the lists who has the smallest id, enter another round
// 		if(af == 0){
// 			for (int i=0; i<lps.size(); ++i){
// 			 if (lps[i]->did == smallest_did) {
// 			 	lps[i]->did = lps[i]->nextGEQ( smallest_did + 1 );
// 			 	// cout<<"smallest_did: "<<smallest_did<<" term: "<< lps[i]->term<<": "<<lps[i]->did<<endl;
// 			}
// 		 }
// 		 	lps.sort();
// 		 	smallest_did = lps[0]->did;
// 		 	// cout<<"smallest_did: "<<smallest_did<<endl;
// 		 	// int test;
// 		 	// cin>>test;
// 		 	af = 1;
// 		 	continue;
// 		}

// 		// evaluate all dids with did == smallest_did
// 		for (int i=0; i<size; ++i){
// 			if (lps[i]->did == smallest_did) {
// 				//PROFILER(CONSTS::EVAL);
// 				//PROFILER(CONSTS::GETFREQ);
// 				const float frequency = lps[i]->getFreq();
// 				const float score = lps[i]->calcScore(frequency,pages[smallest_did]);
// 				final_score += score;

// 				// move safely to the next did
// 				lps[i]->did = lps[i]->nextGEQ( smallest_did + 1 );
// //				cout<<"next did: "<<lps[i]->did<<endl;
// 				//PROFILER(CONSTS::NEXTGEQ);
// 			}
// 		}

// 		// if calculated score more than threshold, heapify
// 		if (Fcompare(final_score, threshold)==1) {
// 			//PROFILER(CONSTS::HEAPIFY);
// 			int j;
// 			for (j = topK-2; (j >= 0) && (Fcompare(final_score, res[j].score)==1); j--)
// 				res[j+1]=res[j];
// 			res[j+1].setR(smallest_did,final_score);
// 		}

// 		// sort list by did
// 		lps.sort();

// 		// set new smallest did for the next round
// 		smallest_did = lps[0]->did;
// 	} //end while
// }

