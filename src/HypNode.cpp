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

#include <sstream>

#include <string>

//NAMESPACEHACK
using namespace std;//sjk2010NAMESPACEHACK

#ifdef WIN32
#include <math.h>
#include <stdio.h>

#else
extern "C" {
#include <math.h>
#include <stdio.h>
}
#endif

#include <string>

#include "HypNode.h"

#ifdef WIN32

#define M_PI                  3.14159265359 

//Inverse Hyperbolic Cosine
double asinh(double dw) {  return log(dw+sqrt(dw*dw+1)); }

#endif

///////////////////////////////////////////////////////////////////////////
//
// OkTPtrArray classes
//
///////////////////////////////////////////////////////////////////////////

int SortKidArray::compareElements(void* e1, void* e2) {
  SortKid *s1 = (SortKid*) e1;
  SortKid *s2 = (SortKid*) e2;
  return(s1->compare(s2));
  //  return(((SortKid*)e1)->compare((SortKid*)e2));
}

int SortKid::compare(SortKid* o) {
  float onum = o->comparator;
  float mnum = comparator;
  if (onum==mnum) return 0;
  else if (onum < mnum) return -1;
  else return 1;
}
int HypNode::getLinkSize() {
	return linkSize;
}
void HypNode::setLinkSize(int input) {
	if (input>4095){ linkSize=4095; return;}
	if (input<0){ linkSize=0; return;}
	linkSize=input;
}
int HypNode::compare(HypNode* o) {
  //  string ourl = o->getId();
  //  string murl = theurl;
  string ourl = o->getId();
  string murl = theurl;
  
  if (ourl==murl) return 0;
  else if (ourl < murl) return 1;
  else return -1;
}

int HypNode::compareDist(HypNode* o) {
  double od = o->getSize();
  double md = getSize();
  if (od==md) return 0;
  else if (od < md) return 1;
  else return -1;
}

////////////////////////////////////////////////////////////////////////
//
// HypNode class
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////////////////

HypNode::HypNode()
{
  parent = (HypNode *)0;
  parentlink = (HypLink *)0;
  hd = (HypData *)0;
  drawFlags = 0;
  setEnabled(true);
  progeny = 0;
  phi = theta = 0.0;
  r = .1;
  C.identity();
  I.identity();
  theedge.identity();
  dist = 9999;
  size = 0;
  generation = 9999;
  longlab = "";
  shortlab = "";
  this->_numObjects++;
  linkSize=0;//sjk2011
  setTitleFlag(false);
}

HypNode::HypNode(HypData *h, HypNode *par, int lev)
{
  hd = h;
  Level = lev;
  parent = par;
  parentlink = NULL;
  drawFlags = 0;
  setEnabled(true);
  progeny = 0;
  phi = theta = 0.0; r = .1;
  C.identity(); I.identity();
  theedge.identity();
  dist = 9999;
  size = 0;
  generation = 9999;
  longlab = "";
  shortlab = "";
  this->_numObjects++;
  linkSize=0;//sjk2011
  setTitleFlag(false);
};

HypNode::~HypNode()
{
   children.erase(children.begin(),children.end());
   childlinks.erase(childlinks.begin(),childlinks.end());
   incoming.erase(incoming.begin(),incoming.end());
   outgoing.erase(outgoing.begin(),outgoing.end());
   thegroups.erase(thegroups.begin(),thegroups.end());
   this->_numObjects--;
}

StringArray HypNode::enumerateSubtree(int levels) {
  int i;
  StringArray strs;
  if (getEnabled())
    return strs;
  strs.push_back(theurl);
  if (levels != 0) {
    for (i = 0; i < children.size(); i++) {
      StringArray strs2 = children[i]->enumerateSubtree(levels-1);
      strs.insert(strs.end(),strs2.begin(),strs2.end());
    }
  } 
  return strs;
}


void HypNode::placeKids(int up, HypNode *base, SortKidArray *sk) {

  if (base==NULL || sk==NULL){return;}
  int i;
  int newlayer = 0;
  HypNode *ww;
  HypPoint to, from;
  HypTransform U,V;
  double layerr = 0.0; // biggest radius in current layer
  double thetaincr = 0.0;
  double edgelength;
  bool noGrandkids=false;//sjk20111
  int potentOffspring=0;
  int grandChildCount=0;
  int giantOffspring=0;

  /* make sure to add hyperbolic amount, not euclidean amount! */
  //edgelength = base->r + base->r*tanh(base->r)*hd->lengthfudge;
  edgelength = base->r + base->r*tanh(base->r)*hd->lengthfudge;// SJK2011 Trial

  if (base->getProgeny()==base->getChildCount()){noGrandkids=true;}// no grandkids finder
  for (int i = 0; i < base->children.size(); i++) {
    ww = base->children[i];
	grandChildCount+=ww->getChildCount();
	if (ww->getChildCount()>0){potentOffspring++;}
	if ((ww->getChildCount()>20)&& (ww->getProgeny()>300)){giantOffspring++;}
  }

  if (noGrandkids ||(( potentOffspring==1 || ( potentOffspring<4 &&( ww->getProgeny()-grandChildCount < 8)))&& grandChildCount < 16 )){
	  // This code should allow for better handling of binary trees
	  edgelength*=hd->broccoliTops;
  }
  if ((giantOffspring>1)||((base->getChildCount()>20)&& (base->getProgeny()>400))){edgelength*=1+(1-hd->broccoliTops);}
  

  /*  miserable failure
  if (outgoing.size()<2) {edgelength *=.8;}
  else {edgelength*=1.1;}
  */

  /* remember bug in TmHypTranslate: can't translate down Z axis... */
  edgelength*=hd->scaleFactor;
  U.translateHyp(edgelength, 0.0, 0.0);
  from.identity();
  from = U.transformPoint(from);
  from.normalizeEuc();
  //int thetaTwist=6.28/(base->getIndex()%2+2) ; //sjk2011
  //base->phi = 0.0; base->theta = 0.0+thetaTwist;
  // good attempt to get the alignment bushier,
  // worked great..but looks like garbage reverting to original.

  base->phi = 0.0; base->theta = 0.0;
  for (i = 0; i < sk->size(); i++) {
    ww = base->children[((*sk)[i])->index];
    ww->theedge.copy(from);
    /* reuse U to start building up the cumulative xform */
    U.translateHyp(-edgelength, 0.0, 0.0);
    if (!(base->phi == 0.0 && base->theta == 0.0 && newlayer == 0)) {
      if (base->phi != 0.0) {
	thetaincr = atan(tanh(ww->r)/(sin(base->phi)*sinh(base->r)));
	/* look ahead to make sure we'll fit */
	if (base->theta+thetaincr > 2*M_PI) {
	  base->theta -= 2*M_PI;
	  newlayer = 1;
	}
      }
      if (newlayer == 1) {
	base->phi += atan(tanh(layerr)/sinh(base->r));
	/* this will work best if they're sorted by childcount! */
	base->phi += atan(tanh(ww->r)/sinh(base->r));
	thetaincr = atan(tanh(ww->r)/(sin(base->phi)*sinh(base->r)));
	newlayer = 0;
	layerr = ww->r;
      }
      base->theta += thetaincr;

    }

    to.y = base->r*cos(base->theta)*sin(base->phi);
    to.z = base->r*sin(base->theta)*sin(base->phi);
    to.x = base->r*cos(base->phi);
    to.w = 1.0;

    V.rotateBetween(from, to);
    if (up && ww == this) {
      base->I = U.concat(V);
      (base->I).invert();
      //      fprintf(stderr, "up from %s to %s\n", (char*)ww->theurl, (char*)base->theurl);
    } else {
      ww->I = U.concat(V);
      //      fprintf(stderr, "down from %s to %s\n", (char*)theurl, (char*)ww->theurl);
    }
    if (ww->r > layerr) layerr = ww->r;
    if (base->phi == 0.0) {
      newlayer = 1;
    } else {
      thetaincr = atan(tanh(ww->r)/(sin(base->phi)*sinh(base->r)));
      base->theta += thetaincr;
    }
  }
}


//----------------------------------------------------------------------------
//                       void HypNode::layout(int up)                      
//............................................................................
//  
//----------------------------------------------------------------------------
void HypNode::layout(int up)
{
  int i;
  HypNode *ww, *base;
  SortKidArray sk;
  int numkids;
  double cumua = 0.0; // cumulative area
  double newa = 0.0; // new area
  double newr;

  if (children.size() == 0) {
    /* leaf node */
    r = hd->leafrad;
    I.identity();
    //    if (!up) return;
  }
  if (up) {
    if (!parent) return;
    base = parent;
    //  fprintf(stderr, "top of %s, set base to %s\n",
    //          (char*)theurl, (char*)base->theurl);
  } else {
    base = this;
  }
  base->progeny = 0;

  numkids = base->children.size();

  for (i = 0; i < numkids; i++) {
    SortKid *newkid = new SortKid();
    ww = base->children[i];
    if (! (up && ww == this)) {
      //      fprintf(stderr, "descend to %s\n", (char*)ww->theurl);
      ww->layout(0);
    }
    newa = (cosh(ww->r)-1)*2*M_PI;
    cumua += newa;
    base->progeny += ww->progeny + 1;
    newkid->index =i;
    newkid->comparator = ww->progeny;
    insertSorted(&sk, newkid);
  }
  /* add some since the circle packing area < sphere area */
  /* make sure to add hyperbolic amount, not euclidean amount! */

  if (numkids == 0) {
    //    cumua += cumua*tanh(cumua)*.0;
  } else if (numkids == 1) {
    cumua += cumua*tanh(cumua)*.2;
  } else if (numkids == 2) {
    cumua += cumua*tanh(cumua)*.9;
  } else if (numkids > 2 && numkids < 6 ) {
    cumua += cumua*tanh(cumua)*.6;
  } else {
    cumua += cumua*tanh(cumua)*.4;
  }

  base->r = asinh(sqrt(cumua/(2*M_PI)));
  base->r = (base->r < hd->leafrad) ? hd->leafrad : base->r;
  base->r = (base->r > hd->maxlength) ? hd->maxlength : base->r;

  placeKids(up, base, &sk);
  if (base->phi > 1.57) {
    // new phi 1.5
    newr = asinh(sqrt((pow(sinh(base->r),2)*(1-cos(base->phi)))*1.076)); 
    base->r = newr;
    //    fprintf(stderr, "recomputing phi %g r %g new %g\n", base->phi, base->r, newr);
    placeKids(up, base, &sk);
    //    fprintf(stderr, "take2 phi %g r %g\n", base->phi, base->r);
  } else if (up && !parent->parent) {
    //    fprintf(stderr, "up %g %g\n", base->phi, base->r);
  }
  if (up) parent->layout(up);
}

void HypNode::layoutC(int up)
{
  HypNode *ww, *base;
  if (up) {
    if (!parent) return;
    base = parent;
    base->C = (base->I).concat(C);
  } else {
    base = this;
  }
  //  fprintf(stderr, "layoutC %s %d base %s\n", (char*)theurl, up, (char*)base->theurl);
  for (int i = 0; i < base->children.size(); i++) {
    ww = base->children[i];
    //    if (ww->getEnabled()) {
      if (!(up && ww == this)) {
	ww->C = (ww->I).concat(base->C); 
	ww->layoutC(0);
      }
      //   }
  }    
  if (up) parent->layoutC(up);
}

void HypNode::layoutT() {
    C.identity(); 
    I.identity();
    theedge.identity();
}


string HypNode::save(int lev) {
  string str;
  string tab;
  ostringstream outerLevStream, outerEnabledStream;
  int i;
  
  for (i = -2; i < lev; i++) tab = tab + " ";
  tab = "";
  outerLevStream << lev;
  outerEnabledStream << this->getEnabled();
  str = str + tab + outerLevStream.str() + theurl + outerEnabledStream.str();
  for (i = 0; i < thegroups.size(); i++) {
      str = str + " " + thegroups[i];
  }
  str.append("\n");
  for (i = 0; i < children.size(); i++) {
    str.append((children[i])->save(lev+1));
  }
  
  tab = tab + " ";
  tab = "";
  for (i = 0; i < outgoing.size(); i++) {
    ostringstream levStream, enabledStream;
    levStream << lev + 1;
    enabledStream << outgoing[i]->getEnabled();
    HypNode *n = outgoing[i]->getChild();
    str = str + tab + levStream.str() + outgoing[i]->getChildId() + enabledStream.str();
    for (int j = 0; j < thegroups.size(); j++) 
	str = str + " " + n->thegroups[j];
    str.append("\n");
  }
  return str;
}

void HypNode::sort() {  
  /*
  int len;
  len = children.length();
  children.qsort(0, len);
  len = incoming.length();
  incoming.qsort(0, len);
  len = outgoing.length();
  outgoing.qsort(0, len);
  */
}


void HypNode::deleteChild(HypNode *n) {
  /*
  int gotit = -1;
  int i;
  for (i = 0; i < children.size(); i++) {
    if (n->getId() == children[i]->getId()) {
      gotit = i;
      break;
    }
  }
  if (gotit >= 0) {
    children.remove(gotit);
  } else {
    fprintf(stderr, "no remove %s\n", (char*)n->getId());
  }
  */

  HypNodeArray::iterator  hnIter;
  for (hnIter = children.begin(); hnIter != children.end(); hnIter++) {
    if (n->getId() == (*hnIter)->getId()) {
      children.erase(hnIter);
      break;
    }
  }
  
}
void HypNode::deleteChildLink(HypLink *l) { 
    HypLinkArray::iterator  hlIter;
  for (hlIter = childlinks.begin(); hlIter != childlinks.end(); hlIter++) {
    if (l->getId() == (*hlIter)->getId()) {
      // delete(hlIter);
      childlinks.erase(hlIter);
      break;
    }
  }
}
void HypNode::deleteOutgoing(HypLink *l) {
   HypLinkArray::iterator hlIter;
  for (hlIter = outgoing.begin(); hlIter != outgoing.end(); hlIter++) {
    if (l->getId() == (*hlIter)->getId()) {
      outgoing.erase(hlIter);
      break;
    }
  }
}

void HypNode::setEnabledOutgoing(int on, int descend) {
  //  if (!bEnabled) return;
  for (int j = 0; j < getOutgoingCount(); j++) {
    HypLink *l = getOutgoing(j);
    HypNode *n = l->getChild();
    l->setDesired(on);
    // if other end is disabled don't turn it on
    if ( (on && getEnabled() && n->getEnabled()) || !on)
      l->setEnabled(on);
  }
  if (descend) 
    for (int i = 0; i < getChildCount(); i++)
      children[i]->setEnabledOutgoing(on, descend);
}

void HypNode::setEnabledIncoming(int on, int descend) {
  //  if (!bEnabled) return;
  for (int j = 0; j < getIncomingCount(); j++) {
    HypLink *l = getIncoming(j);
    HypNode *n = l->getParent();
    l->setDesired(on);
    // if other end is disabled don't turn it on
    if ( (on && getEnabled() && n->getEnabled()) || !on)
      l->setEnabled(on);
  }
  if (descend) 
    for (int i = 0; i < getChildCount(); i++)
      children[i]->setEnabledIncoming(on, descend);
}


void HypNode::markEnable(int on, int descend) {
  int j;
  int wanted, value;
  HypLink *l;
  HypNode *n;
  setEnabled(on);
  for (j = 0; j < getChildCount(); j++) {
    l = getChildLink(j);
    n = l->getChild();
    wanted = l->getDesired();
    value = 0;
    if (n != (HypNode *)0 && on && getEnabled() && n->getEnabled() && wanted)
      value = 1;
    l->setEnabled(value);
  }
  l = getParentLink();
  if (l) {
    n = l->getParent();
    wanted = l->getDesired();
    value = 0;
    if (n != (HypNode *)0 && on && getEnabled() && n->getEnabled() && wanted)
      value = 1;
    l->setEnabled(value);
  }
  for (j = 0; j < getOutgoingCount(); j++) {
    l = getOutgoing(j);
    n = l->getChild();
    wanted = l->getDesired();
    value = 0;
    if (n != (HypNode *)0 && on && getEnabled() && n->getEnabled() && wanted)
      value = 1;
    l->setEnabled(value);
  }
  for (j = 0; j < getIncomingCount(); j++) {
    l = getIncoming(j);
    n = l->getParent();
    wanted = l->getDesired();
    value = 0;
    if (n != (HypNode *)0 && on && getEnabled() && n->getEnabled() && wanted)
      value = 1;
    l->setEnabled(value);
  }
  if (descend) 
    for (j = 0; j < getChildCount(); j++) {
      children[j]->markEnable(on, descend);
    }
}

void HypNode::insertSorted(SortKidArray *Q, SortKid *current) {
  int index;
  SortKid *n, *m;
  Q->push_back(current);
  //if (true) return;//DEBUG April-2012
  index = (*Q).size();
  index--;
  while (index > 0) {
	  n = (*Q)[index];
	  m = (*Q)[index-1];
    	  if (m->compare(n) == 1) {
      // Q->swap(index-1,index);
		  swap((*Q)[index-1],(*Q)[index]);
    		  index--;
    	  } else{
		break;
	  }
  }
}


double HypNode::getFurthest() { 
  double furth = C.T[3][3]; 
  return furth; 
}

unsigned long HypNode::_numObjects = 0;

void HypNode::set_XA(std::vector<std::string> input){
	extendedAttributes=input;
	DNDS_level=0;
	for(size_t i=0;i<extendedAttributes.size();i++){
		
		int pos=extendedAttributes[i].find("DNDS");
		if (pos==extendedAttributes[i].npos) continue;
		//if (true) DNDS_level=1;
		std::string RHS=extendedAttributes[i].substr(pos+5);
		int pos1=RHS.find(":");
		int pos2=RHS.find("]");
		if (pos2<pos1){pos1=pos2;}
		//if (pos2==RHS.npos()) continue;

		//string dndsVal= extendedAttributes[i].substr(pos+5,4);
		string dndsVal= extendedAttributes[i].substr(pos+5,pos1-4);

		float val=atof(dndsVal.c_str());
		if (val> 1.0) DNDS_level=1;
		dndsVal.insert(0,"dN/dS ");
		extendedAttributes[i]=dndsVal;
		//if (true) DNDS_level=1;
		
		
	}
}