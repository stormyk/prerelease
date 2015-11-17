#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <utility>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "hypNHX.h"
#define NHXPAD 7

typedef std::vector <std::string> VS;
typedef std::deque <std::string> DS;
typedef std::vector <int> VI;
typedef std::pair <std::string, int> PSI;

struct sort_pred {
    bool operator()(const PSI &left, const PSI &right) {
        return left.second > right.second; // > to have decending order
    }
};
/*
std::string LXVDisplay(std::string input, int depth){
	std::string result;
	std::string nhxData;
	std::string branchLength;
	std::string name;
	std::pair <int,int> nhxEnds=std::make_pair(input.find("[")+NHXPAD,input.find("]"));
        nhxData=input.substr(nhxEnds.first,nhxEnds.second-nhxEnds.first);	
	name=input.substr(0,input.find(":"));
	nhxData.replace(nhxData.find("="),1,"\t");	
	branchLength=input.substr(0,input.find("["));
	branchLength=branchLength.substr(branchLength.find(":")+1);

	char  buffer[512];
	sprintf(buffer,"%d\t%s\t1\thtml\tBL\t%s\t%s",depth,name.c_str(),branchLength.c_str(),nhxData.c_str());
	result.assign(buffer);
	return result;
}
*/
void nhx::addEntry(std::string input, int depth){
        std::string result;
        std::string nhxData;
        std::string branchLength;
        std::string name;
        std::pair <int,int> nhxEnds=std::make_pair(input.find("[")+NHXPAD,input.find("]"));
        nhxData=input.substr(nhxEnds.first,nhxEnds.second-nhxEnds.first);
        name=input.substr(0,input.find(":"));
        nhxData.replace(nhxData.find("="),1,"\t");
       for(size_t i=0;i<60;i++){ //up to 60 = signs handled.
		 if (-1==nhxData.find("=")){break;}
		 nhxData.replace(nhxData.find("="),1,"\t");
	 }
        branchLength=input.substr(0,input.find("["));
        branchLength=branchLength.substr(branchLength.find(":")+1);

      //  char  buffer[512];
//        sprintf(buffer,"%d\t%s\t1\thtml\tBL\t%s\t%s",depth,name.c_str(),branchLength.c_str(),nhxData.c_str());
//        result.assign(buffer);
//	lvhist.push_back( result);
	names.push_back( name);
	branchLen.push_back(branchLength);
	dnds.push_back(nhxData);
	depthData.push_back(depth);	
}
VS nhx::getLvhist(){
	//VS result;
	//std::string temp;
	//temp.assign("0 ERROR 0 color0 0 0");result.push_back(temp);
	//return result;
	//VS tempx;
	//char  buffer[512];
	//std::string temps;
	//sprintf(buffer,"0 test 0 color57 0 0");
	//temps.assign(buffer);
	//tempx.push_back(temps);
	//	return (tempx);
	if (lvhist.size()==branchLen.size()){ return lvhist;}
	genLvhist();
	return lvhist;
}
void nhx::genLvhist(){
	float val;
	float max;
	float min;
	float step;
	std::vector <float> colorVal;
	std::string temp;
	int color;
	for(size_t i=0;i<branchLen.size();i++){
		val=atof(branchLen[i].c_str());
		colorVal.push_back(val);
		if (1>=i){
			if (0==i){continue;} //we dont want the zero

			min=val;
			max=val;
		
		}
		if (val<min) min=val;
		if (val>max) max=val;
	}
	if (((max-min)/5000)<=0.0){
		step=1.0;// avoid div by zero error
	}
	else{
		step=(max-min)/4095.0;
	}
	lvhist.resize(0);
      char  buffer[512];
	for(size_t i=0;i<branchLen.size();i++){
		color=(colorVal[i]-min)/step;
		if (color>4095){color=4095;}
		if (color<0){color=0;}
		sprintf(buffer,"%d %s 1 color%d BL %s %s",depthData[i],names[i].c_str(),color,branchLen[i].c_str(),dnds[i].c_str());
		//sprintf(buffer,"%d %s_ 0 color7 0 0",depthData[i],names[i].c_str());
		temp.assign(buffer);
		lvhist.push_back( temp);
	}
}
void nhx::parseNewick(const std::vector <char> input){
	VI depthList; //index of active ('s
	std::vector<PSI> tokens;
	char lastToken='X';
	size_t lParenIndex=-1;
	size_t lastLParenIndex=-1;
	std::string entry;
//  Get list of labels for Left Parens;	
	for(size_t i=0;i<input.size();i++){
		if ('\n'==input[i]){continue;}
		bool isToken=false;
		if ('('==input[i]){
			depthList.push_back(i);
			isToken=true;
		}
		if (')'==input[i]){
			lParenIndex=depthList.back();
			depthList.pop_back();
			isToken=true;
		}
		if (','==input[i]){isToken=true;}
		if (';'==input[i]){isToken=true;}
		if (!isToken){//make Label Longer
			entry.append(1,input[i]);
		}
		else {
			if (lastToken==')'){
				tokens.push_back(make_pair(entry,lastLParenIndex));
			}
			lastToken=input[i];
			entry.assign("");
		}
		lastLParenIndex=lParenIndex;
	}
	std::sort(tokens.begin(), tokens.end(), sort_pred());
//////////////////////////////////////////////////////////////////////////////////
//  L parens Labeled
/////////////////////////////////////////////////////////////////////////////////
//
//	for (size_t i=0;i<tokens.size();i++){
//		//std::cout<< tokens[i].first <<"     " << tokens[i].second <<std::endl;
//		std::cout<< addEntry(tokens[i].first,tokens[i].second)<<std::endl;
//	}
//	std::cout << "##############################" << std::endl;
//////////////////////////////////////////////////////////////////////////////////
//  split it out
/////////////////////////////////////////////////////////////////////////////////

	int depth=0;
	int adjust;  //keeping depth right for closing right paren
	for(size_t i=0;i<input.size();i++){
		adjust=0;
		bool isToken=false;
		if ('\n'==input[i]){continue;}
		if ('('==input[i]){
			addEntry(tokens.back().first,depth);
		//	std::cout<< addEntry(tokens.back().first,depth)<<std::endl;
			tokens.pop_back();
			depth++;
			isToken=true;
			lastToken=input[i];
			continue;
		}
		if (')'==input[i]){
			depth--;
			adjust=1;
			isToken=true;
		}
		if (','==input[i]){isToken=true;}
		if (';'==input[i]){isToken=true;}
		if (!isToken){//make Label Longer
			entry.append(1,input[i]);
		}
		else {
			if (')'==lastToken){
				lastToken=input[i];
				entry.assign("");
				continue;
			}
			if (entry.size()<1) continue;
			addEntry(entry,depth+adjust);
			//std::cout<< addEntry(entry,depth+adjust)<<std::endl;
				lastToken=input[i];
			entry.assign("");
		}
	}
}
