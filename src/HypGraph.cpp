
// Copyright 1998, Silicon Graphics, Inc. -- ALL RIGHTS RESERVED 
// 
// Permission is granted to copy, modify, use and distribute this
// software and accompanying documentation free of charge provided (i)
// you include the entirety of this reservation of rights notice in
// all such copies, (ii) you comply with any additional or different
// obligations and/or use restrictions specified by any third party
// owner or supplier of the software and accompanying documentation in
// other notices that may be included with the software, (iii) you do
// not charge any fee for the use or redistribution of the software or
// accompanying documentation, or modified versions thereof.
// 
// Contact sitemgr@sgi.com for information on licensing this software 
// for commercial use. Contact munzner@cs.stanford.edu for technical 
// questions. 
// 
// SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH RESPECT TO THIS
// SOFTWARE, EXPRESS, IMPLIED, OR OTHERWISE, INCLUDING WITHOUT
// LIMITATION, ALL WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE OR NONINFRINGEMENT. SILICON GRAPHICS SHALL NOT
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
// INCLUDING, WITHOUT LIMITATION, LOST REVENUES, LOST PROFITS, OR LOSS
// OF PROSPECTIVE ECONOMIC ADVANTAGE, RESULTING FROM THE USE OR MISUSE
// OF THIS SOFTWARE.
// 
// U.S. GOVERNMENT RESTRICTED RIGHTS LEGEND: 
// 
// Use, duplication or disclosure by the Government is subject to
// restrictions as set forth in FAR 52.227.19(c)(2) or subparagraph
// (c)(1)(ii) of the Rights in Technical Data and Computer Software
// clause at DFARS 252.227-7013 and/or in similar or successor clauses
// in the FAR, or the DOD or NASA FAR Supplement. Unpublished - rights
// reserved under the Copyright Laws of United States.
// Contractor/manufacturer is Silicon Graphics, Inc., 2011 N.
// Shoreline Blvd. Mountain View, CA 94039-7311.

#ifdef WIN32
#include <stdio.h>

#else
extern "C" {
#include <stdlib.h>
#include <stdio.h>
}
#endif
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

//NAMESPACEHACK
#include <iostream> //sjk2010
using namespace std;//sjk2010


#include "HypGraph.h"
#include "HypData.h"

#ifdef WIN32
#include <algorithm>
#else
#include <algo.h>
#endif //WIN32

///////////////////////////////////////////////////////////////////////////
//
// HypGraph class
//
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//                    int HypGraph::findUrl(string URL) const
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::findUrl(string URL) const
{
  map<string,int,less<string> >::const_iterator  nodeMapIter;
  //if (hd->loads>1){exit(1);}//X1-Debug SJK DEBUG
  nodeMapIter = nodetable.find(URL);

  //if (hd->loads>1){exit(1);}//SJK DEBUG  
  //if (&nodes!=NULL){
	if (nodeMapIter != nodetable.end()) {
		int index= (*nodeMapIter).second;
		//if (hd->loads>1){exit(1);}//SJK DEBUG
		HypNode * n1=nodes[index];
		
	    return(nodes[(*nodeMapIter).second]->getIndex());
	}
  //}
	//if (hd->loads>1){exit(1);}//X1-Debug SJK DEBUG
  return(-1);
}

//----------------------------------------------------------------------------
//                    int HypGraph::findNextUrl(string URL, int min) const
//............................................................................
//  
//----------------------------------------------------------------------------


int HypGraph::findNextUrl(string URL, int min) const
{
  
  int result=-1;
  int spareResult=-1;//best result ignoring the minimum (for looping to the front)
  //prevents a redo of the search if we run off the edge, or dont locate a match.
  //int redoStart=false;
  //int redo=true;
  bool caseSensitive=true;
  if (URL.c_str()[0]=='#'){      // CASE-Desensitize Control Character.
	  caseSensitive=false;
	  URL=URL.substr(1); // clips the # off
	  std::transform(URL.begin(),URL.end(),URL.begin(), std::tolower);
  }
  map<string,int,less<string> >::const_iterator  nodeMapIter;
  string temp;
  string tempCased;
  
  nodeMapIter=nodetable.begin();

  
  while (nodeMapIter != nodetable.end()){
	  
	  int	index		= nodes[(*nodeMapIter).second]->getIndex();
			temp		= nodes[(*nodeMapIter).second]->getId();
			tempCased	= temp;	  
    
	if (!caseSensitive){
		std::transform(tempCased.begin(),tempCased.end(),tempCased.begin(), std::tolower);	
	}
	if (tempCased.find(URL,0)!=string::npos){
		if (-1==spareResult){
			spareResult=index;
		}
		if (index< spareResult){
			spareResult=index;
		}
		if (index>=min){  
			if (-1==result){
				result=index;
			}
			if (index< result){
				result=index;
			}
		}
	}
	nodeMapIter++; // Do the next one.		
  } //While loop
  
   if (-1==result){
	   return spareResult;
   }// we have a solution no need to start from scratch.
   //spareresult will also return a -1 if absolutly nothing was found.
   return (result);  //-1 means Not Found
  
} 
///////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//                    int HypGraph::findLink(string id) const
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::findLink(string id) const
{
  map<string,int,less<string> >::const_iterator  linkMapIter;
  
  linkMapIter = linktable.find(id);
  if (linkMapIter != linktable.end())
    return(links[(*linkMapIter).second]->getIndex());
  return(-1);
}

//----------------------------------------------------------------------------
//           void HypGraph::newNode(HypNode *newnode, string id)           
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::newNode(HypNode *newnode, string id, std::vector <std::string> XA){
	newnode->set_XA(XA);
	newNode(newnode,id);
	return;
}
void HypGraph::newNode(HypNode *newnode, string id)
{
  int curlen = nodes.size();
  nodetable[id] = curlen;
  nodes.push_back(newnode);
  newnode->setIndex(curlen);
  newnode->setId(id);
  string longlab, shortlab;
  int pos = 0;
  longlab = id;
  int count = 0;
  while (count < 3 && pos < id.length()) {
    pos = id.find('/',pos+1);
    count++;
  }
  if (pos >= id.length()-1)
    pos = 0;
  longlab = id.substr(pos, id.length()-pos);
  pos = longlab.length();
  int newpos = longlab.rfind('/', pos);
  if (newpos = pos-1) // ends in slash, go back one more
    newpos = longlab.rfind('/', newpos);
  if (newpos <= 0)
    shortlab = longlab;
  else
    shortlab = longlab.substr(newpos, pos-newpos);
  newnode->setShortLabel(shortlab);
  newnode->setLongLabel(longlab);
  newnode->setOctLabels(shortlab);
}

//----------------------------------------------------------------------------
//           void HypGraph::newLink(HypLink *newlink, string id)           
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::newLink(HypLink *newlink, string id)
{
  int curlen = links.size();
  linktable[id] = curlen;
  //  linktable->insert(curlen, (char*)id.c_str());
  links.push_back(newlink);
  newlink->setIndex(curlen);
  newlink->setId(id);
}

  
////////////////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//                 HypGraph::HypGraph(HypData *h, istream & g)
//............................................................................
//  
//----------------------------------------------------------------------------
HypGraph::HypGraph(HypData *h, istream & g)
{
  if (getenv("LINKVIEW_DEBUG") != (char *)0) {
    debug = atoi(getenv("LINKVIEW_DEBUG"));
  }
  else {
    debug = 0;
  }
  
  hd = h;
  doMedia = 1;
  doExternal = 1;
  doOrphan = 1;
  newGraph(g);
  backstopcol[0] = .7;
  backstopcol[1] = .7;
  backstopcol[2] = .7;
}

//----------------------------------------------------------------------------
//                 HypGraph::HypGraph(HypData *h, vector <string> g)
//............................................................................
//  
//----------------------------------------------------------------------------
HypGraph::HypGraph(HypData *h, vector <string> g)
{
  if (getenv("LINKVIEW_DEBUG") != (char *)0) {
    debug = atoi(getenv("LINKVIEW_DEBUG"));
  }
  else {
    debug = 0;
  }
  
  hd = h;
  doMedia = 1;
  doExternal = 1;
  doOrphan = 1;
  newGraph(g);
  backstopcol[0] = .7;
  backstopcol[1] = .7;
  backstopcol[2] = .7;
}


//----------------------------------------------------------------------------
//                 HypGraph::HypGraph(HypData * h, string & rootId,
//									  int rootPriority, string & rootGroup)
//............................................................................
//  
//----------------------------------------------------------------------------
HypGraph::HypGraph(HypData * h, string & rootId,
                   int rootPriority, string & rootGroup)
{
  if (getenv("LINKVIEW_DEBUG") != (char *)0) {
    debug = atoi(getenv("LINKVIEW_DEBUG"));
  }
  else {
    debug = 0;
  }

  hd = h;
  doMedia = 1;
  doExternal = 1;
  doOrphan = 1;
  newGraph(rootId,rootPriority,rootGroup);
  backstopcol[0] = .7;
  backstopcol[1] = .7;
  backstopcol[2] = .7;
}

//----------------------------------------------------------------------------
//                          HypGraph::~HypGraph()                          
//............................................................................
//  
//----------------------------------------------------------------------------
HypGraph::~HypGraph()
{
  int i;
  for ( i = 0; i < nodes.size(); i++)
    delete nodes[i];
  nodes.erase(nodes.begin(),nodes.end());
  
  for ( i = 0; i < links.size(); i++)
    delete links[i];
  links.erase(links.begin(),links.end());
 
}

//----------------------------------------------------------------------------
//                       void HypGraph::clearMarks()                       
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::clearMarks()
{
  double md = hd->maxdist;
  for (int i = 0; i < nodes.size(); i++) {
    nodes[i]->setDrawn(0);
    nodes[i]->setDistance(md);
  }
}

//----------------------------------------------------------------------------
//      StringArray HypGraph::enumerateSubtree(string URL, int levels)     
//............................................................................
//  
//----------------------------------------------------------------------------
StringArray HypGraph::enumerateSubtree(string URL, int levels)
{
  StringArray strs;
  int i;
  if ((i = findUrl(URL)) >= 0) {
    if (nodes[i]->getEnabled()) {
      HypNode *n = nodes[i];
      int foo = levels;
      StringArray strs2 = n->enumerateSubtree(foo);
      strs.insert(strs.end(),strs2.begin(),strs2.end());
    }
  }
  return strs;
}

//----------------------------------------------------------------------------
//                 int HypGraph::getNodeEnabled(string URL)                
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::getNodeEnabled(string URL)
{
  int i;
  if ((i = findUrl(URL)) >= 0) return nodes[i]->getEnabled();
  return 0; 
}

//----------------------------------------------------------------------------
//               HypTransform HypGraph::getInitialPosition()               
//............................................................................
//  
//----------------------------------------------------------------------------
HypTransform HypGraph::getInitialPosition()
{
  return initPos;
}

//----------------------------------------------------------------------------
//         string HypGraph::getNodeGroup(string URL, int groupkey)         
//............................................................................
//  
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//            HypTransform HypGraph::getNodePosition(string URL)           
//............................................................................
//  
//----------------------------------------------------------------------------
HypTransform HypGraph::getNodePosition(string URL)
{
  int i;
  HypTransform T;
  if ((i = findUrl(URL)) >= 0) {
    HypNode *n = nodes[i];
    T = nodes[i]->getC();
  } else {
    T.identity();
  }
  return T;
}

//----------------------------------------------------------------------------
//                    void HypGraph::newGraph(istream & g)
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::newGraph(istream & flat)
{
vector <string> fileLines;
	string line;
	while (! flat.eof()) {
		getline(flat, line);
		if (line.length() <= 0)
           continue;
		fileLines.push_back(line); //sjk2011  decoupling to catch newick
    }
	newGraph(fileLines);
}
//----------------------------------------------------------------------------
//                    void HypGraph::newGraph(vector <string> g)
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::newGraph(vector <string> g)
{

	hd->loads++;//X1-DEBUG
	
   //X1-DEBUG fine to here
  // toss old data
  int i;
  if (nodetable.size() > 0){
	  nodetable.erase(nodetable.begin());
	  nodetable.erase(nodetable.begin(),nodetable.end());
  }
  
  if (linktable.size() > 0){
	  linktable.erase(linktable.begin());
	  linktable.erase(linktable.begin(),linktable.end());
  }
  
  for ( i = 0; i < nodes.size(); i++) {
    if (nodes[i]==NULL) continue;
	delete nodes[i];
  }
  for ( i = 0; i < links.size(); i++) {
	if (links[i]==NULL) continue;
    delete links[i];
  }
  nodes.erase(nodes.begin(),nodes.end());
  links.erase(links.begin(),links.end());
  initPos.identity();
  hd->centerindex = -1;
   //X1-DEBUG fine to here
  //X1-Debug  BAD line NEXT
  loadSpanGraph(g);
  
  treeroot = nodes[0];
  HypNode *treestart =  nodes[hd->centerindex];
  
  //////////////////////////////////////
  if (!treestart) treestart = treeroot;
  doLayout(treestart);
  if (debug) {
    string savestr = saveTree();
    FILE *f = fopen("/tmp/hypview", "w");
    if (f) {
      fprintf(f, "%s", (char*)savestr.c_str());
      fclose(f);
    }
  }
  return;
}


//----------------------------------------------------------------------------
//                    void HypGraph::newGraph(string & rootId, int rootPriority,
//				                              string & rootGroup)
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::newGraph(string & rootId, int rootPriority,
                        string & rootGroup)
{
  // toss old data
	std::vector <std::string> XA;
	XA.push_back("EMPTY");XA.push_back("EMPTY");XA.push_back("EMPTY");XA.push_back("EMPTY");
  int i;
  if (nodetable.size() > 0)
    nodetable.erase(nodetable.begin(),nodetable.end());
  
  if (linktable.size() > 0)
    linktable.erase(linktable.begin(),linktable.end());
  
  for ( i = 0; i < nodes.size(); i++) {
    delete nodes[i];
  }
  for ( i = 0; i < links.size(); i++) {
    delete links[i];
  }
  nodes.erase(nodes.begin(),nodes.end());
  links.erase(links.begin(),links.end());
  initPos.identity();
  hd->centerindex = -1;

  HypNode *newnode = doAddNode((HypNode *)0,rootId,0,rootPriority, XA);
  
  if (newnode) {
    newnode->setGroup(0,rootGroup);
    hd->centerindex = 0;
  }

  int linkparentindex, linkchildindex;
  for (i = 0; i < links.size(); i++) {
    //    fprintf(stderr, "link %s\n", (char*)links[i]->getId().c_str());
    HypLink *link = links[i];
    string par = link->getParentId();
    linkparentindex = findUrl(par);
    if (linkparentindex >= 0) 
      link->setParent(nodes[linkparentindex]);
    string child = link->getChildId();
    linkchildindex = findUrl(child);
    link->setChild(nodes[linkchildindex]);
  }     
  for (i = 0; i < nodes.size(); i++)
    markEnable(nodes[i]);

  treeroot = nodes[0];
  doLayout(treeroot);
  if (debug) {
    string savestr = saveTree();
    FILE *f = fopen("/tmp/hypview", "w");
    if (f) {
      fprintf(f, "%s", (char*)savestr.c_str());
      fclose(f);
    }
  }
  
  return;
}

//----------------------------------------------------------------------------
//                   void HypGraph::newLayout(HypNode *n)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::newLayout(HypNode *n)
{
  doLayout(n);
}

//----------------------------------------------------------------------------
//                   void HypGraph::doLayout(HypNode *n)                   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::doLayout(HypNode *n)
{
  int i;
  HypNode *p = n->getParent();
  n->layoutT();		// initialize top node
  n->layout(0);		// lay out nodes in hyperbolic space below n
  n->layoutC(0);	// save cumulative xforms below
  if (p) {
    n->layout(1);		// lay out nodes in hyperbolic space above n
    n->layoutC(1);	// save cumulative xforms above
  }
  for (i = 0; i < links.size(); i++)
    links[i]->layout();
}

//----------------------------------------------------------------------------
//                void HypGraph::loadSpanGraph(istream & flat)                
//............................................................................
//  
//----------------------------------------------------------------------------

void HypGraph::loadSpanGraph(istream & flat){
	vector <string> fileLines;
	string line;
	while (! flat.eof()) {
		getline(flat, line);
		if (line.length() <= 0)
           continue;
		fileLines.push_back(line); //sjk2011  decoupling to catch newick
    }
	loadSpanGraph(fileLines);

}
//----------------------------------------------------------------------------
//                void HypGraph::loadSpanGraph(vector <char> input)                
//............................................................................
//  
//----------------------------------------------------------------------------

void HypGraph::loadSpanGraph(vector <char> input){ //allows for subgraph load;
    string temp;
	vector<string> fileLines;
	for(size_t i=0;i<input.size();i++){
		temp.push_back(input[i]);
	}
	fileLines.push_back(temp);
	loadSpanGraph(fileLines);
}
//----------------------------------------------------------------------------
//                void HypGraph::loadSpanGraph(vecotr <string> fileLines)                
//............................................................................
//  
//----------------------------------------------------------------------------

void HypGraph::loadSpanGraph(vector <string> fileLines) //allows for subgraph load;
{
	std::vector <std::string> XA;//sjk2012
  //if (hd->loads>1){exit(1);}//SJK X1-DEBUG
  unsigned       linepos = 0;
  HypNode       *current = NULL;
  HypNode       *descend = NULL;
  string         id, type, mainid, line;
  string         token;
  StringArray    groups;
  int            level; 
  static string  whitespace = " \t\n";
  int            currlev = -999;
  int            linecount = 0;
  int			 charCount = 0;
  int            groupkey = 0;
  int            i,j;
  int            pri = 0;
  int			 color=0;
  HypNodeArray   parents;
  bool colorBranch=true;
  //vector <string> fileLines;
  vector <char> fileChar;
  bool isNewick = false;
  /////////////////////////////////////
  /////////////////////////////////////
  // special tests for arbitrary coloring rules
  // piggybacking on the newick input
  if (fileLines.size()>0){
	  if (fileLines[0].find("COLORBRANCH")>=0){
		  colorBranch=true;
	  }
	  if (fileLines[0].find("COLORBRANCH")>9){ // intMAX means fail.
		  colorBranch=false;
	  }
	  if (colorBranch){
//		  cout << "DEBUG:: ColorBRANCH ACTIVE"<<endl;
		  fileLines[0]=fileLines[0].substr(fileLines[0].find("NCH")+3);  
		
		  }
		
	  }
  


  //if (hd->loads>1){exit(1);}//SJK X1-DEBUG
  /////////////////////////////////////
  /////////////////////////////////////
  for(size_t i=0;i<fileLines.size();i++){
	  line=fileLines[i];
	  charCount+= line.size();
  }
  int parenDepth=0;
  int parenCount=0;

  for(size_t i=0;i<fileLines.size();i++){
	  string temp=fileLines[i];
	  string::iterator temp_iter;
	  for(temp_iter=temp.begin();temp_iter!=temp.end();temp_iter++){// cleanup the newick stuff
		  if (*temp_iter=='\n') {continue;}
		  if (*temp_iter=='\r') {continue;}
		
		  if (*temp_iter=='\'') {continue;}
		  if (*temp_iter=='#') {*temp_iter='#';}
		  if (*temp_iter==' ') {*temp_iter='_';}
		  if (*temp_iter=='(') {parenCount++;parenDepth++;}
		  if (*temp_iter==')') {parenCount++;parenDepth--;}
		  fileChar.push_back(*temp_iter);
//		  cout << *temp_iter;//sjk2011/DEBUG
		}//fortemp
  }	  
//if (hd->loads>1){exit(1);}//SJK DEBUG
  if ((parenCount>=2) && parenDepth==0){
		 // cout << "Newick File detected, converting to LVHIST"<<endl;
			  isNewick=true;
	  }
	bool isNhx=isNhxCustom(fileChar);
	  //if (parenDepth !=0) {cout <<"Possible Error: ParenDepth detected at: "<<parenDepth<<endl;}
  

  

  //spanGraph=fileChar;
  
  
  if (isNewick){
	  spanGraph=fileChar;
	  if (!isNhx){
		  fileChar=expandNewick(fileChar);    
	      cout << "Converting newick to LVHIST:";
		  fileLines=nhxToLVHIST(fileChar);
	  }else{
		  nhx tempNhx(fileChar);
		  cout << "Converting nhx to LVHIST:";
		  fileLines=tempNhx.getLvhist();
		  
	  }
	  //for (size_t i=0;i<fileChar.size();i++){
	//	  cout << fileChar[i];
	//	}
	 // cout <<endl;
	
	  
  } // if isNewick

  if (!isNewick){ 
	  cout << "NEWICK CONVERT FAILED"<<endl;
	  fileLines.resize(0);
	  string fl_temp;
	  fl_temp.assign("0 ERROR 0 color0");
	  fileLines.push_back(fl_temp);
	  fl_temp.assign("1 INVALID_INPUT 0 color1");
	  fileLines.push_back(fl_temp);
	  char fl_error[512];
	  sprintf(fl_error,"2 ParenDepth=%d 0 color512",parenDepth);
	  fl_temp.assign(fl_error);
	  fileLines.push_back(fl_temp);
	  sprintf(fl_error,"2 ParenCount=%d 0 color512",parenCount);
	  fl_temp.assign(fl_error);
	  fileLines.push_back(fl_temp);
	  sprintf(fl_error,"2 CharCount=chch%d 0 color512",charCount);
	  fl_temp.assign(fl_error);
	  fileLines.push_back(fl_temp);
  }


//cout << "FLAG3!!!\n";
if (colorBranch) {
	fileLines=leafFilterLVHIST(fileLines);
//	cout << "DEBUG:: LEAF FLAG !!\n";
}

//if (hd->loads>1){exit(1);}//SJK X1-DEBUG


  for(size_t indx=0;indx<fileLines.size();indx++){
	  //cout << "line " <<indx<<endl;//sjk2011 DDEBUG
	  line=fileLines[indx];
	  linecount=indx;//sjk2011
      // skip leading whitespace
	  linepos = 0;
	  linepos = line.find_first_not_of(" \t\n",linepos);
    
    // get the level
    
	  token = line.substr(linepos,line.find(' ',linepos) - linepos);
	  linepos += token.length() + 1;
	  linepos = line.find_first_not_of(" \t\n",linepos);
	  level = atoi(token.c_str());

    // get the id
    
	  if (line.find(' ',linepos) != line.npos)
		  id = line.substr(linepos,line.find(' ',linepos) - linepos);
	  else
		  id = line.substr(linepos,line.length() - linepos);
	  linepos += id.length() + 1;
	  linepos = line.find_first_not_of(" \t\n",linepos);
    
    if (level < -98) {
      if (id == "position") {
	for ( i = 0; i < 4; i++) {
	  for ( j = 0; j < 4; j++) {
            token = line.substr(linepos,line.find(' ',linepos) - linepos);
            initPos.T[i][j] = atof(token.c_str());
            linepos += token.length() + 1;
	    linepos = line.find_first_not_of(" \t\n",linepos);
          }
        }
	continue; 
      } else {
	continue;  // hack: -99 for level means comment. 
      }
    }

    // get the priority
    if (hd->spanPolicy & HV_SPANNUM) {
      token = line.substr(linepos,line.find(' ',linepos) - linepos);
      pri = atoi(token.c_str());
      linepos += token.length() + 1;
      linepos = line.find_first_not_of(" \t\n",linepos);
    }
    
    // enable file value is thrown away since it's obsolete 
    token = line.substr(linepos,line.find(' ',linepos) - linepos);
    linepos += token.length() + 1;
    linepos = line.find_first_not_of(" \t\n",linepos);

    // get the group(s)
    groups.erase(groups.begin(),groups.end());
    groupkey = 0;
	XA.resize(0);
	hd->XA_enabled=true;
    while (linepos < line.length() -1) {
		std::string g = line.substr(linepos,line.find(' ',linepos) - linepos);
      linepos += g.length() + 1;
      linepos = line.find_first_not_of(" \t\n",linepos);
      groups.push_back(g);//sjk2012  killing groups
	  XA.push_back(g);
      groupkey++;  //sjk2012  killing groups
    }
    
    if (level > currlev+1) {
      if (currlev < -98) {
        currlev = level;
      }
      else {
        if (current) {
          parents.insert(parents.begin(),current);
          current = descend; // down
          currlev++;
        }
      }
    }
    else {
      if (level <= currlev) {
        while (currlev >= level) {
          if (parents.size() > 0) {
            current = parents[0];
            parents.erase(parents.begin());
          }
          currlev--;
        }
      }  // else stay at the same level
    }
//if (hd->loads>1){exit(1);}//SJK X1-DEBUG  GOOD TO HERE
	// X1-DEBUG NEXT LINE BAD
    HypNode *newnode = doAddNode(current, id, level, pri,XA);
  //if (hd->loads>1){exit(1);}//SJK X1-DEBUG  BAD BY HERE
    if (newnode) {
		for ( i = 0; i < groupkey && i < groups.size(); i++) {
			newnode->setGroup(i, groups[i]);
		}
		descend = newnode;
		if (!current) {
			current = newnode;
			hd->centerindex = 0;
		}
	}
    else {
		int exists = findUrl(id);
		HypNode *oldnode = nodes[exists];
		descend = oldnode;
	}
  }
//if (hd->loads>1){exit(1);}//SJK X1-DEBUG DEAD BY HERE!!
  int linkparentindex, linkchildindex;
  for (i = 0; i < links.size(); i++) {
    //    fprintf(stderr, "link %s\n", (char*)links[i]->getId().c_str());
    HypLink *link = links[i];
    string par = link->getParentId();
    linkparentindex = findUrl(par);
    if (linkparentindex >= 0) 
      link->setParent(nodes[linkparentindex]);
    string child = link->getChildId();
    linkchildindex = findUrl(child);
    link->setChild(nodes[linkchildindex]);
  }     
//if (hd->loads>1){exit(1);}//SJK X1-DEBUG
  int disabled = 0;
  for (i = 0; !disabled && i < HV_MAXGROUPS; i++) 
    if (disabledGroup[i].size() > 0) disabled = 1;
  for (i = 0; disabled && i < nodes.size(); i++)
    markEnable(nodes[i]);

  return;
}

//----------------------------------------------------------------------------
//                       string HypGraph::saveTree()                       
//............................................................................
//  
//----------------------------------------------------------------------------
string HypGraph::saveTree()// LVHIST  --deprecated
{
  string str;
  int nl = nodes.size();
  int ll = links.size();
  if (debug) fprintf(stderr, "loaded %d nodes, %d links\n", nl, ll);
  //  sort();
  str = treeroot->save(treeroot->getLevel()); 
  return str;
}

//----------------------------------------------------------------------------
//                   void HypGraph::setDrawNodes(int on)                   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::setDrawNodes(int on)
{
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->setEnabled(on);
}

//----------------------------------------------------------------------------
//                   void HypGraph::setDrawLinks(int on)                   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::setDrawLinks(int on)
{
  for (int i = 0; i < links.size(); i++) {
    links[i]->setEnabled(on);
    links[i]->setDesired(on);
  }
}

//----------------------------------------------------------------------------
//                  void HypGraph::setNegativeHide(int on)                 
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::setNegativeHide(int on)
{
  hd->bNegativeHide = on;
  for (int i = 0; i < links.size(); i++) {
    HypNode *par = links[i]->getParent();
    if (par->getLevel() < 0) links[i]->setEnabled(!on);
  }
}
  

//----------------------------------------------------------------------------
//      int HypGraph::setDrawBackFrom(string URL, int on, int descend)     
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::setDrawBackFrom(string URL, int on, int descend)
{
  int i;
  if ((i = findUrl(URL)) >= 0) {
    nodes[i]->setEnabledOutgoing(on, descend);
  } else if (URL == "HV_TOPNODE") {
    nodes[0]->setEnabledOutgoing(on, descend);
  } else {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------------------
//       int HypGraph::setDrawBackTo(string URL, int on, int descend)      
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::setDrawBackTo(string URL, int on, int descend)
{
  int i;
  if ((i = findUrl(URL)) >= 0) {
    nodes[i]->setEnabledIncoming(on, descend);
  } else if (URL == "HV_TOPNODE") {
    nodes[0]->setEnabledIncoming(on, descend);
  } else {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------------------
//    int HypGraph::setNodeGroup(string URL, int groupkey, string group)   
//............................................................................
//  
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//     int HypGraph::setEnableGroup(int groupkey, string group, int on)    
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::setDisableGroup(int groupkey, string group, int on)
{
  int i;
  int gotit = -1;

  for (i = 0; i < (disabledGroup[groupkey]).size(); i++) {
    if ( (disabledGroup[groupkey])[i] == group) {
      gotit = 1;
      break;
    }
  }
  if ( (gotit < 0 && !on) || (gotit >= 0 && on))
    return 0;
  if (on) {
    disabledGroup[groupkey].push_back(group);
  } else {
    swap(disabledGroup[groupkey][i],disabledGroup[groupkey][0]);
    disabledGroup[groupkey].erase(disabledGroup[groupkey].begin());
  }
  for (i = 0; i < nodes.size(); i++)
    markEnable(nodes[i]);
  return 1;
}

//----------------------------------------------------------------------------
//         void HypGraph::setColorGroup(int groupkey, string group,        
//                                      float r, float g, float b)         
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::setColorGroup(int groupkey, string group,
                             float r, float g, float b)
{
  int gotit = -1;
  for (int i = 0; i < (colorGroup[groupkey]).size(); i++) {
    HypGroup *hg = (colorGroup[groupkey])[i];
    string id = hg->getGroup();
    if (id == group) {
      gotit = 1;
      hg->setColor(r,g,b);
      break;
    }
  }
  if (gotit == -1) {
    HypGroup *hg = new HypGroup();
    hg->setColor(r,g,b);
    hg->setGroup(group);
    colorGroup[groupkey].push_back(hg);
  }
}
void HypGraph::setStaticColorGroup(int groupkey, int group,
                             float r, float g, float b)//sjk2011
{
    HypGroup *hg = (colorGroup[groupkey])[group];
	hg->setColor(r,g,b);
}

void HypGraph::addColorGroup(int groupkey, string group,
                             float r, float g, float b)
{
	//MUST BE UNIQUE TO ADD CHECKING DISABLED  //SJK2011
    //faster case of setcolorgroup
  if (true) {
    HypGroup *hg = new HypGroup();
    hg->setColor(r,g,b);
    hg->setGroup(group);
    colorGroup[groupkey].push_back(hg);
  }
}



//----------------------------------------------------------------------------
//        float *HypGraph::getColorGroup(int groupkey, string group)       
//............................................................................
//  
//----------------------------------------------------------------------------
float *HypGraph::getColorGroup(int groupkey, string group)
{
  HypGroup *hg;
  if (groupkey > (HV_MAXGROUPS - 1))
    return((float *)0);
  
  for (int i = 0; i < (colorGroup[groupkey]).size(); i++) {
    hg = (colorGroup[groupkey])[i];
    string id = hg->getGroup();
    if (id == group) {
      return(hg->getColor());
    }
  }
  return backstopcol;
}
float *HypGraph::getStaticColorGroup(int groupkey, string group)//sjk2011
{
  HypGroup *hg;
  int dest =atoi(group.substr(5,4).c_str());
  if (dest>4096){dest=0;}
  hg = (colorGroup[0])[dest];
    return(hg->getColor());// sjk test 2011-03-11
}


//----------------------------------------------------------------------------
//        float *HypGraph::getColorNode(HypNode *n)
//............................................................................
//  
//----------------------------------------------------------------------------

/*
float *HypGraph::getColorNode(HypNode *n) 
{
  string id = n->getGroup(hd->groupKey);
  
  //int dest =atoi(id.substr(5,4).c_str());// sjk2011  TEST CODE
  //if (dest>4096){dest=0;}
  //n->setLinkSize(dest);// end TEST CODE  // just setting the size of the links.

  return getStaticColorGroup(hd->groupKey, id);
  //sjk2011 static is a faster version of getColorGroup
}
*/
float *HypGraph::getColorNode(HypNode *n) 
{	if (!n){return hd->blueLight;}
	string id = n->getGroup(hd->groupKey);
	float * col=getStaticColorGroup(hd->groupKey, id);	

	hd->leafColorScheme=(HV_NODESCHEME_LEAF==hd->nodeColorScheme);
	if (hd->leafColorScheme){
		bool greenLeaf;
		bool blueNode;
		bool greenLight=false;
		bool blueLight=false;
		bool hasData=false;

		greenLeaf= (0==n->getChildCount());
		blueNode=!greenLeaf;
		greenLeaf=(greenLeaf&&hd->leafColorScheme);
		blueNode=(blueNode&&hd->leafColorScheme);
		int branchLength=0;
		if (n->get_XA().size()>3){
			if (&n->get_XA()!=NULL){
				branchLength= atoi(n->get_XA()[2].c_str());
			}
		}
		if (branchLength>0){greenLight=greenLeaf;blueLight=blueNode;};
		if (greenLeaf) {col=hd->greenLeaf;}
		if (greenLight&&hd->isChordateTree) {col=hd->greenLight;}
		if (blueNode) {col=hd->blueNode;}
		if (blueLight&&hd->isChordateTree) {col=hd->blueLight;}
	}
	
	if (HV_NODESCHEME_CLOWN==hd->nodeColorScheme){
		if (0==n->getIndex()){return (hd->blueNode);}
		//if (NULL==n->getParent()){return (hd->blueNode);}
		//int kids=(link->getParent()->getChildCount())%16;
		int kids=(n->getParent()->getChildCount())%16;
		col=hd->clownColor[kids];
		
	}

	return col;
  //sjk2011 static is a faster version of getColorGroup
}


void HypGraph::setLinkSize(HypNode *n) //sjk2011 adds a link size field to the system.
{
  string id = n->getGroup(hd->groupKey);  
  int dest =atoi(id.substr(5,4).c_str());
  if (dest>4096){dest=0;}
  n->setLinkSize(dest);
  
}



//----------------------------------------------------------------------------
//     int HypGraph::resetColorLink(HypNode *fromnode, HypNode *tonode)    
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::resetColorLink(HypNode *fromnode, HypNode *tonode)
{
  string linkid = fromnode->getId() + "|" + tonode->getId(); 
  HypLink *link = getLinkFromId(linkid);
  string linkid2 = tonode->getId() + "|" + fromnode->getId(); 
  HypLink *link2 = getLinkFromId(linkid);

	return (resetColorLink(link)+resetColorLink(link2));

}



int HypGraph::resetColorLink(HypLink *link)
{
  if (link && hd) {
    link->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
		 hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
    link->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
		 hd->colorLinkTo[2], hd->colorLinkTo[3]);
    if (1==link->getChild()->getDndsLevel()){
		link->setColorFrom(hd->dndsColorLinkFrom[0], hd->dndsColorLinkFrom[1], 
			 hd->dndsColorLinkFrom[2], hd->dndsColorLinkFrom[3]);
		link->setColorTo(hd->dndsColorLinkTo[0], hd->dndsColorLinkTo[1], 
			hd->dndsColorLinkTo[2], hd->dndsColorLinkTo[3]);
    }
	if (hd->clownColorScheme){
		int kids=(link->getParent()->getChildCount())%16;
		link->setColorFrom(hd->clownColor[kids][0],hd->clownColor[kids][1],
		    hd->clownColor[kids][2],hd->clownColor[kids][3]);
		link->setColorTo(hd->clownColor[kids][0],hd->clownColor[kids][1],
			hd->clownColor[kids][2],hd->clownColor[kids][3]);
    }
    

    return 1;
  } else return 0;

}


//----------------------------------------------------------------------------
//      int HypGraph::setColorLink(HypNode *fromnode, HypNode *tonode,     
//                                 float r, float g, float b)              
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::setColorLink(HypNode *fromnode, HypNode *tonode, 
                           float r, float g, float b) 
{
  string linkid = fromnode->getId() + "|" + tonode->getId(); 
  HypLink *link = getLinkFromId(linkid);
  if (link) {
    link->setColorFrom(r,g,b,1.0);
    link->setColorTo(r,g,b,1.0);
    return 1;
  } else {
    return 0;
  }
}

//----------------------------------------------------------------------------
//        int HypGraph::addLink(HypNode *fromnode, HypNode *tonode)        
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::addLink(HypNode *fromnode, HypNode *tonode)
{
  HypLink *link;
  string linkid = fromnode->getId() + "|" + tonode->getId(); 
  if ((link = getLinkFromId(linkid)) == NULL) {
    link = new HypLink(fromnode->getId(), tonode->getId());
    newLink(link, linkid);
    //link->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
//		       hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
//    link->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
//		     hd->colorLinkTo[2], hd->colorLinkTo[3]);
    resetColorLink(link);
    link->setParent(fromnode);
    link->setChild(tonode);
    link->setDesired(1);
    link->setEnabled(1);
    fromnode->addOutgoing(link);
    tonode->addIncoming(link);
    link->layout();
  }
  link->setEnabled(1);
  link->setDesired(1);
  return 1;
}

//----------------------------------------------------------------------------
//  int HypGraph::setDrawLink(HypNode *fromnode, HypNode *tonode, int on)  
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::setDrawLink(HypNode *fromnode, HypNode *tonode, int on)
{
  string linkid = fromnode->getId() + "|" + tonode->getId();
  HypLink *link = getLinkFromId(linkid);
  if (link) {
    link->setEnabled(on);
    link->setDesired(on);
    return 1;
  } else {
    return 0;
  }
}

//----------------------------------------------------------------------------
//      int HypGraph::getDrawLink(HypNode *fromnode, HypNode *tonode)      
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::getDrawLink(HypNode *fromnode, HypNode *tonode)
{
  string linkid = fromnode->getId() + "|" + tonode->getId();
  HypLink *link = getLinkFromId(linkid);
  if (link) {
    return link->getEnabled();
  } else {
    return -1;
  }
}

//----------------------------------------------------------------------------
//              int HypGraph::setDrawNode(HypNode *n, int on)              
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::setDrawNode(HypNode *n, int on)
{
  if (n) {
    n->setEnabled(on);
    return 1;
  } else {
    return 0;
  }
}

//----------------------------------------------------------------------------
//          int HypGraph::addNode(string parentid, string childid)         
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::addNode(string parentid, string childid, std::vector <std::string> XA)
{
	
  int i;
  if ((i = findUrl(parentid)) >= 0) {
    HypNode *parent = nodes[i];
    int level = parent->getLevel();
    doAddNode(parent, childid, level+1, 0,XA);
    return 1;
  } else {
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////////////

/* ensure canonical lexicographic ordering */
//----------------------------------------------------------------------------
//                          void HypGraph::sort()                          
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::sort()
{
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->sort();
}

//----------------------------------------------------------------------------
//         int HypGraph::maybeAddChild(HypNode *current, string id)        
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::maybeAddChild(HypNode *current, string id)
{
  if (id == current->getId()) {
    // fprintf(stderr, "identical parent/child %s\n", (char*)id);
    return 0;
  }
  HypNodeArray *kids = current->getChildren();
  // don't add same child twice 
  // can't use find, it uses compareElements
  //      if ( kids->find(oldnode) != -1 || id == current->getId()) {
  int gotit = -1;
  int i;
  for (i = 0; i < kids->size(); i++) {
    //HypNode *thekid = (*kids)[i];
    HypNode *thekid = kids->at(i);
    if (current->getId() == thekid->getId()) {
      gotit = i;
      break;
    }
  }
  if (gotit >= 0) {
    // fprintf(stderr, "already have child %s in %s\n",
    //         (char*)id, (char*)current->getId());
    return 0;
  }
  // else safe to add as child
  return 1;
}

//----------------------------------------------------------------------------
//       int HypGraph::maybeAddOutgoing(HypNode *current, string id)       
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::maybeAddOutgoing(HypNode *current, string id)
{
  // only add if it's not already there
  int nogo = 0;
  int i;
  HypLink *l;
  for (i = 0; i < current->getOutgoingCount(); i++) {
    l = current->getOutgoing(i);
    if (l->getChildId() == id) {
      nogo = 1;
      break;
    }
  }
  if (!nogo) {
    for (i = 0; i < current->getChildCount(); i++) {
      l = current->getChildLink(i);
      if (l->getChildId() == id) {
	nogo = 1;
	break;
      }
    }
  }
  if (nogo) {
    // fprintf(stderr,
    //         "%d nontree: already has outgoing or child %s in %s\n",
    //         linecount,(char*)id, (char*)current->getId());
    return 0;
  } else {
    return 1;
  }
}

//----------------------------------------------------------------------------
//        HypNode *HypGraph::doAddNode(HypNode *current, string id,        
//                                     int level, int pri)                 
//............................................................................
//  
//----------------------------------------------------------------------------
HypNode *HypGraph::doAddNode(HypNode *current, string id,
							 int level, int pri, std::vector<std::string> XA)
{

	//if (hd->loads>1){exit(1);}//SJK DEBUG
  string linkid;
  int swap = 0;
  //X1-Debug  NEXT LINE BAD
  int exists = findUrl(id);
  //if (hd->loads>1){exit(1);}//SJK DEBUG
  if (current && exists >= 0) {
    // possibly choose new main parent
    HypNode *oldnode = nodes[exists];
    if (!maybeAddChild(current, id))
      return (HypNode *)0;
    HypNode *oldpar = oldnode->getParent();
    swap = maybeSwap(current, oldpar, id, pri);
    if (1 == swap) {
      /* switch: found better main parent */

      linkid = current->getId() + "|" + id; // parent|child
      HypLink *oldlink = oldnode->getParentLink();
      //      fprintf(stderr, "swap in %s, swap out %s\n", (char*)linkid.c_str(), (char*)oldlink->getId().c_str());
      oldpar->addOutgoing(oldlink);
      oldpar->deleteChildLink(oldlink);
      oldpar->deleteChild(oldnode);
      HypLink *newlink = new HypLink(current->getId(), id);
      newLink(newlink, linkid);
      newlink->setParent(current);
      newlink->setChild(oldnode);
      newlink->setEnabled(1);
      newlink->setDesired(1);
      oldlink->setEnabled(0);
      oldlink->setDesired(0);
      //newlink->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
	//		    hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
      //newlink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
	//		  hd->colorLinkTo[2], hd->colorLinkTo[3]);
	resetColorLink(newlink);
      current->addChildLink(newlink);
      current->addChild(oldnode);
      oldnode->addParentLink(newlink);
      oldnode->addParent(current);
      oldnode->setLevel(current->getLevel()+1);
      oldnode->addIncoming(oldlink);
    } else {
      // outgoing nontree link
      if (!maybeAddOutgoing(current, id))
        return (HypNode *)0;
      /* mainparent and child nodes might not exist yet, add later */
      linkid = current->getId() + "|" + id; // parent|child
      HypLink *newlink = new HypLink(current->getId(), id);
      newLink(newlink, linkid);
      newlink->setParent(current);
      newlink->setChild(oldnode);
      newlink->setEnabled(0);
      //newlink->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
	//		    hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
      //newlink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
	//		  hd->colorLinkTo[2], hd->colorLinkTo[3]);
	resetColorLink(newlink);
      //	newlink->setEnabled(1);
      current->addOutgoing(newlink);
      oldnode->addIncoming(newlink);
    }

    return (HypNode *)0;

  } else {
    // child node
    //      int thelev = current ? current->getLevel()+1 : level;
    //      HypNode *newnode = new HypNode(hd, current, thelev);
    HypNode *newnode = new HypNode(hd, current, level);
    newNode(newnode, id,XA);
    newnode->setEnabled(1);
	
    if (current) {
	  
      linkid = current->getId() + "|" + id; // parent|child
      HypLink *newlink = new HypLink(current->getId(), id);
      newLink(newlink, linkid);
      newlink->setParent(current);
      newlink->setChild(newnode);
      newlink->setEnabled(1);
      newlink->setDesired(1);
      //newlink->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
	//		    hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
      //newlink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
	//		  hd->colorLinkTo[2], hd->colorLinkTo[3]);
	resetColorLink(newlink);
      current->addChildLink(newlink);
      current->addChild(newnode);
      newnode->addParentLink(newlink);
      newnode->addParent(current);
    }

    return newnode;

  }
}

//----------------------------------------------------------------------------
//                  void HypGraph::markEnable(HypNode *n)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypGraph::markEnable(HypNode *n)
{
  StringArray::iterator gotit;
  string thegroup;
  int i;

  for (i = 0; i < n->getNumGroups(); i++) {
    thegroup = n->getGroup(i);
    gotit = find(disabledGroup[i].begin(),disabledGroup[i].end(),thegroup);
    if (gotit != disabledGroup[i].end()) {
      n->markEnable(0,0);
      return;
    }
  }
  n->markEnable(1, 0);
}

//----------------------------------------------------------------------------
//        int HypGraph::maybeSwap(HypNode *current, HypNode *oldpar,       
//                                string id, int pri)                      
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::maybeSwap(HypNode *current, HypNode *oldpar,
                        string id, int pri)
{
  /* possible policies, in order of precedence
     keep - change nothing
     forest - keep swaps within subtrees starting at level 0
     hierarchy - best match on identifier
     bfs - breadth-first search
     lexicographic - alphanumeric ordering
     */
  if (hd->spanPolicy & HV_SPANKEEP) return 0;
  if (!oldpar || !current || id.length() < 1) return 0;
  //  if ((hd->spanPolicy & HV_SPANFOREST) && (oldpar->getLevel() <= 0))
  //    return 0;

  if (hd->spanPolicy & HV_SPANFOREST) {
    HypNode *newcur, *oldcur;
    int newlev, oldlev;
    newcur = current;
    newlev = newcur->getLevel();
    while (newcur && newlev > 0) {
      newcur = newcur->getParent();      
      newlev = newcur->getLevel();
    }
    oldcur = oldpar;
    oldlev = oldcur->getLevel();
    while (oldcur && oldlev > 0) {
      oldcur = oldcur->getParent();      
      oldlev = oldcur->getLevel();
    }
    if (oldcur != newcur) return 0;
  }

  string oldpiece, newpiece, kidpiece;
  int swap = -1;
  if (hd->spanPolicy & HV_SPANNUM) {
    int oldpri;
    HypLink *curparlink = current->getParentLink();
    if (curparlink) {
      oldpri = curparlink->getPriority();
      if (pri > oldpri) {
	swap = 1;
      } else if (pri == oldpri) {
	swap = -1;
      } else swap = -2;
    }
  }
  if (hd->spanPolicy & HV_SPANHIER) {
    unsigned oldpos, newpos, kidpos;
    oldpos = newpos = kidpos = 0;
    unsigned oldplace, newplace, kidplace;
    oldplace = newplace = kidplace = 0;
    string oldid = oldpar->getId();
    string newid = current->getId();
    swap = 0;
    while (!swap) {
      oldplace = oldid.find('/', oldpos);
      newplace = newid.find('/', newpos);
      kidplace = id.find('/', kidpos);
      oldpiece = oldid.substr(oldpos, oldplace-oldpos);
      newpiece = newid.substr(newpos, newplace-newpos);
      kidpiece = id.substr(kidpos, kidplace-kidpos);
      oldpos = oldplace+1;
      newpos = newplace+1;
      kidpos = kidplace+1;
      if (oldpiece == kidpiece) {
	swap = (newpiece == kidpiece) ? 0 : -2;
	if (oldpos == string::npos && kidpos == string::npos) swap = -1;
      } else 
	swap = (newpiece == kidpiece) ? 1 : -1;
    }
  } else {
    oldpiece = oldpar->getId();
    newpiece = current->getId();
  }
  if (hd->spanPolicy & HV_SPANBFS && swap == -1) {
    if (current->getLevel() < oldpar->getLevel()) {
      swap = 1; // new wins
    } else if (current->getLevel() == oldpar->getLevel()) {
      swap = -1; // no change
    } else {
      swap = -2; // old wins
    }
  }
  if (hd->spanPolicy & HV_SPANLEX && swap == -1) {
    if (newpiece < oldpiece) swap = 1;
  }
  HypNode *par;
  if (1 == swap) {
    // make sure we're not introducing a cycle!
    par = current->getParent();
    while (par && swap == 1) {
      if (par->getId() == id) swap = -1;
      par = par->getParent();
    }
  }
  return swap;
}

//----------------------------------------------------------------------------
//              int HypGraph::getChildCount(const string & id)             
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::getChildCount(const string & id)
{
  int count = 0;
  HypNode *n = getNodeFromId(id);
  if (n && n->getEnabled()) {
    for (int i = 0; i < n->getChildCount(); i++) {
      HypLink *l = n->getChildLink(i);
      if (l->getEnabled()) count++;
    }
    return count;
  }
  return 0;
}

//----------------------------------------------------------------------------
//            int HypGraph::getIncomingCount(const string & id)            
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::getIncomingCount(const string & id)
{
  int count = 0;
  HypNode *n = getNodeFromId(id);
  if (n && n->getEnabled()) {
    for (int i = 0; i < n->getIncomingCount(); i++) {
      HypLink *l = n->getIncoming(i);
      HypNode *p = l->getParent();
      HypNode *c = l->getChild();
      if (p->getEnabled() && c->getEnabled())
        count++;
    }
    //    if (id != treeroot->getId())
    //      count++;
    return count;
  }
  return 0;
}

//----------------------------------------------------------------------------
//            int HypGraph::getOutgoingCount(const string & id)            
//............................................................................
//  
//----------------------------------------------------------------------------
int HypGraph::getOutgoingCount(const string & id)
{
  int count = 0;
  HypNode *n = getNodeFromId(id);
  if (n && n->getEnabled()) {
    for (int i = 0; i < n->getOutgoingCount(); i++) {
      HypLink *l = n->getOutgoing(i);
      HypNode *p = l->getParent();
      HypNode *c = l->getChild();
      if (p->getEnabled() && c->getEnabled())
        count++;
    }
    return count;
  }
  return 0;
}
vector <char> HypGraph::expandNhx(vector <char> &input){
	if (isNhxCustom(input)){exit(2);}
	return (expandNewick(input));

}
vector <char> HypGraph::expandNewick(vector <char> &input0){//input0 not a typo
	// non-recursive expander
	vector <char> result;
	vector <char> input;
	vector <char> sigil;
	vector <size_t> sigilLocation;
	vector <bool> sigilTag;
//	enum status {EMPTY,FOUND,UNNEEDED };
	bool BROKEN=false;
	//////////////////////////////////////////
	//////  NHX [] cleanout
	//////////////////////////////////////////
    int braceDepth=0;
	for(size_t i=0;i<input0.size();i++){
		if ('['==input0[i]){
			braceDepth++;
			continue;
		}
		if (']'==input0[i]){
			braceDepth--;
			continue;  // dont process the ]
		}
		if (braceDepth!=0) {continue;} // ignoring the contents of []
		
		input.push_back(input0[i]);
		//cout << input0[i];
		
	}
    ////done cleaning the NHX stuff out
	///////////////////////////////////////
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
vector <char> HypGraph::expandNewick(vector <char> input, int & firstNode){
	vector <char> result;
	vector <vector <char> > elements;
	vector <char> temp;
	char buffer[512];
	string nodeName;
	cout << "DEBUG:: EXPANDING::";
	for(size_t i=0;i<input.size();i++){
		cout << input[i];
	}
	cout << endl;
	// SOLVE BASE CASE HERE
	 
	if (input.size()==0 || ((input.size()==1) && (input[0]==';')) ) {
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
vector <float> HypGraph::newickGetBounds(vector <char> &input){
	//#define FLT_MAX <float rvalue >= 10^37 >
	vector <float> result;
	result.resize(2);
	result[0] =0.0; //max
	result[1] =10^37; //min
	//STEP 1 Know the targets of each Parenthesis
	vector <size_t> targets;  //matching Paren Values;
	targets.resize(input.size(),input.size());
	vector <int> depthList;
	char buffer[512];
	int depth=0;
	int color=0;
	float colorVal;
	string name;
	string colString;
    

	for(size_t i=0;i<input.size();i++){
		if ('('==input[i]){
			depthList.push_back(i);
		}
		if (')'==input[i]){
			size_t lParenIndex=depthList.back();
			depthList.pop_back();
			targets[lParenIndex]=i;
			targets[i]=lParenIndex;
		}
	}

	//  :KLUDGE:
	//  using newick parser to find max and min.
	//  could be done in fewer lines, but the newick parser 
	//  code is tested.
	size_t nameIndex;
	size_t lastNonSigilIndex;
	size_t printDepth;
	
	char lastSigil;
	for(size_t i=0;i<input.size();i++){
		bool endSigil=false;
		bool colonFound=false;
		nameIndex=input.size();
        
		if ('('==input[i]){
			nameIndex=targets[i]+1;
			colString="";
			lastSigil='(';
			printDepth=depth;//print at right depth before incrementing it
			depth++;
			lastNonSigilIndex=input.size();// clear the print pointer/
		}
		else if (')'==input[i]){
			nameIndex=lastNonSigilIndex;  // tricky skips
			// points to either printable area, or a clear zone
			printDepth=depth;// we want to print the inside at right depth
			depth--;
			lastNonSigilIndex=input.size();//clear the print pointer
			lastSigil=')';

		}
		else if (',' == input[i]){
			if (')'==lastSigil){
				lastNonSigilIndex=input.size();
			}
			printDepth=depth;
			nameIndex=lastNonSigilIndex; // lastsigils are either ( or ,
			lastNonSigilIndex=input.size(); 
			lastSigil=',';
		}
		else if (';' == input[i]){
			lastSigil=';';
			lastNonSigilIndex=input.size();
		}
		else if (input.size()==lastNonSigilIndex){
			// so this isnt a sigil, and it's the start of a label
			lastNonSigilIndex=i;
			nameIndex=input.size();// dont print
			
		}

		for(size_t j=nameIndex;(j<input.size()&& !endSigil);j++){
			//if (endSigil) {continue};
			if (';'==input[j]){endSigil=true;}
			if (','==input[j]){endSigil=true;}
			if (')'==input[j]){endSigil=true;}
			if (endSigil) {
				colorVal=atof(colString.c_str());
				if (colorVal> result[0]){ result[0]=colorVal;}
				if (colorVal< result[1]){ result[1]=colorVal;}
			}
				
			if (':'==input[j]){colonFound=true;}
			if ((colonFound)&&(!endSigil)){
				if (':'!=input[j]){colString.push_back(input[j]);}
			}
			else if (!endSigil) {
				//name.push_back(input[j]);
			}
		}
		colorVal=atof(colString.c_str());
		if (colorVal > result[0]) result[0]=colorVal;
		if (colorVal < result[1]) result[1]=colorVal;
		name="";
		colString="";
		colonFound=false;
			
		
		
	} // forI (step through input)
	return result;

}
///////////////////////////////////////////////////////












bool HypGraph::isNhxCustom(vector <char> &input){
	////////////////////////////////////////////////////////////////////////
	bool badNhx=false;	
	int matchNum=0;
	std::string test="[&&NHX:DNDS=";
	for (size_t i=0;i<input.size();i++){
		if ((input.size()- i) < test.size()){badNhx=true;}
		if (input[i]==','){badNhx=true;}
		if (input[i]==')'){badNhx=true;}
		if (badNhx){return (false);}
		if (input[i]==test.c_str()[matchNum]){matchNum++;}
		else {matchNum=0;}
		if (matchNum>10){return (true);}
	}
	return (false);//ERROR ZONE
}
vector <string> HypGraph::nhxToLVHIST(vector <char> &input){
	vector <string> result;
	string temp;
	////////////////////////////////////////////////////////////////////////
	if (isNhxCustom(input)){
		temp.assign("0 ERROR 0 color0 0 0");result.push_back(temp);
		
		return result;
		
	}
	return newickToLVHIST(input);
	//////////////////////////////////////////////////////////////////////////
}


vector <string> HypGraph::newickToLVHIST(vector <char> &input){
	vector <string> result;
	//vector <char> input;
	//input.resize(0);
		//vector <char> input1;
    ///////////////////////  Step0 remove nhx [] notation
/*		cout <<"DEBUG NHX clearing"<<endl;
	int braceDepth=0;
	for(size_t i=0;i<input0.size();i++){
		if ('['==input0[i]){
			braceDepth++;
			
		}
		if (']'==input0[i]){
			braceDepth--;
			continue;  // dont process the ]
		}
		if (braceDepth!=0) {continue;} // ignoring the contents of []
		
		input.push_back(input0[i]);
		cout << input0[i];
		
	}
	*/
	cout <<"DEBUG NHX clearing done"<<endl;
	///////////////////////////////////////////////
	//STEP 1 Know the targets of each Parenthesis
	vector <size_t> targets;  //matching Paren Values;
	targets.resize(input.size(),input.size());
	vector <int> depthList;
	char buffer[512];
	int depth=0;
	int color=0;
	float colorVal;
	string name;
	string colString;
    

	for(size_t i=0;i<input.size();i++){
		if ('('==input[i]){
			depthList.push_back(i);
		}
		if (')'==input[i]){
			size_t lParenIndex=depthList.back();
			depthList.pop_back();
			targets[lParenIndex]=i;
			targets[i]=lParenIndex;
		}
	}
	// STEP2 Walk Through and Print
	size_t nameIndex;
	size_t lastNonSigilIndex;
	size_t printDepth;
	
	char lastSigil;
	for(size_t i=0;i<input.size();i++){
		bool endSigil=false;
		bool colonFound=false;
		nameIndex=input.size();
        
		if ('('==input[i]){
			nameIndex=targets[i]+1;
			colString="";
			lastSigil='(';
			printDepth=depth;//print at right depth before incrementing it
			depth++;
			lastNonSigilIndex=input.size();// clear the print pointer/
		}
		else if (')'==input[i]){
			nameIndex=lastNonSigilIndex;  // tricky skips
			// points to either printable area, or a clear zone
			printDepth=depth;// we want to print the inside at right depth
			depth--;
			lastNonSigilIndex=input.size();//clear the print pointer
			lastSigil=')';

		}
		else if (',' == input[i]){
			if (')'==lastSigil){
				lastNonSigilIndex=input.size();
			}
			printDepth=depth;
			nameIndex=lastNonSigilIndex; // lastsigils are either ( or ,
			lastNonSigilIndex=input.size(); 
			lastSigil=',';
		}
		else if (';' == input[i]){
			lastSigil=';';
			lastNonSigilIndex=input.size();
		}
		else if (input.size()==lastNonSigilIndex){
			// so this isnt a sigil, and it's the start of a label
			lastNonSigilIndex=i;
			nameIndex=input.size();// dont print
			
		}

		for(size_t j=nameIndex;(j<input.size()&& !endSigil);j++){
			//if (endSigil) {continue};
			if (';'==input[j]){endSigil=true;}
			if (','==input[j]){endSigil=true;}
			if (')'==input[j]){endSigil=true;}
			if (endSigil) {
				colorVal=atof(colString.c_str());
				if (colorVal>1.0) {colorVal=1.0;}
				if (colorVal<0.0) {colorVal=0.0;}
				color=17;
				color=(int)(4095.0* colorVal+.49999);
				if (color>4095){color=4095;}
//				sprintf(buffer,"%d %s 1 color%d\n",printDepth,name.c_str(),color);
//				name="";
//				colString="";
//				colonFound=false;
			}
				
			if (':'==input[j]){colonFound=true;}
			if ((colonFound)&&(!endSigil)){
//				printf("pushing %c\n",input[j]);
				
				if (':'!=input[j]){colString.push_back(input[j]);}
			}
			else if (!endSigil) {
				name.push_back(input[j]);
			}
		}
		colorVal=atof(colString.c_str());
		if (colorVal>1.0) {colorVal=1.0;}
		if (colorVal<0.0) {colorVal=0.0;}
		color=(int)((4095.0* colorVal)+.49999);
		if (color>4095){color=4095;}
		//if (nameIndex<input.size()){ sprintf(buffer,"%d %s 1 color%d\n",depth,name.c_str(),color);}
		if (nameIndex<input.size()){ 
			sprintf(buffer,"%d %s 1 color%d\n",printDepth,name.c_str(),color);
		    name.assign(buffer);
			result.push_back(name);
		}
		name="";
		colString="";
		colonFound=false;
			
		
		
	} // forI (step through input)
	return result;

}
vector <string> HypGraph::leafFilterLVHIST(vector<string> input){
	// takes in an LVHIST checks the front, and changes the colors.
	//  THIS ASSUMES STRIPPED LVHIST COMMENTS
	//  LVHIST COMMENTS WILL CAUSE ODD COLORING.
	vector <string> result;
	size_t size=input.size();
	vector <int> depthValues;
	for(size_t i=0;i<size;i++){
		depthValues.push_back(atoi(input[i].c_str()));
	}
	depthValues.push_back(-999); // forces the last entry to be a leaf. 
	//prevents stack overwalk.
	for(size_t i=0;i<size;i++){
		bool leaf=true;
		string colString;
		
		if (leaf){
			colString.assign("color240");//GREEN
		}
		if (depthValues[i]<depthValues[i+1]){
			leaf=false;
			colString.assign("color15");// BLUE
		}
		string temp=input[i];
		int pos=temp.rfind("color")-1;
		char buffer[512];
		sprintf(buffer,"%s %s\n",temp.substr(0,pos).c_str(),colString.c_str());
		temp.assign(buffer);
		result.push_back(temp);
	}
	//return input;
	return result;

}


/*vector <string> HypGraph::newickToLVHIST(vector<char> input,int currentDepth){
	cout << "DEBUG::newick-flag00!!!\n";
	// Base Case
	// NO PARENS LEFT IN INPUT
//	cout << "DEBUG 1:"<<endl;
	vector <string> result;
	bool parenFound=false;
	bool colonFound=false;
	bool commaFound=false;
	size_t left=input.size();
	size_t right=input.size();
	size_t colon=input.size();
	size_t comma=input.size();
	size_t depth=0;
	for(size_t i=0;i<input.size();i++){
		char current=input[i];
		if('('==current) {
			if (!parenFound){ left=i;}
			depth++;
			parenFound=true;
		}
		if (')'==current){
			parenFound=true;
			right=i;
			depth--;
		}
		if (':'==current) {
			colonFound=true;
			colon=i;
		}
		if (','==current) {
			if (0==depth){
				commaFound=true;
				comma=i;
			}
			
		}

		
	}
//	cout << "DEBUG 2:"<<endl;
	if ((!parenFound) && (!commaFound)){
//	cout << "DEBUG 3:"<<endl;
		if (!colonFound){
//		cout << "DEBUG 3a:"<<endl;
			char buffer[512];
			string name;
			for(size_t i=0;i<input.size();i++){
				if (';'==input[i]) continue;
				name.push_back(input[i]);
			}
			sprintf(buffer,"%d %s 1 color0\n",currentDepth,name.c_str());
			string resultBuffer;
			resultBuffer.assign(buffer);
			if (resultBuffer.size()==12){
			cout <<"FAIL:::::";
			for(size_t i=0;i<input.size();i++){
				cout<<input[i];
			}
			}
			result.push_back(resultBuffer);
			cout << "DEBUG::newick-flag4!!!\n";
			return result;
		}
		else{	
//			cout << "DEBUG 3b:"<<endl;
			vector <char> temp;
			for(size_t i=0;i<colon;i++){
				temp.push_back( input[i]);
			}
//			cout << "DEBUG 3b1:"<<endl;
			string number;
			for(size_t i=colon;i<input.size();i++){
				if (i==colon){continue;}
				number.push_back(input[i]);
			}
//			cout << "DEBUG 3b2:"<<endl;
			float colorVal=atof(number.c_str());
//			cout << "DEBUG 3b3:"<<endl;
			if (colorVal>1.0) {colorVal=1.0;}
			if (colorVal<0.0) {colorVal=0.0;}
			int color=(int)(4095.0*colorVal+.49999);
			char buffer[512];
//			cout << "DEBUG 3b3_1:"<<endl;
			string name;
			for(size_t i=0;i<colon;i++){
				name.push_back(input[i]);
			}
			sprintf(buffer,"%d %s 1 color%d\n",currentDepth,name.c_str(),color);
			string resultBuffer;
//			cout << "DEBUG 3b4:"<<endl;
			resultBuffer.assign(buffer);
			result.push_back(resultBuffer);
			cout << "DEBUG::newick-flag1!!!\n";
			return result;
		}
	}// parenFound
	///////////////////////////////////////////////////
	// Handle the (XXXXX)lable case
	/////////////////////////////////////////////////
	if (!commaFound){                  // only refers to comma's at the current level.
//  		cout << "DEBUG 4:"<<endl;
		vector <char> LHS;			   // Parens must Exist as they were handled earlier.
		for(size_t i=left;i<right;i++){	          // left should be zero every time.
			if (i==left) {continue;}
			LHS.push_back(input[i]);
		}
		vector <char> temp;
		for(size_t i=right;i<input.size();i++){
			if (i==right) {continue;}
			temp.push_back(input[i]);
		}
		//ut <<"TempTest\n";
		result= newickToLVHIST(temp, currentDepth);
		vector < string > inParen;
		//ut <<"LHSTest\n";
		inParen= newickToLVHIST(LHS, currentDepth+1);
		//cout <<"LHSTestDONE\n";
		for(size_t i=0;i<inParen.size();i++){
			result.push_back(inParen[i]);
		}	
		cout << "DEBUG::newick-flag2!!!\n";
		return result;
	}
	///////////////////////////////////////////////////
	// Handle the a,b,c stuff.
	/////////////////////////////////////////////////
	else{
//	cout << "DEBUG 5:"<<endl;
		vector <char> LHS;			   // Parens must Exist as they were handled earlier.
		//cout <<"DEBUG:: handling comma"<<endl;
		for(size_t i=0;i<comma;i++){	          // left should be zero every time.
		//	if (i==left) {continue;}
			LHS.push_back(input[i]);
		}
		vector <char> temp;
		for(size_t i=comma;i<input.size();i++){
			if (i==comma) {continue;}
			temp.push_back(input[i]);
		}
		result= newickToLVHIST(temp, currentDepth);
		vector < string > leftOfComma;
		leftOfComma= newickToLVHIST(LHS, currentDepth);
		for(size_t i=0;i<leftOfComma.size();i++){
			result.push_back(leftOfComma[i]);
		}	
		cout << "DEBUG::newick-flag3!!!\n";
		return result;
		
	}


}

*/

