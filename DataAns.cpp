/*
 * DataAns.cpp
 *
 *  Created on: May 14, 2014
 *      Author: Qi
 */

#include "DataAns.h"
#include "globals.h"
#include "profiling.h"
#include "utils.h"
#include <cstring>
#include <string>
#include <fstream>

using namespace std;

bool mycf (fr a,fr b) { return (a.impact > b.impact); }

void DataAns::operator() (lptrArray& lps, int qn) {


	string queryline;
	string termline;
	int results[500];

    char * docid_s;
	// int docid;
	int count = 0;
	int test;

	ifstream input_stream;
	// input_stream.open("../../../Dropbox/WSDM_Index_Script/result_log");  //for or
	input_stream.open("../../../Dropbox/WSDM_Index_Script/500_cr");  //for cr

	ofstream output_stream;
	output_stream.open("../../../Dropbox/experiments/p_cr_35k", ofstream::app);

	for(int i=0; i < qn-1; ++i){
        input_stream.ignore(numeric_limits<streamsize>::max(),'\n');
    }

	getline(input_stream, queryline);

	char * ql = new char [queryline.length()+1];
    std::strcpy (ql, queryline.c_str());

	// cout<<queryline<<endl;


	docid_s = strtok (ql," ");

	while (docid_s != NULL)
  	{
    	// printf ("%s\n",docid_s);
 		results[count] = atoi(docid_s);   	
 		count++;
    	docid_s = strtok (NULL, " ,.-");
  	}

  	input_stream.close();

  	ifstream input_stream1;
	// input_stream1.open("/home/qi/Dropbox/experiments/4_dp_terms");
	input_stream1.open("../../../Dropbox/experiments/35k_dp_terms");
	vector<string> du_terms;

	while(getline(input_stream1, termline)){
		du_terms.push_back(termline);
	}

  	input_stream1.close();

  	// for(int i=0; i<500; i++){
  	// 	cout<<results[i]<<" ";
  	// }

  	// cout<<endl;


	// string::iterator itr = queryline.begin();
	// string::iterator start = itr;

	// while(itr != queryline.end() && !isspace(*itr)){
	// 			++itr;
	// }

	// docid_s = string(start, itr1);
	// docid = atoi(docid_s.c_str());



	for (int i=0; i<lps.size(); ++i){  //deal with all the terms in this query

	 map< string, map <int, info> >::iterator it;

	 it = cache.find(lps[i]->term);

	 if(it == cache.end()){ //put this term and its posting list with rank in the cache;

	 	// cout<<"cache miss, add to cache, term: "<<lps[i]->term<<endl;

		// cout<<"qid: "<<qn<<endl;

		// cout<<"qlen: "<<lps.size()<<endl;

		// cout<<"term: "<<lps[i]->term<<endl;

		// cout<<"listlen: "<<lps[i]->unpadded_list_length<<endl;

		vector < fr > ri; //Sorting to get the rank

		fr curr_fr;

		curr_fr.did = lps[i]->did;
		curr_fr.freq = lps[i]->getFreq();
		curr_fr.impact = lps[i]->calcScore(curr_fr.freq, pages[curr_fr.did]); 

		ri.push_back(curr_fr);

		while(true){

			// cout<<"did: "<<curr_fr.did<<" "<<"freq: "<<curr_fr.freq<<"impact: "<<curr_fr.impact<<" ";
			curr_fr.did = lps[i]->nextGEQ( curr_fr.did + 1 );
			curr_fr.freq = lps[i]->getFreq();
			curr_fr.impact = lps[i]->calcScore(curr_fr.freq,pages[curr_fr.did]); 
			ri.push_back(curr_fr);

			if(curr_fr.did>=CONSTS::MAXD)
				break;
			// cin>>test;
		}

		sort(ri.begin(), ri.end(), mycf);

		map <int, info> rm;

		info curr_info;

		for ( int j = 0; j < ri.size(); j++) {
            // cout<<"did: "<<ri[i].did <<" freq: "<<ri[i].freq<<" impact: "<<ri[i].impact<<endl;
            curr_info.freq = ri[j].freq;
            curr_info.rank = j + 1;
            curr_info.impact = ri[j].impact;
            rm[ri[j].did] = curr_info;
        }

      //   for (map< int,info >::iterator it=rm.begin(); it!=rm.end(); ++it)
    		// cout << "did: " << it->first << " => " << " freq: "<<it->second.freq <<" impact: "<<it->second.impact<<" rank: "<<it->second.rank<<endl;

    	cache[lps[i]->term] = rm;
    	// it = cache.find(lps[i]->term); //Now it's in cache

      }//end of if
    //   else
	   // cout<<"cache hit, "<<"term: "<<lps[i]->term<<endl;



      // for(int k=0; k<500; k++){
      // 	map <int, info> :: iterator iter;
      // 	iter = it->second.find(results[k]);
      // 	if(iter!=it->second.end()){
      // 	  // cout<<"did: "<<iter->first<<" freq: "<<iter->second.freq<< " impact: "<<iter->second.impact<<" rank: "<<iter->second.rank<<endl;
      // 	  cout<<qn<<" "<<lps.size()<<" "<<lps[i]->term<<" "<<lps[i]->unpadded_list_length<<" "<<iter->first<<" "<<iter->second.freq<<" "<<iter->second.impact<<" "<<iter->second.rank<<endl;
      // 	}
      // 	else
      //     cout<<results[k]<<" did miss"<<endl;
      // }

	}

	for(int k=0; k<500; k++){

	  for (int i=0; i<lps.size(); ++i){

	  	map< string, map <int, info> >::iterator it;
	 	it = cache.find(lps[i]->term);

      	map <int, info> :: iterator iter;
      	iter = it->second.find(results[k]);
      	if(iter!=it->second.end()){
      	    // cout<<qn<<" "<<lps.size()<<" "<<k+1<<" "<<lps[i]->term<<" "<<lps[i]->unpadded_list_length<<" "<<iter->first<<" "<<iter->second.freq<<" "<<iter->second.impact<<" "<<iter->second.rank<<endl;
      		output_stream<<qn<<" "<<lps.size()<<" "<<k+1<<" "<<lps[i]->term<<" "<<lps[i]->unpadded_list_length<<" "<<iter->first<<" "<<iter->second.freq<<" "<<iter->second.impact<<" "<<iter->second.rank<<endl;
      	}
      	// else
       //    cout<<results[k]<<" did miss"<<endl;
  	   }
    }

    for(int i=0; i<lps.size(); ++i){
    	if(find(du_terms.begin(), du_terms.end(), lps[i]->term) == du_terms.end()){
    		// std::cout << "No 10, bummer\n";
    		// cout<<"single term: "<<lps[i]->term<<" erase."<<endl;
    		cache.erase(lps[i]->term);
    	}
		// else{
  //   		// std::cout << "I found a 10!\n";
  //   		cout<<"dup term: "<<lps[i]->term<<" keep in cache."<<endl;
		// }
    }

    output_stream.close();

		// cout<<"cache size: "<<cache.size()<<endl;
}
