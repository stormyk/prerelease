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

#include <string>

//NAMESPACEHACK 
using namespace std;//sjk2010		

#include <stdio.h> 

//#include <unistd.h> 

#ifdef WIN32 
#  include <sys/timeb.h>
#  if 0 && defined(_DEBUG)
#    include <AFX.H>  // For TRACE.
#  else
#    define TRACE printf
#  endif
#else
#  include <sys/time.h>
#endif

#include <math.h>
#include "HypQuat.h"
#include "HypViewer.h"


#ifdef WIN32
int gettimeofday(timeval *tv, int *tz)
{
  struct timeb timebuffer;

  ftime(&timebuffer);
  tv->tv_sec = (long) timebuffer.time;
  tv->tv_usec = (long) (1000 * (timebuffer.millitm));
  return 0;
}
#endif

struct mgcontext *_mgc = NULL;
struct mgfuncs *_mgf = NULL;

HypViewer *thehv = NULL;



HypViewer::HypViewer(HypData *d, HDC w,HWND hwin) {
  leafColorScheme=false;
  wid = w;
  win = hwin;


  hiliteCallback = (void (*)(const string &,int,int))0;
  pickCallback = (void (*)(const string &,int,int))0;
  hd = d;
  pick = 0;
  hiliteNode = -1;
  idleframe = 0;
  thehv = this;
  origin.identity();
  xaxis.x = 0.9;
  xaxis.y = -0.2;
  xaxis.z = 0.0;
  xaxis.w = 1.0;
  // fix "identity" quaternion so it matches the slightly tilted xaxis
  HypPoint rr = origin.sub(xaxis);
  HypTransform R;
  R.rotateBetween(rr, xaxis); 
  iq.fromTransform(R);

  leftClickCB=NULL;
  middleClickCB=NULL;
  rightClickCB=NULL;
  leftDragCB=NULL;
  middleDragCB=NULL;
  rightDragCB=NULL;
  passiveCB=NULL;
  motionCount = 0;
  passiveCount = 0;
  labelbase = 0;
  labelscaledsize = 0;
  labeltoright = 0;
  pickrange = 1.0 / (pow((long double)2,32) -1.0);
}

HypViewer::~HypViewer()
{

  labelscaledsize = 0;
}


void HypViewer::setGraph(HypGraph *h){
  DrawnQ.erase(DrawnQ.begin(),DrawnQ.end());
  TraverseQ.erase(TraverseQ.begin(),TraverseQ.end());
  DrawQ.erase(DrawQ.begin(),DrawQ.end());
  DrawnLinks.erase(DrawnLinks.begin(),DrawnLinks.end());
  hg = h; 
  HypNode *n = hg->getNodeFromIndex(hd->centerindex);
  clearFlashLink();
  if (!n) return;
  setGraphCenter(n->getIndex());
  setCurrentCenter(n);
  largest = -1.0;
  idleFunc(1);
}

void HypViewer::newBackground() {
  framePrepare();
  glClearColor(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2], 1.0);
}

void HypViewer::newLabelFont() {
  drawStringInit();
}

void HypViewer::newSphereColor() {
  sphereInit();
}

void HypViewer::newLayout() {
  HypNode *n = getCurrentCenter();
  HypNode *best = (HypNode *)0;
  while (n) {
    if (!n->getEnabled())
      best = n;
    n = n->getParent();
  }
  if (best != (HypNode *)0) {
    HypNode *lastcenter = best->getParent();
	if (!lastcenter){
      lastcenter = hg->getNodeFromIndex(hd->centerindex);
	}
    //    if (lastcenter) setGraphCenter(lastcenter->getIndex());
    setCurrentCenter(lastcenter);
    // there was something not enabled above us
  }
  DrawQ.erase(DrawQ.begin(),DrawQ.end());
  TraverseQ.erase(TraverseQ.begin(),TraverseQ.end());
}





#ifdef HYPGLUT
  
  //--------------------------------------------------------------------------
  //                      void HypViewer::afterRealize()                     
  //..........................................................................
  //  
  //--------------------------------------------------------------------------
  void HypViewer::afterRealize()
  {
    glGetIntegerv ( GL_VIEWPORT, vp );      /* get viewport size */ 
    //printf("GLUT afterRealize vp=[%d %d %d %d], labelscaledsize=%g\n", vp[0], vp[1], vp[2], vp[3], labelscaledsize );
    resetLabelSize();
    glInit();
    drawStringInit();
    idleFunc(1);
  }

#endif  // HYPGLUT

#ifdef WIN32
void HypViewer::afterRealize(HGLRC cx, int w, int h) {
  cxt = cx;
  wglMakeCurrent(wid,cxt);
  vp[0] = 0;
  vp[1] = 0;
  vp[2] = w;
  vp[3] = h;
  glInit();
  drawStringInit();
  idleFunc(1);
  frameBegin(hd);
  //drawSphere();
  frameEnd();
}
#endif

//--------------------------------------------------------------------------
//                       void HypViewer::sphereInit()                      
//..........................................................................
//  
//--------------------------------------------------------------------------
void HypViewer::sphereInit(){
	sphereInit(HV_INFSPHERE,1.0);
	sphereInit(HV_INFSPHERE_FAINT,0.5);
	sphereInit(HV_INFSPHERE_VERY_FAINT,0.2);

}

//--------------------------------------------------------------------------
//                       void HypViewer::sphereInit(glEnum,float)                      
//..........................................................................
//  
//--------------------------------------------------------------------------
void HypViewer::sphereInit(int list,float alpha)
{
  // sphere at inifinity stuff 
  // don't use GLU sphere object since can't get smooth wireframe
  float phi, theta;
  float phistep = 3.14/12.0;
  float thetastep = 6.28/8.0;
  //glNewList(HV_INFSPHERE, GL_COMPILE);
  glNewList(list, GL_COMPILE);
  glDisable(GL_CULL_FACE);
  


  //glLineWidth(1.0);
  glLineWidth(1.0);//sjk2010
  glColor4f(hd->colorSphere[0], hd->colorSphere[1], hd->colorSphere[2],alpha);
  for (phi = 0.0; phi < 3.14; phi += phistep) {
    //glBegin(GL_LINE_LOOP);
	glBegin(GL_LINES);
    for (theta = 0.0; theta < 6.28; theta += .1) {
	    glVertex3f(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi));
	    glVertex3f(cos(theta+0.1)*sin(phi), cos(phi), sin(theta+0.1)*sin(phi));
    }
    glEnd();
  }
  for (theta = -.35; theta < 3.14; theta += thetastep) {
    glBegin(GL_LINES);
    for (phi = 0.0; phi < 6.28; phi += .1) {
	    glVertex3f(cos(theta)*sin(phi), cos(phi), sin(theta)*sin(phi));
	    glVertex3f(cos(theta)*sin(phi+0.1), cos(phi+0.1), sin(theta)*sin(phi+0.1));
    }
    glEnd();
	// sjk2011   changed from .1 to .01 for poster output.
	//sjk2011  back to .1  overflowed the poster stack.
  }
  glEndList();
}

//----------------------------------------------------------------------------
//                         void HypViewer::glInit()                        
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::glInit()
{
  //printf("glInit\n");
  int i, j;
  framePrepare();
  glClearColor(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2], 1.0);
  sphereInit();
  nodeScale();
  // OGL options
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  glShadeModel(GL_SMOOTH);
/*
  // sjk2011
  // FOG CODE
  //GLfloat angle = 0.0;

	GLfloat density = 0.23; //set the density to 0.3 which is
	//acctually quite thick

	GLfloat fogColor[4] = {0.9, 0.9, 0.9, 0.4}; //set the for
	//color to grey
	glEnable (GL_FOG); //enable the fog
	glFogi (GL_FOG_MODE, GL_LINEAR); //set the fog mode to GL_LINEAR (GL_EXP2 has no start stop)
	glFogfv (GL_FOG_COLOR, fogColor); //set the fog color to
	//our color chosen above
	glFogf (GL_FOG_START, 3.0f); //set the density to the
	glFogf (GL_FOG_END, 5.0f); //set the density to the
	glFogf (GL_FOG_DENSITY, density); //set the density to the
	//value above
	glHint (GL_FOG_HINT, GL_NICEST); // set the fog to look the
							//nicest, may slow down on older cards
   // this doesnt look right.
  //  the stuff closest to the camera is really small, by the time we have things in proper view they are fogged
  //  but if it isnt foogged by then then the fog is mostly useless.
  //sjk2011 end
*/
  camInit();
  glMatrixMode(GL_MODELVIEW);  
  gluLookAt(0.0, 0.0, 3.0, 
	    0.0, 0.0, 0.0, 
	    0.0, 1.0, 0.0);
  glGetDoublev(GL_MODELVIEW_MATRIX, (double*)viewxform.T);
  vx = (double*)(viewxform.T);


  worldxform.identity();
  // start out facing right
  wx = (double*)(worldxform.T);
  wx[0] = -1; wx[10] = -1;
  // make it tilt slightly so parent and child text don't always collide 
  // in the single-child case
  HypPoint rr = origin.sub(xaxis);
  HypTransform R;
  R.rotateBetween(rr, xaxis); 
  worldxform = worldxform.concat(R);
  identxform.copy(worldxform);
  // clear to a blank color during rest of setup
  int oldidle = idleframe;
  idleframe = 0;
  frameBegin(hd);
  frameEnd();
  idleframe = oldidle;
  return;
}

//----------------------------------------------------------------------------
//                        void HypViewer::camInit()                        
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::camInit()
{
  // camera setup
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float fov;
  float horizFudge=1.0; //SJK 2010 to fit windows closer to edge
  float aspectratio = vp[2]/((float)vp[3]);
  
  if (!(hd->keepAspect)) {
    aspectratio = 1.0;
	horizFudge=1.05; //sjk2010 to get expanded box closer to edge
  }
  if (aspectratio > 1.0) {
    fov = 180.0*(2.0 * atan2 (2.25/2.0, 3.0))/3.1415;
	
  } else {
    fov = 180.0*(2.0 * atan2 (2.25/(2.0*aspectratio), 3.0))/3.1415;
    if (fov > 180) fov = 179.0;
  }
  gluPerspective( (fov/horizFudge), aspectratio, 1.5, 4.5);//sjk201
  glGetDoublev(GL_PROJECTION_MATRIX, (double*)camxform.T);
  cx = (double*)(camxform.T);
  resetLabelSize();
  return;
}

// todo: findCenter, HypNodeQueue

//----------------------------------------------------------------------------
//                       void HypViewer::drawFrame()                       
//............................................................................
//  
//----------------------------------------------------------------------------


void HypViewer::drawFrame()
{
  
  if (!labelscaledsize) return; // not yet initialized
   HypNode *centernode, *current, *node;
  HypNodeArray *nodes;
  int i, generation;
  struct timeval now, start, allotted, elapsed, elapsedidle;
  gettimeofday(&now, NULL);
  start = now;
  allotted = hd->dynamictime;
  elapsed.tv_sec = 0; elapsed.tv_usec = 0;
  framePrepare();
  //    fprintf(stderr, "TOP %d %d idle %d\n", now.tv_sec, now.tv_usec, idleframe);

  if (!idleframe) {


    centernode = getCurrentCenter();
    centernode->setDrawn(0); // just in case
	
    for (i = 0; i < DrawnQ.size(); i++) {
      DrawnQ[i]->setDrawn(0);
      DrawnQ[i]->setDistance(9999);
      DrawnQ[i]->setSize(-1);
      DrawnQ[i]->setGeneration(9999);
    }
    if (centernode->getDrawn() == 1) {
      //      fprintf(stderr, "no nodes drawn\n");
    }
    DrawnQ.erase(DrawnQ.begin(),DrawnQ.end());
    TraverseQ.erase(TraverseQ.begin(),TraverseQ.end());
    DrawQ.erase(DrawQ.begin(),DrawQ.end());
    DrawQ.push_back(centernode);
    centernode->setGeneration(0);
	for (i = 0; i < DrawnLinks.size(); i++){
      DrawnLinks[i]->setDrawn(0);
	}
    DrawnLinks.erase(DrawnLinks.begin(),DrawnLinks.end());
    framestart = now;
    largest = -1.0;
    HypTransform T;
    T.copy(worldxform);
    T.invert();
    frameorigin = T.transformPoint(origin);
    frameorigin.normalizeEuc();
    frameBegin(hd);
	drawStatus();
	
  } else {  // THIS IS AN IDLE FRAME
    elapsedidle.tv_sec = now.tv_sec - framestart.tv_sec;
    if (now.tv_usec < framestart.tv_usec) {
      // manual carry...
      elapsedidle.tv_usec = now.tv_usec + (1000000-framestart.tv_usec);
      elapsedidle.tv_sec--; 
    } else {
      elapsedidle.tv_usec = now.tv_usec - framestart.tv_usec;
    }
    if (elapsedidle.tv_sec < hd->idletime.tv_sec    ||
	   (elapsedidle.tv_sec == hd->idletime.tv_sec   && 
	    elapsedidle.tv_usec < hd->idletime.tv_usec)) {
      frameContinue(hd);
      //            fprintf(stderr, "idledraw %d %d\n", now.tv_sec, now.tv_usec);
    } else {
      DrawQ.erase(DrawQ.begin(),DrawQ.end());
      TraverseQ.erase(TraverseQ.begin(),TraverseQ.end());
      idleFunc(0);
      //            fprintf(stderr, "finished idle\n");
      return;
    }
  }
  nodesdrawn = 0;
  linksdrawn = 0;

  // draw until run out of time
  while ( (elapsed.tv_sec < allotted.tv_sec ||
	  (elapsed.tv_sec == allotted.tv_sec && 
	  elapsed.tv_usec < allotted.tv_usec))) {
    if (DrawQ.size() > 0) {
			current = DrawQ[0];
			if (!current) {
				idleFunc(0);
				TraverseQ.erase(TraverseQ.begin(),TraverseQ.end());
				break;
			}
		DrawQ.erase(DrawQ.begin());
		if (current->getDrawn()){	continue;}//sjk2011 DEBUG SHORT CIRCUIT-- NO CHANGE
	} else if (TraverseQ.size() > 0) {
		current = TraverseQ[0];
		TraverseQ.erase(TraverseQ.begin());
		node = current->getParent();
		generation = current->getGeneration();
		if (node && !node->getDrawn() && node->getEnabled()) {
		//if (node) { //sjk2011 DEBUG SHORT CIRCUIT// no change
			DrawQ.push_back(node);
			node->setGeneration(generation+1);
		}
		nodes = current->getChildren();
		for (i = 0; i < (*nodes).size(); i++) {
			HypNode *n = (*nodes)[i];
			if (n && !n->getDrawn() && n->getEnabled()) {
			//if (n) { //sjk2011 DEBUG SHORT CIRCUIT// no change
				DrawQ.push_back(n);
				n->setGeneration(generation+1);
			}
		}
        continue;
    } else {
      idleFunc(0);
      TraverseQ.erase(TraverseQ.begin(),TraverseQ.end());
      DrawQ.erase(DrawQ.begin(),DrawQ.end());
      //            fprintf(stderr, "finished frame\n");
      break;
    }
    drawNode(current);
    current->setDrawn(1);
    drawLinks(current);
    insertSorted(&DrawnQ, current);
    insertSorted(&TraverseQ, current);
    gettimeofday(&now, NULL);
    elapsed.tv_sec = now.tv_sec - start.tv_sec;
    if (now.tv_usec < start.tv_usec) {
      // manual carry...
      elapsed.tv_usec = now.tv_usec + (1000000-start.tv_usec);
      elapsed.tv_sec--; 
    } else {
      elapsed.tv_usec = now.tv_usec - start.tv_usec;
    }
  }
  //    fprintf(stderr, "drew %d nodes, %d links %s\n", nodesdrawn, linksdrawn, 	    idleframe ? "idle" : "");

  frameEnd();
  
  return;
}

//----------------------------------------------------------------------------
//                     void HypViewer::drawPickFrame()                     
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawPickFrame()
{
  struct timeval now, start, allotted, elapsed;
  gettimeofday(&now, NULL);
  start = now;
  allotted = hd->picktime;
  elapsed.tv_sec = 0; elapsed.tv_usec = 0;
  picknodesdrawn = 0; picklinksdrawn = 0;
  int index = 0;
  HypNode *current;
  int length = DrawnQ.size();
  frameBegin(hd);
  while ( index < length && 
	  (elapsed.tv_sec < allotted.tv_sec ||
	  (elapsed.tv_sec == allotted.tv_sec && 
	  elapsed.tv_usec < allotted.tv_usec))) {
    current = DrawnQ[index];
    drawNode(current);
    drawLinks(current);
    gettimeofday(&now, NULL);
    elapsed.tv_sec = now.tv_sec - start.tv_sec;
    elapsed.tv_usec = now.tv_usec - start.tv_usec;
    index++;
  }
  //  fprintf(stderr, "drew %d picknodes, %d picklinks\n", picknodesdrawn, picklinksdrawn);
  return;
}

//----------------------------------------------------------------------------
//   void HypViewer::insertSorted(HypNodeDistArray *Q, HypNode *current)   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::insertSorted(HypNodeDistArray *Q, HypNode *current)
{
  int index;
  HypNode *n, *m;
  Q->push_back(current);
  index = (*Q).size();
  index--;
  while (index > 0) {
    n = (*Q)[index];
    m = (*Q)[index-1];
    if (m->compareDist(n) == -1) {
      //      Q->swap(index-1,index);
      swap((*Q)[index-1],(*Q)[index]);
      index--;
    } else
      break;
  }
  return;
}

//----------------------------------------------------------------------------
//                  void HypViewer::drawLinks(HypNode *n)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawLinks(HypNode *n)
{
  glLineWidth(lineWidth);//sjk2010
  int i;
  HypLink *l;
  HypNode *m;
  if (hd->linkPolicy != HV_LINKINHERIT){
	glEnable(GL_LINE_SMOOTH);//SJK2010
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);//SJK2010
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);//SJK2011
	
  }else {
	  //glDisable(GL_LINE_SMOOTH);//SJK2010
	 //	glDisable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);//SJK2010
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);// sjk debug removed
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);//sjkdebug add
		//glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);//SJK2010
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);//SJK2010

  }

  if (! n->getEnabled()) return;
  if (n->getGeneration() < hd->generationLinkLimit-1) {
    //glLineWidth(2.0);
	//glLineWidth(1.0);//sjk2010
	glLineWidth(lineWidth);//sjk2010
    l = n->getParentLink();
	if (lineWidth > 8.1){ glLineWidth(((n->getLinkSize())/255.0)+0.3);}
	if (lineWidth > 16.1){ glLineWidth(8-((n->getLinkSize())/512.0)+0.3);}
	if (l && !l->getDrawn()){
		glLineWidth(lineWidth);
		if (lineWidth > 8.1){ glLineWidth(((l->getChild()->getLinkSize())/255.0)+0.3);}
		//if (lineWidth > 16.1){ glLineWidth(8-((l->getChild()->getLinkSize())/512.0)+0.3);}
		drawLink(l);
	}
    for(i = 0; i < n->getChildCount(); i++) {
      l = n->getChildLink(i);
	  glLineWidth(lineWidth);
	  if (lineWidth > 8.1){ glLineWidth(((l->getChild()->getLinkSize())/255.0)+0.3);}
	  //if (lineWidth > 16.1){ glLineWidth(8-((l->getChild()->getLinkSize())/512.0)+0.3);}
      drawLink(l);
    }
  }

  //glLineWidth(0.5);

  for(i = 0; i < n->getOutgoingCount(); i++) {
    l = n->getOutgoing(i);
    m = l->getChild();
    if (m && (m->getGeneration() < hd->generationLinkLimit-1
		|| n->getGeneration() < hd->generationLinkLimit-1)){
		glLineWidth(lineWidth);		
		if (lineWidth > 8.1){ glLineWidth(((l->getChild()->getLinkSize())/255.0)+0.3);}
		//if (lineWidth > 16.1){ glLineWidth(8-((l->getChild()->getLinkSize())/512.0)+0.3);}
		drawLink(l);
	}
  }
  
  //glLineWidth(0.5);
  /*
  for(i = 0; i < n->getIncomingCount(); i++) {
    l = n->getIncoming(i);
    m = l->getParent();
    if (m && (m->getGeneration() < hd->generationLinkLimit-1
		|| n->getGeneration() < hd->generationLinkLimit-1)){
		if (lineWidth > 8.1){ glLineWidth(((4095-l->getChild()->getLinkSize())/255.0)+0.3);}
		//if (lineWidth > 16.1){ glLineWidth(8-((l->getChild()->getLinkSize())/512.0)+0.3);}
		glLineWidth(lineWidth);
		drawLink(l);
	}
  }
  */
  return;
}

//----------------------------------------------------------------------------
//                   void HypViewer::drawNode(HypNode *n)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawNode(HypNode *n)
{
	if (!n){return;}
	if (! n->getEnabled()){  return;}

	//bool greenLeaf;
	//bool blueNode;
	//bool greenLight=false;
	//bool blueLight=false;
	//bool hasData=false;

	//greenLeaf= (0==n->getChildCount());
	//blueNode=!greenLeaf;
	//greenLeaf=(greenLeaf&&hd->leafColorScheme);
	//blueNode=(blueNode&&hd->leafColorScheme);
	//int branchLength=0;
	//if (n->get_XA().size()>3){
	//	if (&n->get_XA()!=NULL){
	//		branchLength= atoi(n->get_XA()[2].c_str());
	//		//branchLength=0;
	//	}
	//}
	//if (branchLength>0){greenLight=greenLeaf;blueLight=blueNode;};


	glDisable(GL_CULL_FACE); //ensure that they're opaque
	glEnable(GL_DEPTH_TEST); //for fog effects.
  //glEnable(GL_CULL_FACE);
   //glCullFace(GL_BACK);
	  glMatrixMode(GL_MODELVIEW);
	  
  n->setLabelDrawn(false);	  
  double        xs;
  double        xdist;
  double        curdist;
  unsigned int  name = n->getIndex();
  glLoadName(name);
  //  fprintf(stderr, "drew %d\n", name);
  glPushMatrix();
  HypTransform  T = n->getC();
  double        glm[16];
  memcpy(&glm[0],T.T[0],sizeof(double)*4);
  memcpy(&glm[4],T.T[1],sizeof(double)*4);
  memcpy(&glm[8],T.T[2],sizeof(double)*4);
  memcpy(&glm[12],T.T[3],sizeof(double)*4);
  //printf("glm\n"); for(int i=0;i<16;i++){printf("  %f", glm[i]); if(i%4==3)printf("\n");}

  // glMultMatrixd((double*)T.T);
  glMultMatrixd(glm);
  HypTransform  U;
    HypPoint      v, v1, v2, xp;
    U = T.concat(worldxform);
    xp = U.transformPoint(origin);
    // don't normalize xp!!!
    xp.x += HV_LEAFSIZE;
    xp.z += HV_LEAFSIZE+.02;
    v1 = W2S.transformPoint(xp);
    v1.normalizeEuc();
    xp.x -= 2*HV_LEAFSIZE;
    v2 = W2S.transformPoint(xp);
    v2.normalizeEuc();
	if (labeltoright){
			n->setLabelPos(v1.x, v1.y, -v1.z+.0);
		}else {
			n->setLabelPos(v2.x, v2.y, -v2.z+.0);
		}
	if (!n ||  ! n->getEnabled()){  return;}
  if (!pick) {
    
    xs = v1.distanceEuc(v2);
    if (xs < 0.0) fprintf(stderr, "distance returned negative...\n");
    xs = fabs(xs);
    curdist = thecenter->getDistance();
    v = T.transformPoint(origin);
    xdist = v.distanceHyp(frameorigin);
    if (xs > largest && xs < 100.0 && xdist <= curdist) {
		if (hd->bCenterLargest){
			setCurrentCenter(n);
		}
      //fprintf(stderr, "NEW CENTER: %d %g\n", name, xs);
      largest = xs;
    }
    n->setSize(xs);
    n->setDistance(xdist);
   
    
	
		
	if (n->getGeneration() < hd->generationNodeLimit) {
      // draw label
		if (hd->labels>0) {
			if (xs > labelscaledsize) {
				drawLabel(n);
				n->setLabelDrawn(true);
			}
		}
		nodesdrawn++;
	}
	if (!n->getLabelDrawn()&& n->getTitleFlag()){drawLabel(n);n->setLabelDrawn(true);}
  } else {

    picknodesdrawn++;
    
  }
    glColor4f(.5,1.0,1.0,hd->nodeAlpha);
  if (hd->bCenterShow && n->getGeneration() == 0) {
    //    fprintf(stderr, "hilite %d\n", name);
    glColor4f(1,0,0,hd->nodeAlpha);
  } else if (n->getHighlighted()) {
    //    fprintf(stderr, "hilite %d\n", name);
    glColor4f(hd->colorHilite[0],
	      hd->colorHilite[1],
	      hd->colorHilite[2],
		hd->nodeAlpha);
  } else if (n->getSelected()) {
    //    fprintf(stderr, "select %d\n", name);
	   
    glColor4f(hd->colorSelect[0],
	      hd->colorSelect[1],
	      hd->colorSelect[2],
		hd->nodeAlpha);
  } else {
	  
	  float *col = hg->getColorNode(n);
	  
	  //if (greenLeaf) {col=hd->greenLeaf;}
	  //if (greenLight&&hd->isChordateTree) {col=hd->greenLight;}

	  ////if (true) {col=hd->greenLeaf;}
	  //if (blueNode) {col=hd->blueNode;}
	  //if (blueLight&&hd->isChordateTree) {col=hd->blueLight;}
	  hg->setLinkSize(n);//  prep for line draw
	  glColor4f(col[0], col[1], col[2],hd->nodeAlpha);
  }
    if (hd->linkPolicy != HV_LINKINHERIT){
	glEnable(GL_LINE_SMOOTH);//SJK2010
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);//SJK2010
  }else {
	  //	glDisable(GL_LINE_SMOOTH);//SJK2010
	//	glDisable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);//SJK2010
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_DONT_CARE);//SJK2010

  }
  glLineWidth(0.1);
  int  cubeFaceCurrent=HV_CUBEFACES;
  int  cubeEdgeCurrent=HV_CUBEEDGES;
  if (hd->clownColorScheme){
		cubeFaceCurrent=HV_SMALL_CUBEFACES;
		cubeEdgeCurrent=HV_SMALL_CUBEEDGES;
  }
  int NODE_DRAW_SHAPE=DRAW_SHAPE;
  if (DRAW_SHAPE>=2) { //COMBO SHAPES!!! Sphere/Node
	  NODE_DRAW_SHAPE=DRAW_SHAPE%2;
	  if (n->getProgeny()==0){NODE_DRAW_SHAPE=(DRAW_SHAPE+1)%2;}
  }

  if (n->getGeneration() < hd->generationNodeLimit) {
	  if (NODE_DRAW_SHAPE==1) {glCallList(HV_SPHEREFACES);}
	  else{                 glCallList(cubeFaceCurrent);}
	  if (!pick && (xs > hd->edgesize)){
			if (NODE_DRAW_SHAPE==1){
				glCallList(HV_SPHEREEDGES);
			}else{             
				glCallList(cubeEdgeCurrent);
			}
	  }
  }else{//sjk2011 DEBUG ONLY-- Testing for non-draw bug
	  if (1!=NODE_DRAW_SHAPE)glCallList(HV_CUBEEDGES);// GHOST BOXES
  }
  glPopMatrix();
  if (n->getHighlighted()){ 
	  if (n->getParentLink()){drawLink(n->getParentLink());}
  }
  return;
}

//----------------------------------------------------------------------------
//                   void HypViewer::drawLink(HypLink *l)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawLink(HypLink *l)
{
  glMatrixMode(GL_MODELVIEW);
  if (!l) return;
  if (!l->getEnabled()) return;


  unsigned int name;
  name = l->getIndex();
  glLoadName(-name-1);
  const float *cFrom, *cTo;
  if (false) {
    HypNode *p = l->getParent();
    HypNode *c = l->getChild();
    cFrom = hg->getColorNode(p);
    cTo = hg->getColorNode(c);
  } else if (false) {
    cFrom = hd->colorLinkFrom;
    cTo = hd->colorLinkTo;
  } else { // HV_LINKLOCAL
    cFrom = l->getColorFrom();
    cTo = l->getColorTo();
    //cTo = hd->dndsColorLinkFrom;//sjk2011 debug
  }
  
  //if (l->getChild()->getDndsLevel()>0){
//	  cTo = hd->dndsColorLinkFrom;//sjk2011 debug
//	 cFrom = hd->dndsColorLinkTo;//sjk2011 debug
//  }
  

  glBegin(GL_LINES);
  glColor4f(cFrom[0], cFrom[1], cFrom[2], cFrom[3]);
  glVertex4d(l->start.x, l->start.y, l->start.z, l->start.w);
  glColor4f(cTo[0], cTo[1], cTo[2], cTo[3]);
  glVertex4d(l->end.x, l->end.y, l->end.z, l->end.w);
  glEnd();
  
 
  int existing=l->getDrawn();
  if (existing==0){
	if (prevHighlight &&(prevHighlight!=l)) {drawLink(prevHighlight);prevHighlight=l;}
	//if (prevHighlight &&(prevHighlight!=l)) {drawLink(prevHighlight);}
	if(l->getChild()->getHighlighted()){ drawLabel(l);}	  
	l->setDrawn(1);
	linksdrawn++;
	DrawnLinks.push_back(l);
  }
  return;
}

void HypViewer::drawStatus(){
	int line=1;
	if (hd->clownColorScheme){
		drawStatus(line,"Dense Chordate View: On",1);
	  line++;
	}else{
		drawStatus(line,"Dense Chordate View: Off",1);
	  line++;
	}
	if (true){	
		char  fogArray[20];
		std::string fogString;
		sprintf(fogArray,"Fog Level: %d",hd->fogLevel);
		fogString.assign(fogArray);
		drawStatus(line,fogString,1);
		line++;
	}

	if (hd->isChordateTree){
		drawStatus(line,"Chordate Extended Attributes: On",1);
		line++;
	}else{
		drawStatus(line,"Chordate Extended Attributes: On",1);
		line++;
	}
    if (true){	
		char  lineArray[50];
		std::string lineString;
		int lw=lineWidth/1;
		sprintf(lineArray,"Line Level: %d",lw);
		if (10==lw){
			sprintf(lineArray,"Line Level: Adaptive",lw);
		}
		lineString.assign(lineArray);
		drawStatus(line,lineString,1);
		line++;
	}

    if (true){	
		char  lineArray[50];
		std::string lineString;
		sprintf(lineArray,"Label Sparsenes: %0.2f",hd->labelsize);
		lineString.assign(lineArray);
		drawStatus(line,lineString,1);
		line++;
	}
}
//----------------------------------------------------------------------------
//                  void HypViewer::drawStatus(std::string input)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawStatus(int lineNumber,std::string input, int highlight){
	//if (!input.compare("dN/dS 0.000")){return;}
	//input.assign("xyzzy");//DEBUG-2012

	int len;
	const char *str;
    str = input.c_str();
	if ( (len = strlen(str) ) < 1){return;}


   
	
	int labw = drawStringWidth(str);
    int charwidth = labw/len;
	double labr, labl;
	float labx,laby,labz;
    labx=10.0;
	laby=10.0*lineNumber;
	labz=0.0;

    labl = 0.0;
    labr = labw*1.4;
	labr = labw;
	
  if (highlight==3){laby+=12;}
  glPushMatrix();
  glLoadIdentity();
  glPolygonMode(GL_FRONT, GL_FILL);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vp[2] - vp[0], 0, vp[3] - vp[1], -1.0, 1.0);

  /*
  glPushMatrix();
  glPolygonMode(GL_FRONT, GL_FILL);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glOrtho(0, vp[2] - vp[0], 0, vp[3] - vp[1], -1.0, 1.0);
  */
  
  
  if (highlight==0){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],0.90);}
  if (highlight==1){glColor4f(hd->colorLabel[0], hd->colorLabel[1], hd->colorLabel[2],0.90);}
  if (highlight==2){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],0.90);}
  if (highlight==3){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],0.90);}
  if (highlight==4){glColor4f(hd->blueNode[0],hd->blueNode[1],hd->blueNode[2],0.90);}
  
   
  
  if(true){
	  glBegin(GL_TRIANGLE_STRIP);	  
			glVertex3d(labx+labl,		laby-labdescent, labz );
			glVertex3d(labx+labr,		laby-labdescent, labz );
			glVertex3d(labx+labl,		laby+labascent,	labz );
			glVertex3d(labx+labr,		laby+labascent,	labz );
	  glEnd();
  }

  if (highlight==0){glColor4f(hd->colorLabel[0], hd->colorLabel[1], hd->colorLabel[2],1.00);}
  if (highlight==1){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],1.0);}
  if (highlight==2){glColor4f(hd->colorLinkFrom[0],hd->colorLinkFrom[1],hd->colorLinkFrom[2],1.0);}
  if (highlight==3){glColor4f(hd->colorLinkTo[0],hd->colorLinkTo[1],hd->colorLinkTo[2],1.0);}
  if (highlight==4){glColor4f(hd->colorLabel[0], hd->colorLabel[1], hd->colorLabel[2],1.00);}

  glRasterPos3d(labx+labl, laby, labz+.001);
  //if (highlight==3){glRasterPos3d(lab.x+labl, lab.y, lab.z+.001);}
  gl2psText(str, "Arial", 14);//debug/sjk2011
	
  
	drawString(str);
   
  
  glPopMatrix(); 
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
    
    return;


}


//----------------------------------------------------------------------------
//                  void HypViewer::drawLabel(HypPoint *p,std::string input)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawLabel(HypPoint p,std::string input, int highlight){
	if (!input.compare("dN/dS 0.000")){return;}
	//input.assign("xyzzy");//DEBUG-2012
    int len;
	const char *str;
    str = input.c_str();
	if ( (len = strlen(str) ) < 1){return;}
	HypPoint lab = p;
	int labw = drawStringWidth(str);
    int charwidth = labw/len;
	double labr, labl;


if (labeltoright) {
    labl = 0.0;
    labr = labw*1.4;
	if (true){
		labr = labw;
	}
  } else {  //label to left
    labr = 0.0;
    labl = labw *-1.4;
	if (true){
		labl = -labw;
	}
    
  }
	if ( lab.x - charwidth < 0.0 ){
		//glPopMatrix();
		//glMatrixMode(GL_MODELVIEW);
		//glPopMatrix();	  
      return;                   // Entirely off the left side of the viewport.
	}

	if (lab.x+labl < 0.0) {
      // GL won't display string if beginning is to left of window border:
      // truncate the requisite number of chars from the front
      int truncamt = (int) ((lab.x-labw)/charwidth);
      /* fprintf(stderr,"drawLabel str=%s, len=%d, truncamt=%d, labl=%f, lab.x=%f\n", 
         str, len, truncamt, labl, lab.x); /**/

      // Beware of hugely negative lab.x due to busted transform.
      if ( truncamt < 0 && 
		  ( abs(truncamt) < len ) && 
		   truncamt != 0x80000000 ) {
        //fprintf(stderr,"  str=%s\n", str);
        //const char * ostr = str;
        str += -truncamt;
        labl = -lab.x;
        //fprintf(stderr, "  str=%s\n", str);
	  }
	  else{
		  return;
	  }
    }
  if (highlight==3){lab.y+=12;}

  //glPolygonMode(GL_FRONT, GL_FILL);
  glPushMatrix();
  glLoadIdentity();
  glPolygonMode(GL_FRONT, GL_FILL);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vp[2] - vp[0], 0, vp[3] - vp[1], -1.0, 1.0);
  
  
  
  
  if (highlight==0){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],0.90);}
  if (highlight==1){glColor4f(hd->colorLabel[0], hd->colorLabel[1], hd->colorLabel[2],0.90);}
  if (highlight==2){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],0.90);}
  if (highlight==3){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],0.90);}
  if (highlight==4){glColor4f(hd->blueNode[0],hd->blueNode[1],hd->blueNode[2],0.90);}
  
  
  
  if(false){
	  glBegin(GL_TRIANGLE_STRIP);	  
			glVertex3d(lab.x+labl-4.0,		lab.y-labdescent,	lab.z *1.01);
			glVertex3d(lab.x+labr+4.0,		lab.y-labdescent,	lab.z *1.01);
			glVertex3d(lab.x+labl-4.0,		lab.y+labascent,	lab.z *1.01);
			glVertex3d(lab.x+labr+4.0,		lab.y+labascent,	lab.z *1.01);
	  glEnd();
  }

  if(true){
	  glBegin(GL_TRIANGLE_STRIP);	  
			glVertex3d(lab.x+labl,		lab.y-labdescent,	lab.z );
			glVertex3d(lab.x+labr,		lab.y-labdescent,	lab.z );
			glVertex3d(lab.x+labl,		lab.y+labascent,	lab.z );
			glVertex3d(lab.x+labr,		lab.y+labascent,	lab.z );
	  glEnd();
  }

  if (highlight==0){glColor4f(hd->colorLabel[0], hd->colorLabel[1], hd->colorLabel[2],1.00);}
  if (highlight==1){glColor4f(hd->colorBack[0], hd->colorBack[1], hd->colorBack[2],1.0);}
  if (highlight==2){glColor4f(hd->colorLinkFrom[0],hd->colorLinkFrom[1],hd->colorLinkFrom[2],1.0);}
  if (highlight==3){glColor4f(hd->colorLinkTo[0],hd->colorLinkTo[1],hd->colorLinkTo[2],1.0);}
  if (highlight==4){glColor4f(hd->colorLabel[0], hd->colorLabel[1], hd->colorLabel[2],1.00);}

  glRasterPos3d(lab.x+labl, lab.y, lab.z+.001);
  //if (highlight==3){glRasterPos3d(lab.x+labl, lab.y, lab.z+.001);}
  gl2psText(str, "Arial", 14);//debug/sjk2011
	
  
	drawString(str);
  

  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  return;


}

//----------------------------------------------------------------------------
//                  void HypViewer::drawLabel(HypNode *n)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawLabel(HypNode *n)
{
  
  HypPoint lab = n->getLabelPos();
  int highlight=0;
  if (n->getTitleFlag()){highlight=4;}
  if (n->getHighlighted()){highlight=1;}
  //drawLabel(lab,n->getOctLabel(hd->labels).c_str(),n->getHighlighted());
  drawLabel(lab,n->getOctLabel(hd->labels).c_str(),highlight);
  return;
}
//----------------------------------------------------------------------------
//                  void HypViewer::drawLabel(HypLink *l)                  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawLabel(HypLink *l)
{
	HypNode *p=l->getParent();
	HypNode *n=l->getChild();
	HypPoint labn = n->getLabelPos();
	HypPoint labp = p->getLabelPos();
	HypPoint labl;
	labl.x=(labn.x+labp.x)*0.5;
	labl.y=(labn.y+labp.y)*0.5;
	labl.z=(labn.z+labp.z)*0.5;
	labl.w=1.0;
	if (n->get_XA().size()>3){drawLabel(labl,n->get_XA()[2].c_str(),2);}
	if (n->get_XA().size()>3){drawLabel(labl,n->get_XA()[3].c_str(),3);}
  return;
}




//----------------------------------------------------------------------------
//                       void HypViewer::drawSphere()                      
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::drawSphere()
{
	glEnable(GL_LINE_SMOOTH);//SJK2010
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);//SJK2011
	glLineWidth(1.0);//sjk2010
	glCallList(HV_INFSPHERE);
  return;
}

//----------------------------------------------------------------------------
//                        void HypViewer::camSetup()                       
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::camSetup()
{
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  if (pick) {
	double xd = pickx;
	double yd = picky;
    //    fprintf(stderr, "pickplace %g %g\n", xd, (GLdouble)(vp[3]-yd));
    //    glGetIntegerv ( GL_VIEWPORT, vp );      /* get viewport size */ 
	gluPickMatrix( xd, (GLdouble)(vp[3]-yd), 3.0, 3.0, vp);
  }
  glMultMatrixd((double*)cx);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixd((double*)vx);  
  HypTransform V, S;
  V = viewxform.concat(camxform); 
  // V maps world to [-1..1],[-1..1],[-1..1] */
  S.translateEuc(1., 1., 0.);
  V = V.concat(S); // now maps to [0..2],[0..2],[-1..1]
  S.scale(.5*(vp[2]-vp[0]+1), .5*(vp[3]-vp[1]+1), 1.);
  // now maps to [0..xsize],[0..ysize],[-1..1]
  W2S = V.concat(S); // final world-to-screen matrix
  return;
}


//----------------------------------------------------------------------------
//                       void HypViewer::frameBegin()                      
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::frameBegin(HypData *d)
{
  
  glDrawBuffer(GL_BACK);
  //glDrawBuffer(GL_FRONT);
  glDepthFunc(GL_LESS);
//  glDepthFunc(GL_ALWAYS);  //sjk2011// makes the cubes transparent-ish
  glClear((GLbitfield) (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  //glClear((GLbitfield) (GL_COLOR_BUFFER_BIT));
  
  camSetup();
  //glMultMatrixd((double*)worldxform.T);
  if (!pick && hd->bSphere){
    drawSphere();
  }
  
  //drawStatus(1,"xyzzy",1);// breaks it
  glMultMatrixd((double*)worldxform.T); // does the transform
  
  
  return;
}

//----------------------------------------------------------------------------
//                     void HypViewer::frameContinue()                     
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::frameContinue(HypData *d)
{
  glDrawBuffer(GL_FRONT);
  glDepthFunc(GL_LESS);
  camSetup();
  glMultMatrixd((double*)worldxform.T);
  
  return;
}

//----------------------------------------------------------------------------
//                          void HypViewer::idle()                         
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::idle()
{
  idleframe = 1;
  drawFrame();
  idleframe = 0;
  idleCont();
  return;
}


#ifdef WIN32
   //--------------------------------------------------------------------------
  //                       int HypViewer::idleCB(void)
  //..........................................................................
  //
  //--------------------------------------------------------------------------
  int HypViewer::idleCB(void)
  {
    thehv->idle();
    return idleid;
  }


#endif //WIN32


//----------------------------------------------------------------------------
//    void HypViewer::hiliteFuncCB(int x, int y, int shift, int control)   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::hiliteFuncCB(int x, int y, int shift, int control)
{
  thehv->hiliteFunc(x,y,shift,control);
  return;
}

//----------------------------------------------------------------------------
//     void HypViewer::pickFuncCB(int x, int y, int shift, int control)    
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::pickFuncCB(int x, int y, int shift, int control)
{
  thehv->pickFunc(x,y,shift,control);
  return;
}

//----------------------------------------------------------------------------
//    void HypViewer::transFuncCB(int x, int y, int shift, int control)    
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::transFuncCB(int x, int y, int shift, int control)
{ 
  thehv->transFunc(x,y,shift,control);
  return;
}

//----------------------------------------------------------------------------
//     void HypViewer::rotFuncCB(int x, int y, int shift, int control)     
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::rotFuncCB(int x, int y, int shift, int control)
{
  thehv->rotFunc(x,y,shift,control);
  return;
}


//----------------------------------------------------------------------------
//     void HypViewer::spinFuncCB(int x, int y, int shift, int control)     
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::spinFuncCB(int x, int y, int shift, int control)
{
  thehv->spinFunc(x,y,shift,control);
  return;
}

//----------------------------------------------------------------------------
//                void HypViewer::bindCallback(int b, int c)               
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::bindCallback(int b, int c)
{
  void (*fp)(int,int,int,int);

  switch (c) {
    case HV_PICK:        fp = pickFuncCB;      break;
    case HV_HILITE:      fp = hiliteFuncCB;    break;
    case HV_TRANS:       fp = transFuncCB;     break;
    case HV_ROT:         fp = rotFuncCB;       break;
    case HV_SPIN:        fp = spinFuncCB;      break;
    default:                                   break;
  }

  switch (b) {
    case HV_LEFT_CLICK:      leftClickCB   = fp;    break;
    case HV_MIDDLE_CLICK:    middleClickCB = fp;    break;
    case HV_RIGHT_CLICK:     rightClickCB  = fp;    break;
    case HV_LEFT_DRAG:       leftDragCB    = fp;    break;
    case HV_MIDDLE_DRAG:     middleDragCB  = fp;    break;
    case HV_RIGHT_DRAG:      rightDragCB   = fp;    break;
    case HV_PASSIVE:         passiveCB     = fp;    break;
    default:                                        break;
  }
  
  return;
}

//----------------------------------------------------------------------------
//   void HypViewer::setPickCallback(void (*fp)(const string &,int,int))   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setPickCallback(void (*fp)(const string &,int,int))
{
  pickCallback = fp;
  return;
}

//----------------------------------------------------------------------------
//  void HypViewer::setHiliteCallback(void (*fp)(const string &,int,int))  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setHiliteCallback(void (*fp)(const string &,int,int))
{
  hiliteCallback = fp;
  return;
}

//----------------------------------------------------------------------------
//  void HypViewer::setFrameEndCallback(void (*fp)(int))  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setFrameEndCallback(void (*fp)(int))
{
  frameEndCallback = fp;
  return;
}

//----------------------------------------------------------------------------
//  int HypViewer::getHiliteNode()  
//............................................................................
//  
//----------------------------------------------------------------------------
int HypViewer::getHiliteNode()
{
  
  return hiliteNode;
}



//----------------------------------------------------------------------------
//         void HypViewer::mouse(int btn, int state, int x, int y,         
//                               int shift, int control)                   
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::mouse(int btn, int state, int x, int y, 
		      int shift, int control)
{
  button = btn;
  buttonstate = state;

  switch (button) {
    case 0:
      if (leftClickCB)
        (*leftClickCB)(x,y,shift,control);
      break;
    case 1:
      if (middleClickCB)
        (*middleClickCB)(x,y,shift,control);
      break;
    case 2:
      if (rightClickCB)
        (*rightClickCB)(x,y,shift,control);
      break;
    default:
      break;
  }
  
  oldx = x; oldy = y;
  
  return;
}

//----------------------------------------------------------------------------
//       void HypViewer::motion(int x, int y, int shift, int control)      
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::motion(int x, int y, int shift, int control)
{
  if (motionCount < hd->motionCull) {
    motionCount++;
    return;
  }
  else {
    motionCount = 0;
  }

  switch (button) {
    case 0:
      if (leftDragCB)
        (*leftDragCB)(x,y,shift,control);
      break;
      case 1:
        if (middleDragCB)
          (*middleDragCB)(x,y,shift,control);
        break;
    case 2:
      if (rightDragCB)
        (*rightDragCB)(x,y,shift,control);
      break;
    default:
      break;
  }
  
  oldx = x; oldy = y;
  
  return;
}

//----------------------------------------------------------------------------
//      void HypViewer::passive(int x, int y, int shift, int control)      
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::passive(int x, int y, int shift, int control)
{
  if (passiveCount < hd->passiveCull) {
    passiveCount++;
    return;
  }
  else {
    passiveCount = 0;
  }
  if (passiveCB)
    (*passiveCB)(x,y,shift,control);
  
  oldx = x; oldy = y;
  
  return;
} 

//----------------------------------------------------------------------------
//     void HypViewer::hiliteFunc(int x, int y, int shift, int control)    
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::hiliteFunc(int x, int y, int shift, int control)
{
  HypNode *n;
  int picked = doPick(x,y);
#if 0 //#ifdef WIN32
  TRACE("hiliteFunc %d\n", picked);
  // Kluge around problems with picking nodes on Windows.
  if (picked < 0 && picked!= -INT_MAX ) picked = -picked;
#endif
  if (picked != hiliteNode) {
    n = hg->getNodeFromIndex(hiliteNode);
    glDrawBuffer(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    camSetup();
    glMultMatrixd(wx);
	if (n) {
      n->setHighlighted(0);    
	  n->setDrawn(false);
      drawNode(n);
	  if (DrawQ.size()<20){ DrawQ.push_back(n);}
 	  else {
	  	  DrawQ.push_back(DrawQ[10]);
		  DrawQ[10]=n;
	  }
  	  redraw();
	
	
	 //if (n->getParentLink()){drawLink(n->getParentLink());redraw();}
	  if (n->getParentLink()){drawLink(n->getParentLink());}
	}
    hiliteNode = picked;
    n = hg->getNodeFromIndex(hiliteNode);
	if (n) {
      n->setHighlighted(1);
	n->setDrawn(false);
      drawNode(n);
	DrawQ.push_back(n);
	if (DrawQ.size()<20){ DrawQ.push_back(n);}
	else {
		DrawQ.push_back(DrawQ[10]);
		DrawQ[10]=n;
	}
	  redraw();
      //if (n->getParentLink()){drawLink(n->getParentLink());redraw();}
	  if (n->getParentLink()){drawLink(n->getParentLink());}
	} //if (n)
    string foo;
    if (n) {
      foo = n->getId();
      if (foo.length() > 0) {
        if (hiliteCallback) {
          (*hiliteCallback)(foo.c_str(),shift,control);
        }
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//      void HypViewer::pickFunc(int x, int y, int shift, int control)     
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::pickFunc(int x, int y, int shift, int control)
{
  if (buttonstate == 0) {
    mousemoved = 0;
    idleFunc(0);
    clickx = x;
    clicky = y;
  }
  else{
    double dx = (x-clickx);
    double dy = (y-clicky);
    // window dang well better be > (0,0)! 
    double sdx = (1.0*dx)/(1.0+hd->winx);
    double sdy = (1.0*dy)/(1.0+hd->winy);
    //    fprintf(stderr, "total trans %g %g\n", sdx, sdy);
    if (sdx <= .01 && sdx >= -.01 && sdy <= .01 && sdy >= -.01) {
    
      int picked = doPick(x,y);
#if 0 //#def WIN32
      // Kluge around problems with picking nodes on Windows.
      if (picked < 0 && picked!= -INT_MAX ) picked = -picked;
#endif
      if (picked >= 0) {
	// got a node
		  selectedNode=picked;
	      HypNode *n = hg->getNodeFromIndex(picked);
		  if (n){
			flashLink(n,n);// reset the selected link
		  }
        #ifdef HYPGLX
          SetWatchCursor(wid);
        #endif
	if (pickCallback)
          (*pickCallback)(n->getId().c_str(), shift, control);
        #ifdef HYPGLX
          SetDefaultCursor(wid);
        #endif
	idleFunc(1);
      } 
	else if (picked == (-INT_MAX)) {
	// got the background
		if (pickCallback)
			  (*pickCallback)("", shift, control);
		} 
	else if (picked < 0) {
//	else if (false) {//sjktest
	// got an edge
    //TODO EDGE Highliting
			HypLink *l = hg->getLinkFromIndex(-picked-1);
			HypNode *n = l->getChild();//sjktest 2011
			//selectedNode= l->getParent()->getIndex();
			if (l){//do we have a real link
				selectedNode= l->getChild()->getIndex();
				flashLink(l->getParent(),l->getChild());
				//clearHiliteNode();
				//redraw();
			
				if (pickCallback){
					//(*pickCallback)(l->getId().c_str(), shift, control);
					// THIS IS AN EVIL CLUDGE
					// THIS PREVENTS GRAPH OVERSHOOT
					// IT SHOULD BE DONE BETTER, BUT THIS WORKS WELL ENOUGH
					(*pickCallback)(n->getId().c_str(), shift, control);
					
				}
			}
	  }
    }
  }

  #ifdef HYPXT
    XtSetKeyboardFocus(this->wid,this->wid);
  #endif

  return;
}

//----------------------------------------------------------------------------
//             void HypViewer::setSelected(HypNode *n, bool on)
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setSelected(HypNode *n, bool on)
{
  n->setSelected(on);    
  if (!n->getEnabled())
    return;
  if (!n->getDrawn())
    return;
  glDrawBuffer(GL_FRONT);
  glDepthFunc(GL_LEQUAL);
  camSetup();
  glMultMatrixd(wx);
  drawNode(n);
  
  insertSorted(&DrawnQ, n);
  return;
}

//----------------------------------------------------------------------------
//  void HypViewer::rotFunc(int x, int y, int /*shift*/, int /*control*/)  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::rotFunc(int x, int y, int /*shift*/, int /*control*/)
{
  if (buttonstate == 0) {
    idleFunc(0);
    static float Radius = 100.0;
    double dx = (x-oldx);
    double dy = (y-oldy);
    double dr = sqrt(dx*dx + dy*dy);
    double denom = sqrt(Radius*Radius + dr*dr);
    double theta = acos(Radius/denom);
    //    fprintf(stderr, "rot  %g ", theta);
    if (theta > .0001) {
      HypPoint p;
      p.x = dy/dr;   
      p.y = dx/dr;
      p.z = 0.0;
      p.w = 1.0;
      HypTransform T;
      T.rotate(theta, p);
      worldxform = worldxform.concat(T);
      mousemoved = 1;
      redraw();
    }
  } else {
  }
  //    redraw();
    idleFunc(1);
}
//----------------------------------------------------------------------------
//  void HypViewer::spinFunc(int x, int y, int /*shift*/, int /*control*/)  
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::spinFunc(int x, int y, int /*shift*/, int /*control*/)
{
  if (buttonstate == 0) {
    idleFunc(0);
    static float Radius = 100.0;
    double dx = (x-oldx);
    double dy = (y-oldy);
	double sdx = (2.0*dx)/hd->winx;
	double sdy = (2.0*dy)/hd->winy;
    double dr = sqrt(dx*dx + dy*dy);
    double denom = sqrt(Radius*Radius + dr*dr);
    double theta = acos(Radius/denom);
    //    fprintf(stderr, "rot  %g ", theta);
    if (theta > .0001) {
      HypPoint p;
      p.x =0.0;   
      p.y = 0.0;
      p.z = dx/dr;
      //p.w = 1.0;
	  p.w = 1.0;
      HypTransform T;
	  if (abs(dx)>abs(dy)){
		  T.rotate(theta, p);
		  worldxform = worldxform.concat(T);
	  }else{
		  T.translateHyp(0.0, 0.0, -sdy);
		  worldxform = worldxform.concat(T);
	  }
      mousemoved = 1;
      redraw();
    }
  } else {
  }
  //    redraw();
    idleFunc(1);
}


//----------------------------------------------------------------------------
// void HypViewer::transFunc(int x, int y, int /*shift*/, int /*control*/) 
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::transFunc(int x, int y, int /*shift*/, int /*control*/)
{
  if (buttonstate == 0) {
    idleFunc(0);
    double dx = (x-oldx);
    double dy = (y-oldy);
    // window dang well better be > (0,0)! 
    double sdx = (2.0*dx)/hd->winx;
    double sdy = (2.0*dy)/hd->winy;
    //    fprintf(stderr, "trans %g %g\n", sdx, sdy);
    if ( sdx > .0001 || sdx < -.0001 || sdy > .0001 || sdy < -.0001) {
      HypTransform T;
      T.translateHyp(sdx, -sdy, 0.0);
      worldxform = worldxform.concat(T);
      mousemoved = 1;
      redraw();
    }
  }
  //    redraw();
  idleFunc(1);
  return;
}


//----------------------------------------------------------------------------
//                   int HypViewer::doPick(int x, int y)                   
//............................................................................
//  
//----------------------------------------------------------------------------
int HypViewer::doPick(int x, int y)
{
  int picked = -INT_MAX;
  pickx = x; picky = y;
  pick = 1;
  GLuint hitbuf[5096];
  /* Set up picking: hit buffer, name stack. */ 
//  fprintf(stderr, "PICKSTART\n");
  glSelectBuffer ( 5096, hitbuf ); 
  glRenderMode ( GL_SELECT ); 
  glInitNames(); 
  glPushName(6555);
  drawPickFrame();
  glFlush();
  GLint hits = glRenderMode(GL_RENDER);
  if (hits > 0) {
    picked = hitbuf[3];
    GLuint zint = hitbuf[1];
    GLdouble xw, yw, zw;
    xw = (double) x;
    yw = (double) y;
    zw = (double) zint*pickrange;
    gluUnProject(xw, yw, zw, 
		 (double*)vx, // modelview
		 (double*)cx, // projection
		 vp, // viewport
		 &(pickpoint.x), &(pickpoint.y), &(pickpoint.z));
    pickpoint.y = -pickpoint.y;
    if (pickpoint.z > .9999999f || pickpoint.z < -.9999999f) pickpoint.z = .001;
    pickpoint.w = 1.0;
  }
  pick = 0;
  
  return picked;
}

//----------------------------------------------------------------------------
//                int HypViewer::gotoPickPoint(int animate)                
//............................................................................
//  
//----------------------------------------------------------------------------
int HypViewer::gotoPickPoint(int animate)
{
  #ifdef HYPGLX
    SetWatchCursor(wid);
  #endif
  double hypdist = origin.distanceHyp(pickpoint);
  
  //if ((animate == HV_ANIMATE)&& (hypdist < 1234)) {//sjk2011 hmmm trying to fix the overshoot here
  if (animate == HV_ANIMATE){
    //double hypdist = origin.distanceHyp(pickpoint);
    HypTransform U2W;
    U2W.copy(worldxform);
    doAnimation(pickpoint, iq, hypdist, 0.0, U2W);
  } else {
    HypTransform T;
    T.translateOriginHyp(pickpoint);
    T.invert();
    worldxform = worldxform.concat(T);
    redraw();
  }

  #ifdef HYPGLX
    SetDefaultCursor(wid);
  #endif
    
  return 1;
}

//----------------------------------------------------------------------------
//             int HypViewer::gotoNode(HypNode *n, int animate)            
//............................................................................
//  
//----------------------------------------------------------------------------
int HypViewer::gotoNode(HypNode *n, int animate)
{
  if (!n){
    return 0;
  }
  selectedNode=n->getIndex();
  
  HypPoint v,p,q,rr;
  HypTransform U2W, W2N, T, TI, N2WT, W2NT, U2NT, N2UT, H, R, ID;
  HypQuat rq, sq;
  double ds;

  #ifdef HYPGLX
    SetWatchCursor(wid);
  #endif
    
  W2N = n->getC();
  U2W.copy(worldxform);
  H = W2N.concat(U2W);
  v = H.transformPoint(origin);
  v.normalizeEuc();
  double len = v.length();
  if (len > 1) {
    fprintf(stderr, "Floating point error:  x %g y %g z %g w %g\n", 
	    v.x, v.y, v.z, v.w); 
    v.mult(1./len);
    v.x -= .01; v.y -= .01; v.z -= .01;
    fprintf(stderr, "Normalizing to:  x %g y %g z %g w %g\n", 
	    v.x, v.y, v.z, v.w); 
  }
  N2WT.translateOriginHyp(v);
  W2NT.copy(N2WT);
  W2NT.invert();
  U2NT = U2W.concat(W2NT);
  N2UT.copy(U2NT);
  N2UT.invert();
  ID.identity();
  if (W2N.T != ID.T) {
    // rotate
    // W2N contains both trans & rot, U2NT is just trans
    H = W2N.concat(U2NT);
    q = H.transformPoint(origin);
    T.translateOriginHyp(q);
    // T has just translational component from U to N
    TI.copy(T);
    TI.invert();
    H = H.concat(T);
    // H has just the rotational component of the cumulative xform
    p = H.transformPoint(xaxis);
    p.normalizeEuc();
    q = H.transformPoint(origin);
    q.normalizeEuc();
    rr = q.sub(p);
    R.rotateBetween(rr,xaxis);
    rq.fromTransform(R);
    ds = rq.fixSign();
  }
  if (animate == HV_JUMP) {
    worldxform = U2NT.concat(R);
    redraw();
    return 1;
  }
  else {
    if (animate == HV_ANIMATE) {
      double hypdist = origin.distanceHyp(v);
      doAnimation(v, rq, hypdist, ds, U2W);
	  redraw();
    }
    #ifdef HYPGLX
      SetDefaultCursor(wid);
    #endif
  }
  
  return 1;
}

//----------------------------------------------------------------------------
//           void HypViewer::doAnimation(HypPoint v, HypQuat rq,           
//                                       double transdist, double rotdist,  
//                                       HypTransform U2W)                 
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::doAnimation(HypPoint v, HypQuat rq, 
			    double transdist, double rotdist, 
			    HypTransform U2W)
{
 //if (transdist>.9999) return;//sjk2011 test fails to stop graph-destruction
  HypPoint p;
  float incr = 1.0;
  HypTransform TI, S;
  HypQuat sq;
  float k;

  if ((transdist > .0001) || (rotdist > .0001)) {
    hd->tossEvents = 1;
    for (k = hd->gotoStepSize; k <= 1.01; k+= hd->gotoStepSize) { 
      if (transdist > .0001) 
	incr = tanh(transdist*k)/tanh(transdist);
      p.x = incr*v.x; 
      p.y = incr*v.y; 
      p.z = incr*v.z; 
      p.w = 1;
      if (rotdist > .0001) {
	sq = rq.slerp(iq, k);
	S = sq.toTransform();
      }
      TI.translateOriginHyp(p);
      TI.invert();
      worldxform = U2W.concat(TI);
      if (rotdist > .0001) {
	worldxform = worldxform.concat(S);
      }
      drawFrame();
    }
  }
  return;
}

//----------------------------------------------------------------------------
//                void HypViewer::setGraphCenter(int cindex)               
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setGraphCenter(int cindex)
{
  HypNode *c;
  worldxform.copy(identxform);
  if (cindex < 0) {
    c = hg->getNodeFromIndex(hd->centerindex);
  } else {
    c = hg->getNodeFromIndex(cindex);
    HypTransform t = c->getC();
    worldxform = worldxform.concat(t);
  }
  
  return;
}

//----------------------------------------------------------------------------
//            void HypViewer::setGraphPosition(HypTransform pos)           
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setGraphPosition(HypTransform pos)
{
  worldxform.copy(identxform);
  worldxform = worldxform.concat(pos);
  return;
}

//----------------------------------------------------------------------------
//                 void HypViewer::setLabelToRight(bool on)                 
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::setLabelToRight(bool on)
{
  labeltoright = on;
}

//----------------------------------------------------------------------------
//                     void HypViewer::resetLabelSize()                    
//............................................................................
//  
//----------------------------------------------------------------------------
void HypViewer::resetLabelSize()
{
  // labelsize is measured in x direction
  float scaledvp;
  float aspectratio = vp[2]/((float)vp[3]);
  if (hd->keepAspect && aspectratio > 1.0) {
    scaledvp = vp[2]/aspectratio;
  } else {
    scaledvp = vp[2];
  }
  labelscaledsize = hd->labelsize*(scaledvp/1000.0);
  //printf("resetLabelSize vp=[%d %d %d %d], labelscaledsize=%g\n", vp[0], vp[1], vp[2], vp[3], labelscaledsize );

}


//----------------------------------------------------------------------------
//       int HypViewer::flashLink(HypNode *fromnode, HypNode *tonode)      
//............................................................................
//  
//----------------------------------------------------------------------------
int HypViewer::flashLink(HypNode *fromnode, HypNode *tonode)
{
	if (fromnode==NULL){return -1;}
	if (tonode==NULL){return -1;}
  string linkid = fromnode->getId() + "|" + tonode->getId(); 
  HypLink *link = hg->getLinkFromId(linkid);
  string linkid2 = tonode->getId() + "|" + fromnode->getId(); 
  HypLink *link2 = hg->getLinkFromId(linkid);
  if (flashingLink!=NULL){ //Clear previous colors
	  hg->resetColorLink(flashingLink);
	  //hg->resetColorLink(link);
	 //flashingLink->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
	//	 hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
       //flashingLink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
	//	 hd->colorLinkTo[2], hd->colorLinkTo[3]);
	 //if (1==tonode->getDndsLevel()){
	//	flashingLink->setColorFrom(hd->dndsColorLinkFrom[0], hd->dndsColorLinkFrom[1], 
	//		 hd->dndsColorLinkFrom[2], hd->dndsColorLinkFrom[3]);
	//	flashingLink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
	//		hd->dndsColorLinkTo[2], hd->dndsColorLinkTo[3]);
//	 }

  
	for (size_t i=0;i<flashingPathLinks.size();i++){// Redraw pathed colors
		if (flashingLink==flashingPathLinks[i]){
			flashingLink->setColorFrom(.5,.5,.5,1);
			flashingLink->setColorTo(1,.6,0,1);
			break;

		}
	}
	flashingLink=link;
  }
  if (link) {
    link->setColorFrom(.5,.5,.5,1);
	link->setColorTo(.2,1,0,1);
	flashingLink=link;
    return 1;
  } else return 0;


/*
  HypLink *link = NULL;
  link = new HypLink(fromnode->getId().c_str(), tonode->getId().c_str());
  link->setEnabled(1);
  link->setParent(fromnode);
  link->setChild(tonode);
  link->layout();
  link->setDrawn(0);// sjk2011
  //float *flashcol = hd->colorSelect;
  //link->setColorFrom(flashcol[0], flashcol[1], flashcol[2], flashcol[3]);
  //link->setColorTo(flashcol[0], flashcol[1], flashcol[2], flashcol[3]);
  link->setColorFrom(.5,.5,.5,.5);
  link->setColorTo(.5,.5,.5,.5);
  drawLink(link);
  return 1;
  */
}
  int HypViewer::flashPathLink(HypNode *fromnode, HypNode *tonode)
{
  if (! (fromnode && tonode) ){exit(6);}
  string linkid = fromnode->getId() + "|" + tonode->getId(); 
  HypLink *link = hg->getLinkFromId(linkid);
  if (link==flashingLink){ 
	  flashingPathLinks.push_back(link);
	  return 0;} // skip the flash link
  if (link) {
    link->setColorFrom(.8,.8,.5,1);
	link->setColorTo(1,.6,0,1);
	flashingPathLinks.push_back(link);
	
    return 1;
  } else return 0;
  }
  void HypViewer::resetPath(){
	  //this if for a reset..
	  flashingPathLinks.resize(0);
  }
  int HypViewer::clearPath(){
	  if (hd==NULL) {return -1;}
	  for (size_t i=0;i<flashingPathLinks.size();i++){
			HypLink * currentLink = flashingPathLinks[i];
			if (!currentLink) {continue;}
			if (currentLink==flashingLink) {continue;}// skip the current link
			hg->resetColorLink(currentLink);
			//currentLink->setColorFrom(hd->colorLinkFrom[0], hd->colorLinkFrom[1], 
			//						hd->colorLinkFrom[2], hd->colorLinkFrom[3]);
			//currentLink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
			//						hd->colorLinkTo[2], hd->colorLinkTo[3]);
			//if (1==currentLink->getChild()->getDndsLevel()){
			//	currentLink->setColorFrom(hd->dndsColorLinkFrom[0], hd->dndsColorLinkFrom[1], 
			//		 hd->dndsColorLinkFrom[2], hd->dndsColorLinkFrom[3]);
			//	currentLink->setColorTo(hd->colorLinkTo[0], hd->colorLinkTo[1], 
			//		hd->dndsColorLinkTo[2], hd->dndsColorLinkTo[3]);
					//}
	  }
	return(0);
  }
  int HypViewer::flashPath(HypNode *leftnode, HypNode *rightnode){
	  //flashPathLink(rightnode,leftnode);
	  if (leftnode==NULL){return -1;}
	  if (rightnode==NULL){return -1;}
	  if ((!leftnode)||(!rightnode)){clearPath();return (-1);}
	  vector <HypNode *> left;
	  vector <HypNode *> right;

		
	  HypNode * current=leftnode;
	
	  while (current->getParent()){
		  //flashPathLink(current->getParent(),current);
		  left.push_back(current);
		  current= current->getParent();
		  if (left.size()>5000){exit(5);}

	  }
	  
	  current=rightnode;
	  while (current ->getParent()){
		  right.push_back(current);
		  current=current ->getParent();
		  if (right.size()>5000){exit(5);}
	  }
	  //  we now have 2 lists going to the root node, the 
	  //  tailing entries will be identical
	  
	  // clean the former links..
	  clearPath();
      // clean the left and right list of duplicates
	  
	  while (
			 (
				(left.size()>0) && 
				(right.size()>0)
		      )
			  &&
				( 
				  left.back()==right.back()
				)
			 )
	  {
		  current=left.back();
		  left.pop_back();
		  right.pop_back();
	  }// While (((>)&&(>))&&(==))
	  
		left.push_back(current); // we want the common
		right.push_back(current);//element where the join occurs

		
		
		
		for (size_t i=0;i<left.size()-1;i++){
			flashPathLink(left[i+1],left[i]);
			if (hd->landMarkPath){
				left[i]->setTitleFlag(true);
				left[i+1]->setTitleFlag(true);
			}
		}
    	for (size_t i=0;i<right.size()-1;i++){
			flashPathLink(right[i+1],right[i]);
			if (hd->landMarkPath){
				right[i]->setTitleFlag(true);
				right[i+1]->setTitleFlag(true);
			}
		}
       

      return(0);





  }

// window system specific code

//  glx specific code



//  glut specific code


// window system specific code
//  WIN32 specific code
#ifdef WIN32

void HypViewer::drawStringInit() {
	  //glDepthFunc(GL_ALWAYS);//sjk2011  causes issues
	  //  the draw order makes the system look random
	 
     LOGFONT     lf;
     HFONT       hFont,hOldFont;

     if (!hd->labelfont) return;

     // An hDC and an hRC have already been created.
     wglMakeCurrent( wid, cxt );

     // Let's create a TrueType font to display.
     memset(&lf,0,sizeof(LOGFONT));
     lf.lfHeight               =   hd->labelfontsize;
     lf.lfWeight               =   FW_NORMAL ;
     lf.lfCharSet              =   ANSI_CHARSET ;
     lf.lfOutPrecision         =   OUT_TT_ONLY_PRECIS;
     lf.lfClipPrecision        =   CLIP_DEFAULT_PRECIS ;
     lf.lfQuality              =   PROOF_QUALITY ;
     lf.lfPitchAndFamily       =   VARIABLE_PITCH | TMPF_TRUETYPE;
     lstrcpy (lf.lfFaceName, hd->labelfont) ;//   hd->labelfont
     hFont = CreateFontIndirect(&lf);
     hOldFont = (HFONT) SelectObject(wid,hFont);

     labascent = 13;
     labdescent = 0;

     TEXTMETRIC *tm;
     tm = new TEXTMETRIC;
     bool r = GetTextMetrics(wid,tm);

     labascent = tm->tmAscent;
     labdescent = tm->tmDescent;
     
     delete tm;

     memset(&iCharWidths,0,sizeof(iCharWidths));
     r = GetCharWidth(wid,0,255,iCharWidths);

     if (!labelbase)  labelbase = glGenLists(256);
     if (labelbase) {
         // Create a set of display lists based on the TT font we selected
          wglUseFontBitmaps(wid,0,256,labelbase);
     }

     DeleteObject(SelectObject(wid,hOldFont));

     return;

  }

  //--------------------------------------------------------------------------
  //               void HypViewer::drawString(const string & s)              
  //..........................................................................
  //  
  //--------------------------------------------------------------------------
  void HypViewer::drawString(const string & s)
  {
    //  int len = strlen(s);
	//glDepthFunc(GL_ALWAYS);
    glListBase(labelbase);
    glCallLists(s.length(), GL_UNSIGNED_BYTE, s.c_str());
	//glDepthFunc(GL_LESS);
    return;
  }

  //--------------------------------------------------------------------------
  //             int HypViewer::drawStringWidth(const string & s)
  //..........................................................................
  //
  //--------------------------------------------------------------------------
  int HypViewer::drawStringWidth(const string & s)
  {
//      SIZE sz;
//      GetTextExtentPoint32(wid, s.c_str(), s.length(), &sz);
//     return sz.cx;
     int iWidth = 0;
     for (int i=0; i<s.length(); i++)
        iWidth += iCharWidths[s[i]];
     return iWidth;
  }

void HypViewer::framePrepare() {
  wglMakeCurrent(wid, cxt);
}

void HypViewer::frameEnd() {
  if (!pick && !idleframe)  SwapBuffers(wid);
  idleCont();
}

  //--------------------------------------------------------------------------
  //                    void HypViewer::idleFunc(bool on)                    
  //..........................................................................
  //  
  //--------------------------------------------------------------------------
  void HypViewer::idleFunc(bool on)
  {
  if (on) {
     idleid = 1;
   } else {
     idleid = 0;
   }
}

void HypViewer::idleCont() {
/*
   if (idleframe == 1) {
     OutputDebugString("idle cont\n");
     InvalidateRect(win,NULL,FALSE);
   } else {
      ValidateRect(win,NULL);
   }
   */
}

void HypViewer::redraw() {
  drawFrame();
}
void HypViewer::toggleDrawShape(){
	if (DRAW_SHAPE>=3){ DRAW_SHAPE=0; return;}
	DRAW_SHAPE++;
}
void HypViewer::setLineWidth(float input){
	if (input<0.1) {input=0.1;}
	if (input>10.0) {input=10.0;}
	lineWidth=input;
	
}

void HypViewer::reshape(int w, int h) {
  wglMakeCurrent(wid,cxt);
  glViewport(0,0,w,h);
  glScissor(0,0,w,h);
  vp[2] = w;
  vp[3] = h;
 // hd->winx = w;//sjk-2010
 // hd->winy = h;//sjk-2010
  
  // I'm not sure why we have to do this twice, but doesn't work just once
  // TMM 1/18/98
 // camInit();
 // drawFrame();
 // camInit();
 // drawFrame();
}
#endif  //WIN32

void HypViewer::nodeScale(){
	
	
	float scale=hd->scaleFactor*hd->scaleFactor;// really small squares.
  

  static float box[6][4][3] = 
  {
    {{-HV_LEAFSIZE,     HV_LEAFSIZE,    -HV_LEAFSIZE},
     { HV_LEAFSIZE,     HV_LEAFSIZE,    -HV_LEAFSIZE},
     { HV_LEAFSIZE,    -HV_LEAFSIZE,    -HV_LEAFSIZE},
     {-HV_LEAFSIZE,    -HV_LEAFSIZE,    -HV_LEAFSIZE}
    }, 
    {{-HV_LEAFSIZE,    -HV_LEAFSIZE,     HV_LEAFSIZE},
     { HV_LEAFSIZE,    -HV_LEAFSIZE,     HV_LEAFSIZE},
     { HV_LEAFSIZE,     HV_LEAFSIZE,     HV_LEAFSIZE},
     {-HV_LEAFSIZE,     HV_LEAFSIZE,     HV_LEAFSIZE}
    },
    {{ HV_LEAFSIZE,     HV_LEAFSIZE,    -HV_LEAFSIZE},
     {-HV_LEAFSIZE,     HV_LEAFSIZE,    -HV_LEAFSIZE},
     {-HV_LEAFSIZE,     HV_LEAFSIZE,     HV_LEAFSIZE},
     { HV_LEAFSIZE,     HV_LEAFSIZE,     HV_LEAFSIZE}
    },
    {{-HV_LEAFSIZE,    -HV_LEAFSIZE,    -HV_LEAFSIZE},
     { HV_LEAFSIZE,    -HV_LEAFSIZE,    -HV_LEAFSIZE},
     { HV_LEAFSIZE,    -HV_LEAFSIZE,     HV_LEAFSIZE},
     {-HV_LEAFSIZE,    -HV_LEAFSIZE,     HV_LEAFSIZE}
    },
    {{-HV_LEAFSIZE,    -HV_LEAFSIZE,    -HV_LEAFSIZE},
     {-HV_LEAFSIZE,    -HV_LEAFSIZE,     HV_LEAFSIZE},
     {-HV_LEAFSIZE,     HV_LEAFSIZE,     HV_LEAFSIZE},
     {-HV_LEAFSIZE,     HV_LEAFSIZE,    -HV_LEAFSIZE}
    },
    {{ HV_LEAFSIZE,    -HV_LEAFSIZE,    -HV_LEAFSIZE},
     { HV_LEAFSIZE,     HV_LEAFSIZE,    -HV_LEAFSIZE},
     { HV_LEAFSIZE,     HV_LEAFSIZE,     HV_LEAFSIZE},
     { HV_LEAFSIZE,    -HV_LEAFSIZE,     HV_LEAFSIZE}
    }
  };
  //static float smallBox[6][4][3];
  //for(size_t i=0;i<6;i++)for(size_t j=0;i<4;j++)for(size_t k=0;k<3;k++){
//	  smallBox[i][j][k]=box[i][j][k]*hd->scaleFactor;
  //}

  

  //static float c[6][3]= {{1.0,0.5,1.0},{1.0,1.0,0.0},{1.0,0.0,1.0},{0.0,1.0,1.0},{0.0,1.0,0.0},{0.0,1.0,1.0}};
  glNewList(HV_CUBECOORDS, GL_COMPILE);
  glBegin(GL_QUADS);   
  for (size_t i = 0; i < 6; i++) {        
    for (size_t j = 0; j < 4; j++)
      glVertex3f(box[i][j][0], box[i][j][1], box[i][j][2]);
  }
  glEnd();
  glEndList();


  //int NODE_SHAPE=HV_CUBECOORDS;
  //NODE_SHAPE=HV_SPHERECOORDS;

  glNewList(HV_CUBEFACES, GL_COMPILE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glCallList(HV_CUBECOORDS);

  glEndList();


  
  glNewList(HV_CUBEEDGES, GL_COMPILE);
  glColor3f(0, 0, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glCallList(HV_CUBECOORDS);
  glEndList();


  glNewList(HV_CUBEALL, GL_COMPILE);
  glCallList(HV_CUBEFACES);
  glCallList(HV_CUBEEDGES);
  glEndList();
////////////////////////////////////////////////////////////////////////////////////////////
//  small square zone
///////////////////////////////////////////////////////////////////////////////////////////
  scale=.35;
  static float smallBox[6][4][3] = 
  {
    {{-HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale}
    }, 
    {{-HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale}
    },
    {{ HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale}
    },
    {{-HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale}
    },
    {{-HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     {-HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale}
    },
    {{ HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale},
     { HV_LEAFSIZE*scale,    -HV_LEAFSIZE*scale,     HV_LEAFSIZE*scale}
    }
  };
  //static float smallBox[6][4][3];
  //for(size_t i=0;i<6;i++)for(size_t j=0;i<4;j++)for(size_t k=0;k<3;k++){
//	  smallBox[i][j][k]=box[i][j][k]*hd->scaleFactor;
  //}

  

  //static float c[6][3]= {{1.0,0.5,1.0},{1.0,1.0,0.0},{1.0,0.0,1.0},{0.0,1.0,1.0},{0.0,1.0,0.0},{0.0,1.0,1.0}};
  glNewList(HV_SMALL_CUBECOORDS, GL_COMPILE);
  glBegin(GL_QUADS);   
  for (size_t i = 0; i < 6; i++) {        
    for (size_t j = 0; j < 4; j++)
      glVertex3f(smallBox[i][j][0], smallBox[i][j][1], smallBox[i][j][2]);
  }
  glEnd();
  glEndList();


  //int NODE_SHAPE=HV_SMALL_CUBECOORDS;
  //NODE_SHAPE=HV_SPHERECOORDS;

  glNewList(HV_SMALL_CUBEFACES, GL_COMPILE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glCallList(HV_SMALL_CUBECOORDS);

  glEndList();


  
  glNewList(HV_SMALL_CUBEEDGES, GL_COMPILE);
  glColor3f(0, 0, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glCallList(HV_SMALL_CUBECOORDS);
  glEndList();


  glNewList(HV_SMALL_CUBEALL, GL_COMPILE);
  glCallList(HV_SMALL_CUBEFACES);
  glCallList(HV_SMALL_CUBEEDGES);
  glEndList();

/////////////////////////////////////////////////////////////////////////////////////////
//  Large and small Cubes Defined, defining spheres
///////////////////////////////////////////////////////////////////////////////////////
glNewList(HV_SPHERECOORDS, GL_COMPILE);
  glBegin(GL_QUADS);   
  //static float c[6][3]= {{1.0,0.5,1.0},{1.0,1.0,0.0},{1.0,0.0,1.0},{0.0,1.0,1.0},{0.0,1.0,0.0},{0.0,1.0,1.0}};
  //sjk2011  --cubes to spheres code
  float sphere [11][11][3][4];
  for (size_t i=0;i<11;i++){
       float theta=      ((i/10.0)*3.14);
       float radius=     sin(theta)*HV_LEAFSIZE;
       float sph_y=      cos(theta)*HV_LEAFSIZE;
	   float theta2=     (((i+1)/10.0)*3.14);
       
	   float radius2=   sin(theta2)*HV_LEAFSIZE;
	   float sph_y2=    cos(theta2)*HV_LEAFSIZE;
	   float sph_y3=    sph_y2;
	   
       for (size_t j=0;j<11;j++){
               float phi= ((j/10.0)*6.28);
               float sph_x=radius*cos(phi);
               float sph_z=radius*sin(phi);
			   
               float sph_x2=radius2*cos(phi);
               float sph_z2=radius2*sin(phi);

	         float phi3= (((1+j)/10.0)*6.28);
               float sph_x3=radius2*cos(phi3);
               float sph_z3=radius2*sin(phi3);

	         float phi4= (((j+1)/10.0)*6.28);
               float sph_x4=radius*cos(phi4);
               float sph_z4=radius*sin(phi4);
			   glVertex3f(sph_x , sph_y , sph_z);
			   glVertex3f(sph_x2 , sph_y2 , sph_z2);
			   glVertex3f(sph_x3 , sph_y2 , sph_z3);
			   glVertex3f(sph_x4 , sph_y , sph_z4);
       }
  }
  glEnd();
  glEndList();

  glNewList(HV_SPHEREFACES, GL_COMPILE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glCallList(HV_SPHERECOORDS);

  glEndList();
  glNewList(HV_SPHEREEDGES, GL_COMPILE);
  glColor3f(0, 0, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glCallList(HV_SPHERECOORDS);
  glEndList();
  glNewList(HV_SPHEREALL, GL_COMPILE);
  glCallList(HV_SPHEREFACES);
  glCallList(HV_SPHEREEDGES);
  glEndList();

}