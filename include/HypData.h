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
#ifndef HYPDATA_H
#define HYPDATA_H

// #include <utmpx.h>

#ifdef WIN32

// disable warning C4786: symbol greater than 255 character,
// okay to ignore
#pragma warning(disable: 4786)

// Get rid of these warnings that come from double literals:
// truncation from const double to float
// conversion from double to float
#pragma warning(disable: 4305 4244)

// And this one: forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4800)

#include <winsock.h>   
#else
#include <sys/time.h>
#endif

#define HV_LEAFSIZE .04
#define HV_MAXGROUPS 16

#define HV_LEFT_CLICK           0
#define HV_MIDDLE_CLICK         1
#define HV_RIGHT_CLICK          2
#define HV_LEFT_DRAG            3
#define HV_MIDDLE_DRAG          4
#define HV_RIGHT_DRAG           5
#define HV_PASSIVE              6

#define HV_HILITE 0
#define HV_PICK   1 
#define HV_TRANS  2
#define HV_ROT    3
#define HV_SPIN    4

#define HV_LABELNONE  0
#define HV_LABELSHORT 1 
#define HV_LABELLONG  2


#define HV_ANIMATE 0
#define HV_JUMP    1

#define HV_CENTERLARGEST  1
#define HV_CENTEREXTERNAL 2
#define HV_CENTER 4

#define HV_SPANKEEP   1
#define HV_SPANHIER   2
#define HV_SPANFOREST 4
#define HV_SPANLEX    8
#define HV_SPANNUM    16
#define HV_SPANBFS    32

// GL display lists - must start at 1, not 0
#define HV_INFSPHERE	1
#define HV_CUBECOORDS	2
#define HV_CUBEFACES	3
#define HV_CUBEEDGES	4
#define HV_CUBEALL	5

#define HV_SPHERECOORDS	6
#define HV_SPHEREFACES	7
#define HV_SPHEREEDGES	8
#define HV_SPHEREALL	9

#define HV_SMALL_CUBECOORDS	10
#define HV_SMALL_CUBEFACES	11
#define HV_SMALL_CUBEEDGES	12
#define HV_SMALL_CUBEALL	13

#define HV_SMALL_SPHERECOORDS	14
#define HV_SMALL_SPHEREFACES	15
#define HV_SMALL_SPHEREEDGES	16
#define HV_SMALL_SPHEREALL	17

#define HV_INFSPHERE_FAINT	18
#define HV_INFSPHERE_VERY_FAINT	19
// END GL display lists

#define HV_LINKLOCAL	0
#define HV_LINKINHERIT	1
#define HV_LINKCENTRAL	2

#define HV_SCREEN_LCD	0
#define HV_SCREEN_PDF	1

#define HV_NODESCHEME_LEAF 0
#define HV_NODESCHEME_TEMP 1
#define HV_NODESCHEME_RGB  2
#define HV_NODESCHEME_CLOWN  3


class HypData {

public:
    int loads;
    float nodeAlpha;
     bool leafColorScheme;
     float scaleFactor;
     bool clownColorScheme;//Quasi random coloring based on number of children
     int fogLevel;
	 bool XA_enabled;
     HypData() {
	  loads=0;
	  scaleFactor=1.0;
	  clownColorScheme=false;
	  nodeColorScheme=HV_NODESCHEME_LEAF;
	  landMarkPath=false;
	  pathVisible=false;
	  nodeAlpha=1.0;// opaque
    bCenterLargest = 1;
    bCenterShow = 0;
    bNegativeHide = 0;
    dynamictime.tv_sec = 0;
    dynamictime.tv_usec = 50000;//sjk (FRAMERATE CONTROL)
    idletime.tv_sec = 1;
    idletime.tv_usec = 0;
    picktime.tv_sec = 0;
    picktime.tv_usec = 100000;
    bSphere = 1;
    maxlength = 2.7;
    leafrad = .3;
    areafudge = .4;
    lengthfudge = 0.0; 
	broccoliTops=0.6; // relative length of childless nodes.
    //labels = HV_LABELLONG;
	labels = 7;
    winx = 400;
    winy = 400;
    labelsize = 20; 			 
	screentype = 0;//LCD=0 PDF=1  (most of the time this should be LCD)
    passiveCull = 5;
    motionCull = 5;
	hv_labelOctal = 7; // 0-7  bitwise 000 001 010 011 100 101 110 111
     leafColorScheme=false;
	 XA_enabled=false;

#ifdef WIN32
    labelfont = "Arial";
    labelfontsize = 12;
#else
    labelfont = "-*-courier-medium-r-normal--12-*";
#endif
    colorBack[0] = 1.0;
    colorBack[1] = 1.0;
    colorBack[2] = 1.0;
    colorBack[3] = 1.0;
    colorSphere[0] = .9;
    colorSphere[1] = .7;
    colorSphere[2] = .6;
    colorSphere[3] = 1.0;
    colorHilite[0] = 0.0;
    colorHilite[1] = 1.0;
    colorHilite[2] = 0.0;
    colorHilite[3] = 1.0;
    colorSelect[0] = 1.0;
    colorSelect[1] = 1.0;
    colorSelect[2] = 0.0;
    colorSelect[3] = 1.0;
    colorLabel[0] = 0.0;
    colorLabel[1] = 0.0;
    colorLabel[2] = 0.0;
    colorLabel[3] = 1.0;
    colorLinkFrom[0] = .6;     
    colorLinkFrom[1] = .2;
    colorLinkFrom[2] = .2;
    colorLinkFrom[3] = 1.0;
    colorLinkTo[0] = .2;     
    colorLinkTo[1] = .2;
    colorLinkTo[2] = .6;
    colorLinkTo[3] = 1.0;

    dndsColorLinkFrom[0] = 1.0;     
    dndsColorLinkFrom[1] = .6;
    dndsColorLinkFrom[2] = .6;
    dndsColorLinkFrom[3] = 1.0;
    dndsColorLinkTo[0] = 1.0;     
    dndsColorLinkTo[1] = .2;
    dndsColorLinkTo[2] = .6;
    dndsColorLinkTo[3] = 1.0;

    greenLeaf[0]=0.4;
    greenLeaf[1]=0.7;
    greenLeaf[2]=0.4;
    greenLeaf[3]=.1;

    greenLight[0]=.7;
    greenLight[1]=1.0;
    greenLight[2]=.1;
    greenLight[3]=.2;


    blueNode[0]=.4;
    blueNode[1]=.7;
    blueNode[2]=1.0;
    blueNode[3]=.2;
    
    blueLight[0]=0.8;
    blueLight[1]=.2;
    blueLight[2]=1.0;
    blueLight[3]=.2;

    bMedia = 1;
    bExternal = 1;
    maxdist = 100.00;
    centerindex = -1;
    generationNodeLimit = 30;
    generationLinkLimit = 30;
    groupKey = 0;
    spanPolicy = HV_SPANKEEP;
	spanPolicy = HV_SPANFOREST;
    linkPolicy = HV_LINKLOCAL;
    edgesize = 5;
    gotoStepSize = .015; // 1.0 / gotoStepSize == number of steps
    tossEvents = 0;			     
    keepAspect = 0;

    for(size_t i=0;i<16;i++){ //cheeseball Init sjk2012
	float temp1;
	clownColor[i][0]=(i%4)/4.0;
	clownColor[i][1]=((i/4)%4)/4.0;
	clownColor[i][2]=1.0-clownColor[i][0];
	clownColor[i][3]=1.0;
	if ((2==i%4)&&(2==((i/4)%4))){  // no grey ALLOWED (NOR BLACK OR WHITE)
		clownColor[i][0]=1.0;
		clownColor[i][1]=0.0;
		clownColor[i][2]=((i/4)%4)/4.0;
		clownColor[i][3]=1.0;
	}
    }
  }

  ~HypData() {}

  // in usec
  timeval dynamictime;
  timeval idletime;
  timeval picktime;


  /* draw sphere? */
  int bSphere;
  /* draw remote site nodes? */
  int bExternal;
  /* draw non-HTML nodes? */
  int bMedia;
  float colorSphere[4];
  float colorBack[4];
  float colorHilite[4];
  float colorSelect[4];
  float colorLabel[4];
  float colorLinkFrom[4];
  float colorLinkTo[4];
  float dndsColorLinkFrom[4];
  float dndsColorLinkTo[4];
  float clownColor[16][4];
  float * getClownColor(int input);
  bool isChordateTree;
  float edgesize;
  /* draw labels or not */
  int labels;
  int hv_labelOctal;
  /* nodes bigger than this should have labels drawn.
     size is in pixels, assuming window is 1000 pixels wide.
     scaled wrt real window size when actually used*/
  
  float labelsize;
  /* font to draw labels with */
  char *labelfont;
  /* font size - in Windows font string is name only */
  int screentype;
  /*  LCD or PDF to determine the Label Drawing functions*/

#ifdef WIN32
  int labelfontsize;
#endif
  /* how many events to throw away for every 1 processed when mouse moving */
  int passiveCull;
  /* how many events to throw away for every 1 processed when mouse dragging */
  int motionCull;
  /* how far from center to draw */
  int generationNodeLimit;
  int generationLinkLimit;

  // layout
  float areafudge;	// adjust disk area: pad circlepack to get sphere
  float lengthfudge;	// adjust edgelength: pad so spheres dont touch
  float leafrad;	// radius of disk allocated to single leaf node
  float maxlength;	// edgelength maximum cutoff: else precision problems
  float leafsize;

  int winx;
  int winy;
  /* positive infinity - distance beyond which you'd never draw a node */
  double maxdist;
  int centerindex;
  int groupKey;
  float greenLeaf[4];
  float blueNode[4];
  float greenLight[4];
  float blueLight[4];
  int bCenterShow;
  int bCenterLargest;
  int bNegativeHide;
  int spanPolicy;
  int linkPolicy;
  float gotoStepSize;
  float broccoliTops; //reduces the end peices for better viewing in some cases.//sjk2011
  int tossEvents;
  int keepAspect;
  int nodeColorScheme;
  bool landMarkPath;  
  bool pathVisible;
  int startNextLandmarkIndex;
};

#endif
