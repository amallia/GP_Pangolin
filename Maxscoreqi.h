/*
 * Maxscoreqi.h
 *
 *  Created on: June 27, 2014
 *      Author: Qi
 */

#ifndef MAXSCOREQI_H_
#define MAXSCOREQI_H_

#include "PostingOriented_BMW.h"

class Maxscoreqi{
private:
	unsigned int* pages;
	inline void evaluate(lptrArray& lps, QpResult* res, const float& threshold, const int topK, const int& num_of_essential_lists, int& smallest_did, std::vector <float>& lists_maxscore, const float& current_prefix_sum_max_score, bool& check_for_new_essential_list, const int& threshold_unknown, const int& comparison, int& next_smallest_did) MYNOINLINE;
	inline bool find_smallest_did_in_essential_set(lptrArray& lps, const int& num_of_essential_lists, int& smallest_did) MYNOINLINE;
	inline void find_new_essential_lists(unsigned int& num_essential_lists, unsigned int& num_of_non_essential_lists, std::vector <float>& prefix_sum_max_score, lptrArray& lps, const float& threshold, bool& essential_list_added, float& parameter);

public:
	Maxscoreqi(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, const int k, QpResult* r, const float& threshold);
};

#endif /* MAXSCOREQI_H_ */
