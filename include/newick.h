#pragma once

#include <vector>
#include <deque>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cctype>
using namespace std;
//vector <char> expandNewick(vector <char> &input, int x){expandNewick(input);}// old version compatability
vector <char> expandNewick(vector <char> &input){
	// non-recursive expander
	vector <char> result;
	vector <char> sigil;
	vector <size_t> sigilLocation;
	vector <bool> sigilTag;
//	enum status {EMPTY,FOUND,UNNEEDED };
	bool BROKEN=false;

	//STEP 1 find and chart sigils in the newick file
	for(size_t i=0;i<input.size();i++){
		bool isSigil=false;
		if (')'==input[i]){ isSigil=true;}
		if ('('==input[i]){ isSigil=true;}
		if (','==input[i]){ isSigil=true;}
		if (';'==input[i]){ isSigil=true;}
		if (isSigil){
			sigil.push_back(input[i]);
			sigilLocation.push_back(i);
			sigilTag.push_back(false);
		}

	}
	//STEP 2a  find the sigils that would need tags to their right IGNORING THE TEXT
	for(size_t i=0;i+1<sigil.size();i++){
		char current=sigil[i];
		char next=sigil[i+1];

		char leftIndex=sigilLocation[i];
		char rightIndex=sigilLocation[i+1];
		//Setting the tags to false is for readability only.
		if ('('==current){//  ((  good (, needs () error (; error
			if ('('==next){ sigilTag[i]=true; continue;}
			if (','==next){ sigilTag[i]=false; continue;}
			if (')'==next){ BROKEN=true; continue;}
			if (';'==next){ BROKEN=true; continue;}
		}
		if (')'==current){//  )) needs ), needs ); needs )( error 
			if (')'==next){ sigilTag[i]=false; continue;}
			if (','==next){ sigilTag[i]=false; continue;}
			if (';'==next){ sigilTag[i]=false; continue;}
			if ('('==next){ BROKEN=true; continue;}
		}
		if (','==current){//  ,) needs ,, needs ,( needs ,; error 
			if (')'==next){ sigilTag[i]=false; continue;}
			if (','==next){ sigilTag[i]=false; continue;}
			if ('('==next){ sigilTag[i]=true; continue;}
			if (';'==next){ BROKEN=true; continue;}
			if ('('==next){ BROKEN=true; continue;}
		}
	//STEP 2b  check if TEXT provides TAGS on each sigil.
	}	
	for(size_t i=0;i+1<sigil.size();i++){
		if (sigilTag[i]){continue;}// no point in checking for (label(
		bool colonFound=false;
		for(size_t j=sigilLocation[i];j<sigilLocation[i+1];j++){
			if (colonFound){continue;}
			char current=input[j];
			if (':'==current){ colonFound=true; continue;}
			if (isalnum(current)){sigilTag[i]=true;}
		}	

	}

	//STEP3 Adding in Tags
	sigilLocation.push_back(input.size()); // prevent a run-over
	size_t locIndex=0;
	size_t nodeNum=0;
	string nodeName;
	size_t index=sigilLocation[locIndex];
	char buffer[512];
	for (size_t i=0;i<input.size();i++){
		result.push_back(input[i]);
		if( ';'==input[i]) {continue;}
		if (i==index){
			if (!sigilTag[locIndex]){
				sprintf(buffer,"NODE_%d",nodeNum);
				nodeName.assign(buffer);
				nodeNum++;
				for(size_t j=0;j<nodeName.size();j++){
					result.push_back(nodeName[j]);
				}
				
			}
			locIndex++;
			index=sigilLocation[locIndex];
		}
		
	}
	return result;
}



/*
// expandNewick works correctly, but slow 
// replacing with a clever algorithm
vector <char> expandNewick(vector <char> input, int & firstNode){
	vector <char> result;
	vector <vector <char> > elements;
	vector <char> temp;
	char buffer[512];
	string nodeName;
	// SOLVE BASE CASE HERE
	if (input.size()==0) {
		sprintf(buffer,"NODE_%d",firstNode);
		nodeName.assign(buffer);
		for(size_t i=0;i<nodeName.size();i++){
			result.push_back(nodeName[i]);
		}
		firstNode++;
		return result;
	}
	// SOLVE CASE of (Label| empty): branchlength
	size_t left=0;
	size_t right=0;
	size_t colon;
	size_t comma;
	size_t depth=0;
	bool parenFound=false;
	bool colonFound=false;
	bool commaFound=false;
	for(size_t i=0;i<input.size();i++){
		if (input[i]==','){
			comma=i;
			if (0==depth){
				commaFound=true;
			}
			
		}
		if (input[i]==':'){
			colon=i;
			colonFound=true;

		
		}
		if (input[i]=='('){
			depth++;
		}
		if (!parenFound && input[i]=='('){
			left=i;
			parenFound=true;
		}
		if (input[i]==')'){ 
			right=i;
			depth--;
		}
	}// for i-input
	bool solo=true;  
	if (commaFound) {solo=false;}
	if (parenFound) {solo=false;}
	if (!colonFound){colon=input.size();}//last element
	if (solo) {
		temp=input;
		temp.resize(colon);// chop off the distance for now.
		if (temp.size()>0&&colonFound){return input;}
		if (temp.size()>0){return temp;}
		// temp is = zero now, and
		if (colonFound){
			temp=expandNewick(temp,firstNode);
			for(size_t i=colon;i<input.size();i++){
				temp.push_back(input[i]);
			}
			return(temp);
		}
	}
	////////////////////////////////////////////////////////////////
	//  x  mammal   bird:0223  :445         are now handled
        // now we handle the cases
        //   (*)x
        //   (*)mammal
        //   (*)bird:0223
        //   (*):445
	//    Note the internals of the parens are procrestinated to 
        //    deeper sections of the function.
        ///////////////////////////////////////////////////////////////
	//if ((right>comma)||(!commaFound)){
	if (!commaFound){
		//ensures that the comma is inside of the parenthesis.
		temp.resize(0);
		for(size_t i=right;i<input.size();i++){
			if (i==right){continue;}// skip the right paren
			temp.push_back(input[i]);
		}
		temp=expandNewick(temp,firstNode);
		vector <char> LHS;
		for(size_t i=left;i<right;i++){
			if (i==left){continue;}// skip the left paren
			LHS.push_back(input[i]);
		}
		LHS=expandNewick(LHS,firstNode);
		result.resize(0);
		result.push_back('(');
		for(size_t i=0;i<LHS.size();i++){
			result.push_back(LHS[i]);
		}//for LHS
		result.push_back(')');
		for(size_t i=0;i<temp.size();i++){
			result.push_back(temp[i]);
		}// for temp

		return result;	
	}
	/////////////////////////////////////////////////////////
	//  from this point up we can handle
	//  ((():17)a:22)b); without a problem
	//  now we handle adding in commas.
	/////////////////////////////////////////////////////////
	// The concept is to handle one comma at a time
        // lisp style
        //  so (a,b,c,d,(e,f));
        // is handled as function [a,b,c,d]  COMMA the function [(e,f)]
	// which creates::,(e,f)NODE_0 (ok node1 when it's done)
        // search for the zero-depth rightmost commas, and work the sub-parts.
	/////////////////////////////////////////////////////////
	
	size_t bestCommaIndex=input.size();
	depth=0;	
	for(size_t i=0;i<input.size();i++){
		if (input[i]=='('){
			depth++;
		}
		if (input[i]==')'){
			depth--;
		}
		if (input[i]==','){
			if (0==depth){
				bestCommaIndex=i;
			}
		}
		//cout <<input[i];
	}
	if (bestCommaIndex<input.size()){
		right=bestCommaIndex;
		left=0;
		//ensures that the comma is inside of the parenthesis.
		temp.resize(0);
		for(size_t i=right;i<input.size();i++){
			if (i==right){continue;}// skip the comma
			temp.push_back(input[i]);
		}
		temp=expandNewick(temp,firstNode);
		vector <char> LHS;
		for(size_t i=left;i<right;i++){
	//		if (i==left){continue;}// skip the left paren
			LHS.push_back(input[i]);
		}
		LHS=expandNewick(LHS,firstNode);
		result.resize(0);
//result.push_back('(');
		for(size_t i=0;i<LHS.size();i++){
			result.push_back(LHS[i]);
		}//for LHS
//cout <<"DEBUG-LHS2"<<endl;
		result.push_back(',');
		for(size_t i=0;i<temp.size();i++){
			result.push_back(temp[i]);
		}// for temp

		return result;	
	}
	return (input);
}
*/

vector <char> getNewickSubgraph(vector <char> inGraph, string name){//sjk2011
//	int tempNum=0;
	inGraph=expandNewick(inGraph);
	//printf("DEBUG :x:>");
	//for (size_t i=0;i<inGraph.size();i++){
		//printf("%c",inGraph[i]);
//	}
//	printf("\n");
	//ASSUMPTIONS:: NAME IS IN VECTOR
	// IF node22 is passed it must be processed first to a newick form
	//  (a,b(c,d)); -> (a,b,(c,d)node2)node1;
	// vector <char> getNewickFormal(vector <char> normalNewick
	//int index=0;
	//vector <size_t> depthList;
	size_t depth=0;
	size_t last=(inGraph.size()-name.size())-1;//so we dont walk off the end of vector
	vector <char> result;
	size_t left=0;
	size_t right=0;
	size_t trueRight=0;
	int index=-1;
	for(size_t i=0;i<last;i++){
		bool match=true;
		bool leftOK=false;
		bool rightOK=false;
		for(size_t j=0;((j<name.size())&&match);j++){
			if (inGraph[i+j]!=name[j]){
				match=false;				
			//	if (!match) {printf("DEBUG:: %c  %c  FAIL\n",inGraph[i+j],name[j]);}
			}
		//	if (match) {printf("DEBUG:: %c  %c  MATCH\n",inGraph[i+j],name[j]);}
			
		}
		if(!match){ continue;}
			if (match && i>0){
//				printf("FLAG:117:\n");
				

				left=i-1;
				right=i+name.size();
				if (inGraph[right]==':'){// move off of the length.
					while ((right<inGraph.size())&&
							((inGraph[right]<='9' && inGraph[right] >='0')||
							  inGraph[right]=='.')
							)
						right++;
				}
				if (inGraph[left]==')') {leftOK=true;}

				if (inGraph[right]==',') {rightOK=true;}
				if (inGraph[right]==')') {rightOK=true;}
				if (inGraph[right]==';') {rightOK=true;}
				if (inGraph[right]==':') {rightOK=true;}
//				printf("DEBUG: MATCH! %c %c\n",inGraph[left],inGraph[right]);
				if (leftOK && rightOK) {
					for(size_t j=0;j<inGraph.size();j++){
						//printf("%c",inGraph[j]);
					}
				}
//				printf("\nSTART OF SUB:");
				if (leftOK &&rightOK){
					for(size_t j=left;j<=right;j++){
						//printf("%c",inGraph[j]);
					}
//					printf("\n");
				}

			}
			if (leftOK && rightOK){
				trueRight=right;
//				printf("\nDEBUG LOKROK\n");
				printf(">::>");
//				if (right>inGraph.size()) {printf("DEBUG::OVERFLOW\n");}
				depth=0;
				index=-1;
				for(int j=left;j>=0;j--){
					if (inGraph[j]==')') {depth++;}// BACKWARDS TIME
					if (inGraph[j]=='(') {depth--;}// BACKWARDS TIME
			//		printf("DEBUG %c DEPTH %d\n",inGraph[j],depth);
					if (0==depth) {index=j;j=-1;} // find first zero depth and END					
				}
				if (-1==index){
					printf("TREE FORM BROKEN\n");
				}
				
				
			}
		
	}// for i
//	printf("FLAG:118:\n");
	//string debugLine="(()ERROR)n;";
	if (index>=0){
		result.resize(0);
//		printf ("\nDEBUG::SUBGRAPH RETURN->");
		for(size_t i=index;i<trueRight;i++){
			result.push_back(inGraph[i]);
		//	printf("DEBUG:( subgraph) %c",inGraph[i]);
		}
//		printf(";\n");
		result.push_back(';');
		return result;
	}
	// IT DIDNT WORK  MAKE AN ERROR MSG GRAPH
	string debugLine="(()ERROR_";
		vector <string> junk;
		junk.push_back(debugLine);
        vector <char> tmpchrv;
		for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}
		char buffer[500];
		itoa(depth,buffer,10);
		debugLine.assign(buffer);
		for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}
		tmpchrv.push_back('#');
		buffer[500];
		
		itoa(inGraph.size(),buffer,10);
		debugLine.assign(buffer);
		for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}
		tmpchrv.push_back('#');
		itoa(left,buffer,10);
		debugLine.assign(buffer);
		for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}
		tmpchrv.push_back('#');
		itoa(right,buffer,10);
		debugLine.assign(buffer);
		for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}

		tmpchrv.push_back('#');
		debugLine=")n;";
		for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}
	    return(tmpchrv);
	
	return inGraph;

}

/*
vector <char> expandNewick(vector <char> & input, int & firstNode,int front,int back){
	vector <char> result;
	vector <char> temp;
	char buffer[512];
	string nodeName;
	// SOLVE BASE CASE HERE
	if (0>=(back-front)) {
		sprintf(buffer,"NODE_%d",firstNode);
		nodeName.assign(buffer);
		for(size_t i=0;i<nodeName.size();i++){
			result.push_back(nodeName[i]);
		}
		firstNode++;
		return result;
	}
	// SOLVE CASE of (Label| empty): branchlength
	size_t left=0;
	size_t right=0;
	size_t colon;
	size_t comma;
	size_t depth=0;
	bool parenFound=false;
	bool colonFound=false;
	bool commaFound=false;
	for(size_t i=front;i<=back;i++){
		if (input[i]==','){
			comma=i;
			if (0==depth){
				commaFound=true;
			}
			
		}
		if (input[i]==':'){
			colon=i;
			colonFound=true;

		
		}
		if (input[i]=='('){
			depth++;
		}
		if (!parenFound && input[i]=='('){
			left=i;
			parenFound=true;
		}
		if (input[i]==')'){ 
			right=i;
			depth--;
		}
	}// for i-input
	bool solo=true;  
	if (commaFound) {solo=false;}
	if (parenFound) {solo=false;}
	if (!colonFound){colon=input.size();}//last element
	if (solo) {
		temp.resize(0);
		for(size_t i=front;i<=back;i++){
			temp.push_back(input[i]);
		}
		//temp=input;
		temp.resize(colon-(front-1));// chop off the distance for now.
		if (temp.size()>0&&colonFound){return input;}
		if (temp.size()>0){return temp;}
		// temp is = zero now, and
		if (colonFound){
			temp=expandNewick(input,firstNode,front,(colon-1));
			for(size_t i=colon;i<=back;i++){
				temp.push_back(input[i]);
			}
			return(temp);
		}
	}
	////////////////////////////////////////////////////////////////
	//  x  mammal   bird:0223  :445         are now handled
        // now we handle the cases
        //   (*)x
        //   (*)mammal
        //   (*)bird:0223
        //   (*):445
	//    Note the internals of the parens are procrestinated to 
        //    deeper sections of the function.
        ///////////////////////////////////////////////////////////////
	//if ((right>comma)||(!commaFound)){
	if (!commaFound){
		//ensures that the comma is inside of the parenthesis.
		temp.resize(0);
//		for(size_t i=right;i<=back;i++){
//			if (i==right){continue;}// skip the right paren
//			temp.push_back(input[i]);
//		}
		temp=expandNewick(input,firstNode,right+1,back);
		vector <char> LHS;
//		for(size_t i=left;i<right;i++){
//			if (i==left){continue;}// skip the left paren
//			LHS.push_back(input[i]);
//		}
		LHS=expandNewick(input,firstNode,left+1,right-1);
		result.resize(0);
		result.push_back('(');
		for(size_t i=0;i<LHS.size();i++){
			result.push_back(LHS[i]);
		}//for LHS
		result.push_back(')');
		for(size_t i=0;i<temp.size();i++){
			result.push_back(temp[i]);
		}// for temp

		return result;	
	}
	/////////////////////////////////////////////////////////
	//  from this point up we can handle
	//  ((():17)a:22)b); without a problem
	//  now we handle adding in commas.
	/////////////////////////////////////////////////////////
	// The concept is to handle one comma at a time
        // lisp style
        //  so (a,b,c,d,(e,f));
        // is handled as function [a,b,c,d]  COMMA the function [(e,f)]
	// which creates::,(e,f)NODE_0 (ok node1 when it's done)
        // search for the zero-depth rightmost commas, and work the sub-parts.
	/////////////////////////////////////////////////////////
	
	size_t bestCommaIndex=input.size();
	depth=0;	
	for(size_t i=front;i<=back;i++){
		if (input[i]=='('){
			depth++;
		}
		if (input[i]==')'){
			depth--;
		}
		if (input[i]==','){
			if (0==depth){
				bestCommaIndex=i;
			}
		}
		//cout <<input[i];
	}
	if (bestCommaIndex<input.size()){
		right=bestCommaIndex;
		left=0;
		//ensures that the comma is inside of the parenthesis.
		temp.resize(0);
//		for(size_t i=right;i<input.size();i++){
//			if (i==right){continue;}// skip the comma
//			temp.push_back(input[i]);
//		}

		temp=expandNewick(input,firstNode,right+1,back);
		vector <char> LHS;
//		for(size_t i=left;i<right;i++){
	//		if (i==left){continue;}// skip the left paren
			/// THIS WAS A BUG LEFT MUST BE IN HERE
			
//			LHS.push_back(input[i]);
		}
		LHS=expandNewick(input,firstNode,left,right-1);
		result.resize(0);
//result.push_back('(');
		for(size_t i=0;i<LHS.size();i++){
			result.push_back(LHS[i]);
		}//for LHS
//cout <<"DEBUG-LHS2"<<endl;
		result.push_back(',');
		for(size_t i=0;i<temp.size();i++){
			result.push_back(temp[i]);
		}// for temp

		return result;	
	}
	return (input);
}
*/

vector <char> newickConvertBootStrap (vector <char> input){
	vector <char> result;
	vector <char> buffer;
	//input.push_back(';'); //force final sigil parse
	char lastSigil='X';
	char current;
	bool bufferIsNum=true;
	bool dotFound=false;
	int bootStrapValue=0;
	size_t size=input.size();
	for(size_t i=0;i<size;i++){
		bool isSigil=false;
		bool validNum=false;
		current=input[i];
		if (input[i]=='('){isSigil=true;}
		if (input[i]==')'){isSigil=true;}
		if (input[i]==','){isSigil=true;}
		if (input[i]==':'){isSigil=true;}
		if (input[i]==';'){isSigil=true;}
		if (input[i]=='0'){validNum=true;}
		if (input[i]=='1'){validNum=true;}
		if (input[i]=='2'){validNum=true;}
		if (input[i]=='3'){validNum=true;}
		if (input[i]=='4'){validNum=true;}
		if (input[i]=='5'){validNum=true;}
		if (input[i]=='6'){validNum=true;}
		if (input[i]=='7'){validNum=true;}
		if (input[i]=='8'){validNum=true;}
		if (input[i]=='9'){validNum=true;}
		if (input[i]=='_'){validNum=true;}// spaces are tricky
		if (input[i]=='.'){
			if (dotFound){validNum=false;} // 2 dots in label stop a number.
			else {validNum=true;}
			dotFound=true;
		}
		if (isSigil){
			for(size_t j=0;j<buffer.size();j++){
				result.push_back(buffer[j]);// whatever the label was
			}
			buffer.resize(0); //clear the printed buffer.
			if (':'==lastSigil){validNum=false;}// skip branch lengths
			if (bufferIsNum){ // is a label, and a number
				result.push_back('_');
				result.push_back('B');
				result.push_back('S');
				result.push_back('V');
				result.push_back('-');
				char BootStrapString[10]; // 
				itoa(bootStrapValue,BootStrapString,10);
				if (true)             {result.push_back(BootStrapString[0]);}
				if (bootStrapValue>=10){result.push_back(BootStrapString[1]);}
				if (bootStrapValue>=100){result.push_back(BootStrapString[2]);}
				if (bootStrapValue>=1000){result.push_back(BootStrapString[3]);}
				if (bootStrapValue>=10000){result.push_back(BootStrapString[4]);}
				if (bootStrapValue>=100000){result.push_back(BootStrapString[5]);}
				if (bootStrapValue>=1000000){result.push_back(BootStrapString[6]);}
				bootStrapValue++;
			}
			result.push_back(current);
			
			lastSigil=current;
			bufferIsNum=true;
		} // if isSigil
		else {
			buffer.push_back(current);
			if (!validNum){bufferIsNum=false;}
		}
	}// for i to input.size
	return result;
}// function newickconvertBootStrap;