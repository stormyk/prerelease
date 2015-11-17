
#include <string>
#include <iostream>
#include <ostream>


//NAMESPACEHACK
using namespace std;//sjk2010NAMESPACEHACK

#include <sys/timeb.h>
//#include <afx.h>
//#include "newick.h"
#include "HypView.h"
#include "HypData.h"
#include "HypNode.h"

#include <fstream>

#include "newick.h"
#include "main.h"
#include "gl2ps.h"


char prevsel[1024];
HypView *hv;

int created = 0;
int gotfile = 0;
int width = 0;
int height = 0;
int f=0;
int lineWidth=0;//sjk2011  
int colorBranchToggle=0;
void draw() { if (created && gotfile) hv->drawFrame(); }
string prefix;
string postfix;
std::string drawgramFillFile;
string leftPathString;
string rightPathString;
bool delayLoad=true;
bool delayOccured=false;
const char * delayFname;

void setPrefix(char * input){
	//cout << "DEBUG:  main-prefixassign "<<endl;
	prefix.assign(input);
}
void setPostfix(char * input){
	postfix.assign(input);
}
void motion(int x, int y, int shift, int control) {
  if (created && gotfile) hv->motion(x,y,shift,control);
}
void mouse(int b, int s, int x, int y, int shift, int control) {
  if (created && gotfile) hv->mouse(b,s,x,y,shift,control);
}
void passive(int x, int y, int shift, int control) {
  if (created && gotfile) hv->passive(x,y,shift,control);
}
void resize(int w, int h) {
  if (created && gotfile) hv->reshape(w,h);
  width = w;
  height = h;
}
int onidle() {
  int stayidle = 0;
  if (created && gotfile) stayidle = hv->idleCB();
  return stayidle;
}
void tagLeft(){
	leftPathString=hv->getPick();
}

void tagRight(){
	rightPathString=hv->getPick();
}

void gNode(char buf[500]){
	hv->gotoNode(buf, HV_ANIMATE);
}
void gNextNode(char buf[500]){
	hv->gotoNextNode(buf,false, HV_ANIMATE);
}

void keyboard(unsigned char key) {	
  if (!gotfile) {return;}// dont muck with a graph that isnt there.
  static int gen = 30;
  static int labels = 7;
  static int sphere = 1;
  static int external = 1;
  static int orphan = 1;
  static int media = 1;
  static int group = 0;
  static int incoming = 0;
  static int selincoming = 0;
  static int subincoming = 0;
  static int outgoing = 0;
  static int seloutgoing = 0;
  static int suboutgoing = 0;
  static int rendstyle = 0;
  static int drawlinks = 1;
  static int drawnodes = 1;
  static int showcenter = 0;
  static int negativehide = 0;
  static int selsubtree = 0;
  static int labelright = 0;
  static int labelOct = 7;
  static int backcolor = 0;
  static int keepaspect = 0;
  static int rybGradient=1;
  char * NCBIbuffer="http://www.ncbi.nlm.nih.gov/gquery/?term= ";
  //setPrefix(NCBIbuffer);
  //setPostfix("");

  hv->idle(0);
  if (key >= '0' && key <= '9') {
    gen = key-'0';
    //if (rendstyle)//non-functional 
      hv->setGenerationNodeLimit(gen);
    //else 
    //  hv->setGenerationLinkLimit(gen);
  } else if (key == '+') {
	  hv->resetAllLinkColors();
  } else if (key == '*') {
      hv->setGenerationNodeLimit(99);
      hv->setGenerationLinkLimit(99);
  } else if (key == 'L') {
    labels++;
    if (labels >= 8) labels = 0;
    hv->setLabels(labels);
  } else if (key == 't') {
	  string tmpstr;
	  tmpstr=hv->getPick();
	  hv->toggleTitleFlag(tmpstr);
  } else if (key == 'T') {
	  hv->clearAllTitleFlags();
	//if (colorBranchToggle==1){keyboard('G');}// turn off the 
	//rybGradient= !rybGradient;
	//hv->setRybGradient(rybGradient);
  } else if (key == 'l') {
    labelright = !labelright;
    hv->setLabelToRight(labelright);
  } else if (key == 'b') {
    float labsize = hv->getLabelSize();
    fprintf(stderr, "labsize %f\n", labsize);
    --labsize;
	if (labsize<-10){labsize++;}
    hv->setLabelSize(labsize);
  } else if (key == 'B') {
    float labsize = hv->getLabelSize();
    fprintf(stderr, "labsize %f\n", labsize);
    ++labsize;
	if (labsize>50){labsize--;}
    hv->setLabelSize(labsize);
  } else if (key == 's') {
    sphere = !(sphere);
    hv->setSphere(sphere);
	hv->redraw();
  } else if (key == 'e') {
    external = !(external);
    hv->setDisableGroup(1, "external", external);    
  } else if (key == 'y') {// undo subtree clip
	  //Reset to loaded Graph
	  //crashes with a highlighted path open.
	  hv->resetPath();
	  //hv->getStartGraph();  //SJK X1-DEBUG

      hv->setGraph(hv->getStartGraph());
	keyboard('+');// reset link colors
    
  } else if (key == 'Y') {//DONT undo subtree clip
	hv->resetPath();
	hv->setGraph(newickConvertBootStrap(hv->getSpanGraph()));
	keyboard('+');// reset link colors
    
  
  } else if (key == 'E') {
    lineWidth++;
	if (1==lineWidth){hv->setLineWidth(2.0);}
    if (2==lineWidth){hv->setLineWidth(4.0);}
    if (3==lineWidth){hv->setLineWidth(8.0);}
	if (4==lineWidth){hv->setLineWidth(16.0);}	
	if (5==lineWidth){hv->setLineWidth(1.0);lineWidth=0;}
	hv->drawFrame();
  } else if ((key == 'O')||(key == 'o')) {
	
	  vector <char> newickChars;
	  string newickString;
	  string tmpFilename;
	  
	  newickChars= hv->getDrawgramSafeSpanGraph();
		  for(size_t i=0;i<newickChars.size();i++){
			  newickString.push_back(newickChars[i]);
		  }
		  char buffer [512];
		  tmpnam(buffer);
		  tmpFilename.assign(buffer);
		  FILE * nFile;
		  nFile=fopen("intree","w");
		  if (nFile!=NULL){
			  fputs(newickString.c_str(),nFile);// graph here..
			  fclose(nFile);
		  }
	// :KLUDGE:  this should be a resource.
	// I dont know the right way to access it. SJK2011
    newickString.assign("\
CA 501 21 18 28\n\
 -1135 1956 2735 -1442 2442 -12835\n\
CB 502 21 21 28\n\
 -1435 1456 2356 2655 2754 2852 2850 2748 2647 2346\n\
 -1446 2346 2645 2744 2842 2839 2737 2636 2335 1435\n\
 -13135\n\
CC 503 21 21 28\n\
 -2851 2753 2555 2356 1956 1755 1553 1451 1348 1343\n\
 1440 1538 1736 1935 2335 2536 2738 2840 -13135\n\
CD 504 21 21 28\n\
 -1435 1456 2156 2455 2653 2751 2848 2843 2740 2638\n\
 2436 2135 1435 -13135\n\
CE 505 21 19 28\n\
 -1435 1456 2756 -1446 2246 -1435 2735 -12935\n\
CF 506 21 18 28\n\
 -1435 1456 2756 -1446 2246 -12835\n\
CG 507 21 21 28\n\
 -2851 2753 2555 2356 1956 1755 1553 1451 1348 1343\n\
 1440 1538 1736 1935 2335 2536 2738 2840 2843 2343\n\
 -13135\n\
CH 508 21 22 28\n\
 -1456 1435 -2856 2835 -1446 2846 -13235\n\
CI 509 21 8 28\n\
 -1456 1435 -11835\n\
CJ 510 21 16 28\n\
 -2256 2240 2137 2036 1835 1635 1436 1337 1240 1242\n\
 -12635\n\
CK 511 21 21 28\n\
 -1456 1435 -2856 1442 -1947 2835 -13135\n\
CL 512 21 17 28\n\
 -1456 1435 2635 -12735\n\
CM 513 21 24 28\n\
 -1435 1456 2235 -3056 2235 -3056 3035 -13435\n\
CN 514 21 22 28\n\
 -1435 1456 2835 -2856 2835 -13235\n\
CO 515 21 22 28\n\
 -1956 1755 1553 1451 1348 1343 1440 1538 1736 1935\n\
 2335 2536 2738 2840 2943 2948 2851 2753 2555 2356\n\
 1956 -13235\n\
CP 516 21 21 28\n\
 -1435 1456 2356 2655 2754 2852 2849 2747 2646 2345\n\
 1445 -13135\n\
CQ 517 21 22 28\n\
 -1956 1755 1553 1451 1348 1343 1440 1538 1736 1935\n\
 2335 2536 2738 2840 2943 2948 2851 2753 2555 2356\n\
 1956 -2239 2833 -13235\n\
CR 518 21 21 28\n\
 -1435 1456 2356 2655 2754 2852 2850 2748 2647 2346\n\
 1446 -2146 2835 -13135\n\
CS 519 21 20 28\n\
 -2753 2555 2256 1856 1555 1353 1351 1449 1548 1747\n\
 2345 2544 2643 2741 2738 2536 2235 1835 1536 1338\n\
 -13035\n\
CT 520 21 16 28\n\
 -1835 1856 1156 2556 -12635\n\
CU 521 21 22 28\n\
 -1456 1441 1538 1736 2035 2235 2536 2738 2841 2856\n\
 -13235\n\
CV 522 21 18 28\n\
 -1156 1935 2756 -12835\n\
CW 523 21 24 28\n\
 -1256 1735 2256 2735 3256 -13435\n\
CX 524 21 20 28\n\
 -1356 2735 -2756 1335 -13035\n\
CY 525 21 18 28\n\
 -1156 1946 1935 -2756 1946 -12835\n\
CZ 526 21 20 28\n\
 -1356 2756 1335 2735 -13035\n\
Ca 601 21 19 28\n\
 -2549 2535 -2546 2348 2149 1849 1648 1446 1343 1341\n\
 1438 1636 1835 2135 2336 2538 -12935\n\
Cb 602 21 19 28\n\
 -1456 1435 -1446 1648 1849 2149 2348 2546 2643 2641\n\
 2538 2336 2135 1835 1636 1438 -12935\n\
Cc 603 21 18 28\n\
 -2546 2348 2149 1849 1648 1446 1343 1341 1438 1636\n\
 1835 2135 2336 2538 -12835\n\
Cd 604 21 19 28\n\
 -2556 2535 -2546 2348 2149 1849 1648 1446 1343 1341\n\
 1438 1636 1835 2135 2336 2538 -12935\n\
Ce 605 21 18 28\n\
 -1343 2543 2545 2447 2348 2149 1849 1648 1446 1343\n\
 1341 1438 1636 1835 2135 2336 2538 -12835\n\
Cf 606 21 12 28\n\
 -2056 1856 1655 1552 1535 -1249 1949 -12235\n\
Cg 607 21 19 28\n\
 -2549 2533 2430 2329 2128 1828 1629 -2546 2348 2149\n\
 1849 1648 1446 1343 1341 1438 1636 1835 2135 2336\n\
 2538 -12935\n\
Ch 608 21 19 28\n\
 -1456 1435 -1445 1748 1949 2249 2448 2545 2535 -12935\n\
Ci 609 21 8 28\n\
 -1356 1455 1556 1457 1356 -1449 1435 -11835\n\
Cj 610 21 10 28\n\
 -1556 1655 1756 1657 1556 -1649 1632 1529 1328 1128\n\
 -12035\n\
Ck 611 21 17 28\n\
 -1456 1435 -2449 1439 -1843 2535 -12735\n\
Cl 612 21 8 28\n\
 -1456 1435 -11835\n\
Cm 613 21 30 28\n\
 -1449 1435 -1445 1748 1949 2249 2448 2545 2535 -2545\n\
 2848 3049 3349 3548 3645 3635 -14035\n\
Cn 614 21 19 28\n\
 -1449 1435 -1445 1748 1949 2249 2448 2545 2535 -12935\n\
Co 615 21 19 28\n\
 -1849 1648 1446 1343 1341 1438 1636 1835 2135 2336\n\
 2538 2641 2643 2546 2348 2149 1849 -12935\n\
Cp 616 21 19 28\n\
 -1449 1428 -1446 1648 1849 2149 2348 2546 2643 2641\n\
 2538 2336 2135 1835 1636 1438 -12935\n\
Cq 617 21 19 28\n\
 -2549 2528 -2546 2348 2149 1849 1648 1446 1343 1341\n\
 1438 1636 1835 2135 2336 2538 -12935\n\
Cr 618 21 13 28\n\
 -1449 1435 -1443 1546 1748 1949 2249 -12335\n\
Cs 619 21 17 28\n\
 -2446 2348 2049 1749 1448 1346 1444 1643 2142 2341\n\
 2439 2438 2336 2035 1735 1436 1338 -12735\n\
Ct 620 21 12 28\n\
 -1556 1539 1636 1835 2035 -1249 1949 -12235\n\
Cu 621 21 19 28\n\
 -1449 1439 1536 1735 2035 2236 2539 -2549 2535 -12935\n\
Cv 622 21 16 28\n\
 -1249 1835 -2449 1835 -12635\n\
Cw 623 21 22 28\n\
 -1349 1735 -2149 1735 -2149 2535 -2949 2535 -13235\n\
Cx 624 21 17 28\n\
 -1349 2435 -2449 1335 -12735\n\
Cy 625 21 16 28\n\
 -1249 1835 -2449 1835 1631 1429 1228 1128 -12635\n\
Cz 626 21 17 28\n\
 -2449 1335 -1349 2449 -1335 2435 -12735\n\
C0 700 21 20 28\n\
 -1956 1655 1452 1347 1344 1439 1636 1935 2135 2436\n\
 2639 2744 2747 2652 2455 2156 1956 -13035\n\
C1 701 21 20 28\n\
 -1652 1853 2156 2135 -13035\n\
C2 702 21 20 28\n\
 -1451 1452 1554 1655 1856 2256 2455 2554 2652 2650\n\
 2548 2345 1335 2735 -13035\n\
C3 703 21 20 28\n\
 -1556 2656 2048 2348 2547 2646 2743 2741 2638 2436\n\
 2135 1835 1536 1437 1339 -13035\n\
C4 704 21 20 28\n\
 -2356 1342 2842 -2356 2335 -13035\n\
C5 705 21 20 28\n\
 -2556 1556 1447 1548 1849 2149 2448 2646 2743 2741\n\
 2638 2436 2135 1835 1536 1437 1339 -13035\n\
C6 706 21 20 28\n\
 -2653 2555 2256 2056 1755 1552 1447 1442 1538 1736\n\
 2035 2135 2436 2638 2741 2742 2645 2447 2148 2048\n\
 1747 1545 1442 -13035\n\
C7 707 21 20 28\n\
 -1356 2756 1735 -13035\n\
C8 708 21 20 28\n\
 -1856 1555 1453 1451 1549 1748 2147 2446 2644 2742\n\
 2739 2637 2536 2235 1835 1536 1437 1339 1342 1444\n\
 1646 1947 2348 2549 2651 2653 2555 2256 1856 -13035\n\
C9 709 21 20 28\n\
 -2649 2546 2344 2043 1943 1644 1446 1349 1350 1453\n\
 1655 1956 2056 2355 2553 2649 2644 2539 2336 2035\n\
 1835 1536 1438 -13035\n\
C. 710 21 10 28\n\
 -1537 1436 1535 1636 1537 -12035\n\
C, 711 21 10 28\n\
 -1636 1535 1436 1537 1636 1634 1532 1431 -12035\n\
C: 712 21 10 28\n\
 -1549 1448 1547 1648 1549 -1537 1436 1535 1636 1537\n\
 -12035\n\
C; 713 21 10 28\n\
 -1549 1448 1547 1648 1549 -1636 1535 1436 1537 1636\n\
 1634 1532 1431 -12035\n\
C! 714 21 10 28\n\
 -1556 1542 -1537 1436 1535 1636 1537 -12035\n\
C? 715 21 18 28\n\
 -1351 1352 1454 1555 1756 2156 2355 2454 2552 2550\n\
 2448 2347 1945 1942 -1937 1836 1935 2036 1937 -12835\n\
C/ 720 21 22 28\n\
 -3060 1228 -13235\n\
C( 721 21 14 28\n\
 -2160 1958 1755 1551 1446 1442 1537 1733 1930 2128\n\
 -12435\n\
C) 722 21 14 28\n\
 -1360 1558 1755 1951 2046 2042 1937 1733 1530 1328\n\
 -12435\n\
C- 724 21 26 28\n\
 -1444 3244 -13635\n\
C* 728 21 16 28\n\
 -1850 1838 -1347 2341 -2347 1341 -12635\n\
C  699 21 16 28\n\
-12635\n\
");
	nFile=fopen("fontfile","w");
	if (nFile!=NULL){
		fputs(newickString.c_str(),nFile);
	    fclose(nFile);
	}
	nFile=fopen("y.txt","w");
	if (nFile!=NULL){
		fputs("y",nFile);
	    fclose(nFile);
	}
	//printf("DEBUG: WINDOWS TRY\n");
	//system ("C:\\drawgram.exe <y.txt\n");
	//printf("DEBUG: Linux Try TRY\n");
	//system ("wine drawgram.exe\n");
	//:KLUDGE:
	//  The System Call doesnt return control back to TreeThrasher
	//  until drawgram is closed.
	//  Need a system that will work correctly under Wine.
	if (key== 'O'){ system ("drawgram.exe <y.txt &");}
	if (key== 'o'){ system ("drawgram.exe ");};
    //printf("DEBUG: end of tries\n");
  //}
  //  else if (key == 'o') {
  //  orphan = !(orphan);
  //  hv->setDisableGroup(1, "orphan", orphan);    
  } else if (key == 'i') {
    incoming = !(incoming);
    hv->setDrawBackTo("HV_TOPNODE", incoming, 1);
  } else if (key == 'j') {
    selincoming = !(selincoming);
    hv->setDrawBackTo(prevsel, selincoming, 1);
  } else if (key == 'J') {
	  hv->setMaxLength(1);
    
  
  } else if (key == 'k') {
    subincoming = !(subincoming);
    hv->setDrawBackTo(prevsel, subincoming, 0);
  } else if (key == 'u') {
    outgoing = !(outgoing);
    hv->setDrawBackFrom("HV_TOPNODE", outgoing, 1);
  } else if (key == 'v') {
    seloutgoing = !(seloutgoing);
    hv->setDrawBackFrom(prevsel, seloutgoing, 1);
  //} else if (key == 'w') {
  //  suboutgoing = !(suboutgoing);
  //  hv->setDrawBackFrom(prevsel, suboutgoing, 0);
  } else if (key == 'S') {
    selsubtree = !(selsubtree);
    hv->setSelectedSubtree(prevsel, selsubtree);
  } else if (key == 'c') {
    hv->gotoCenterNode(HV_ANIMATE);
  } else if (key == '%') {
    hv->gotoSelectNode(HV_ANIMATE);
  } else if (key == 'C') {
    showcenter = !(showcenter);
    hv->setCenterShow(showcenter);
  } else if (key == 'r') {
	  hv->cycleFogLevel();
	  
	  if (backcolor == 0) {
		  hv->setColorBackground(1.0, 1.0, 1.0);
	  } else if (backcolor == 1) {
		  hv->setColorBackground(0.5, 0.5, 0.5);
	  } else if (backcolor == 2){
		  hv->setColorBackground(0.0, 0.0, 0.0);
	  }
	  

	
  } else if (key == 'R') {// RECOLOR CYCLE
    backcolor++;
    if (backcolor >= 3) backcolor = 0;
    if (backcolor == 0) {
		//hv->setLinkPolicy(HV_LINKCENTRAL);
      hv->setColorBackground(1.0, 1.0, 1.0);
      hv->setColorSphere(.89, .7, .6);
      hv->setColorLabel(0.0, 0.0, 0.0);
    } else if (backcolor == 1) {
      hv->setColorBackground(0.5, 0.5, 0.5);
      hv->setColorSphere(.5, .4, .3);
      hv->setColorLabel(0.0, 0.0, 0.0);
    } else if (backcolor == 2){
      hv->setColorBackground(0.0, 0.0, 0.0);
      hv->setColorSphere(.25, .15, .1);
      hv->setColorLabel(1.0, 1.0, 1.0);
    }else{
	  //hv->setLinkPolicy(HV_LINKINHERIT);
	  hv->setLinkPolicy(1);
      hv->setColorBackground(1.0, 1.0, 1.0);
      hv->setColorSphere(.25, .15, .1);
      hv->setColorLabel(0.0, 0.0, 0.0);
    }
    
  } else if (key == 'N') {
    drawlinks = !(drawlinks);
    hv->setDrawLinks(drawlinks);
  } else if (key == 'n') {
    drawnodes = !(drawnodes);
    hv->setDrawNodes(drawnodes);
  } else if (key == 'h') {
    negativehide = !(negativehide);
    hv->setNegativeHide(negativehide);
  } else if (key == 'H') {
	hv->toggleDrawShape();
	
	hv->redraw();
	//hv->drawFrame();
	//hv->redraw();
  } else if (key == '<') {
	tagLeft();	  
  } else if (key == '>') {
	tagRight();	  
  } else if (key == '[') {
	  hv->gotoPrevLandmark(HV_ANIMATE);	 
  } else if (key == ']') {
	  hv->gotoNextLandmark(HV_ANIMATE);	  
  } else if (key == 'z') {
	  hv->landmarkHighDnds();
  } else if (key == 'Z') {
	  bool lmp=hv->landmarkPath();
	  keyboard('?');// do for either
	  if (!lmp){keyboard('/');}//turn it back off if needed.
	  


  } else if (key == '?') {
	  hv->showPath(leftPathString,rightPathString);
  } else if (key == '/') {
	  hv->clearPath();
  } else if (key == ',') {
	  char buffer[512];
	  sprintf(buffer,"%s",leftPathString.c_str());
	  gNode(buffer);
  } else if (key == '.') {
	  char buffer[512];
	  sprintf(buffer,"%s",rightPathString.c_str());
	  gNode(buffer);
  } else if (key == 'K') {
    keepaspect = !(keepaspect);
    hv->setKeepAspect(keepaspect);
  } else if (key =='M'){
		vector <char> tmpchrv,newickGraph;
		hv->resetPath();
	    hv->toggleBroccoliTops();
	    tmpchrv= hv->getSpanGraph();
		for(size_t i=0;i<tmpchrv.size();i++){newickGraph.push_back(tmpchrv.at(i));}
		hv->setGraph(newickGraph);
  } else if (key == 'm') {
    media = !(media);
    //hv->setDisableGroup(0, "image", media);    
    //hv->setDisableGroup(0, "text", media);    
    //hv->setDisableGroup(0, "application", media);    
    //hv->setDisableGroup(0, "audio", media);    
    //hv->setDisableGroup(0, "audio", media);    
    //hv->setDisableGroup(0, "video", media);    
    //hv->setDisableGroup(0, "other", media);    
    //    hv->newLayout(NULL);
  }else if (key == 'D') {
	  hv->toggleChordateDenseView();
	  keyboard('Y');
	  keyboard('+');// reset link colors
	  hv->setLineWidth(16.0);
	  lineWidth=4;
	  
  }
  /*--------------------------------------------------
  |        RECOLORING RULE TO LEAF NODES
  |
  |---------------------------------------------*/
  else if (key == 'G') { 
	  hv->cycleNodeScheme();
	  //hv->toggleLeafColorScheme();//deprecated
	  /*
	    vector <char> tmpchrv;
		vector <char> newickGraph;
		string rule;
		if (colorBranchToggle==0){
			rule.assign("RULE:COLORBRANCH");
			colorBranchToggle=1;
			rybGradient= true;
            hv->setRybGradient(rybGradient);
		}else{
			rule.assign("");
			colorBranchToggle=0;
			rybGradient= true;
            hv->setRybGradient(rybGradient);
		}
		for(size_t i=0;i<rule.size();i++){
			newickGraph.push_back(rule[i]);
		}
		tmpchrv= hv->getSpanGraph();
		for(size_t i=0;i<tmpchrv.size();i++){newickGraph.push_back(tmpchrv.at(i));}
		hv->setGraph(newickGraph);
		*/

  } else if (key == 'g') {
	  
		string tmpstr;
		//tmpstr=hv->getHighlightNode();
		tmpstr=hv->getPick();
		cout << "DEBUG:Clip function:: "<< tmpstr<<endl;
		//string debugLine="(a,b,c,d,e,(f,(t,u,v)g,h,i,j)m)n;";
		vector <char> tmpchrv;
		string debugLine;
		tmpchrv= hv->getDrawgramSafeSpanGraph();
		for(size_t i=0;i<tmpchrv.size();i++){debugLine.push_back(tmpchrv.at(i));}
		vector <string> junk;
		junk.push_back(debugLine);
        //vector <char> tmpchrv;
		//for(size_t i=0;i<debugLine.size();i++){tmpchrv.push_back(debugLine.at(i));}
		vector <char> junk2 = getNewickSubgraph(tmpchrv,tmpstr);
		//hv->setGraph(getNewickSubgraph(tmpchrv,tmpstr.c_str()));


		hv->setGraph(junk2);
		
		
		//hv->setGraph(hv->getSpanGraph());
		//tmpchrv = hv->getSpanGraph();
	    //hv->setGraph(getNewickSubgraph(hv->getSpanGraph(),tmpstr));
		printf( "HighliteNode :%s\n",tmpstr.c_str());
		for(size_t i=0;i<junk2.size();i++){
			printf("%c",junk2[i]);
		}
		printf("\n");
		for(size_t i=0;i<tmpchrv.size();i++){
			printf("%c",tmpchrv[i]);
		}
		printf("\n");
		//hv->setGraph(junk);
	  

	  //int x223=0; // junk line g was broken already
    //group = !(group);
    //hv->setGroupKey(group);
  }
  else if (key == '#'){
	  char buffer[660];
	  string tmpstr;
	  tmpstr=hv->getPick();
	  tmpstr=sanitizeWebAddress(tmpstr);
	  //setPrefix("http://www.bing.com");
	  string cmdstr;
	  sprintf(buffer,"%s",prefix.c_str());
	  if (prefix.size()<2){prefix.assign("http://scholar.google.com/scholar?q=");}
	  printf("DEBUG::prefix:> %s\n",prefix.c_str());
	  //sprintf(buffer,"http://www.google.com/#q=");
	  cmdstr.assign(buffer);
	  sprintf(buffer,"%s%s",cmdstr.c_str(),tmpstr.c_str());
	  tmpstr.assign(buffer);
	  ShellExecute(NULL,"open", tmpstr.c_str(),NULL,NULL,SW_SHOWNORMAL);

  
  
 /* }else if (key == 'A'){
	  char buffer[660];
	  string tmpstr;
	  sprintf(buffer,"uninitilized");
	  tmpstr.assign(buffer);
	  tmpstr=hv->getPick();
	  tmpstr=sanitizeWebAddress(tmpstr);
	  string cmdstr;
	  sprintf(buffer,"%s",prefix.c_str());
	  printf("DEBUG::prefix:> %s\n",prefix.c_str());
	  sprintf(buffer,"http://www.google.com/search?q=");
	  cmdstr.assign(buffer);
	  sprintf(buffer,"%s%s",cmdstr.c_str(),tmpstr.c_str());
	  tmpstr.assign(buffer);
	  ShellExecute(NULL,"open", tmpstr.c_str(),NULL,NULL,SW_SHOWNORMAL);*/

  }else if (key == 'W') {
	  // activate web
	  char buffer[660];
	  string tmpstr;
	  sprintf(buffer,"uninitilized");
	  tmpstr.assign(buffer);
	  tmpstr=hv->getPick();
	  tmpstr=sanitizeWebAddress(tmpstr);
	  string cmdstr;
	  sprintf(buffer,"http://www.google.com/search?q=");
	  cmdstr.assign(buffer);
	  sprintf(buffer,"%s%s",cmdstr.c_str(),tmpstr.c_str());
	  tmpstr.assign(buffer);
	  ShellExecute(NULL,"open", tmpstr.c_str(),NULL,NULL,SW_SHOWNORMAL);
}else if (key == 'w') {
	  // activate web
	  char buffer[660];
	  string tmpstr;
	  sprintf(buffer,"uninitilized");
	  tmpstr.assign(buffer);
	  tmpstr=hv->getPick();
	  tmpstr=sanitizeWebAddress(tmpstr);
	  string cmdstr;
	  //sprintf(buffer,"http://www.google.com/#q=NCBI");
	  cmdstr.assign(NCBIbuffer);
	  sprintf(buffer,"%s%s",cmdstr.c_str(),tmpstr.c_str());
	  tmpstr.assign(buffer);
	  ShellExecute(NULL,"open", tmpstr.c_str(),NULL,NULL,SW_SHOWNORMAL);
}else if (key == 'A') {
	  // activate web
	  char buffer[660];
	  string tmpstr;
	  sprintf(buffer,"uninitilized");
	  tmpstr.assign(buffer);
	  tmpstr=hv->getPick();
	  tmpstr=extractGiFromWebAddress(tmpstr);
	  string cmdstr;
	  sprintf(buffer,"http://www.ncbi.nlm.nih.gov/gquery/?term=");
	  cmdstr.assign(buffer);
	  sprintf(buffer,"%s%s",cmdstr.c_str(),tmpstr.c_str());
	  tmpstr.assign(buffer);
	  ShellExecute(NULL,"open", tmpstr.c_str(),NULL,NULL,SW_SHOWNORMAL);

  }else if (key == 'a') {
	  // activate web
	  char buffer[660];
	  string tmpstr;
	  sprintf(buffer,"uninitilized");
	  tmpstr.assign(buffer);
	  tmpstr=hv->getPick();
	  //tmpstr=sanitizeWebAddress(tmpstr);// TAED site doesnt need a sanitize currently
	  string cmdstr;
	  //sprintf(buffer,"http://www.wyomingbioinformatics.org/search.php?PASSWORD=Industrial1259&TYPE=0&GI=");
	  sprintf(buffer,"http://www.wyomingbioinformatics.org/TAED/TAEDPoratal.php?GI=");
	  cmdstr.assign(buffer);
	  sprintf(buffer,"%s%s",cmdstr.c_str(),tmpstr.c_str());
	  tmpstr.assign(buffer);
	  ShellExecute(NULL,"open", tmpstr.c_str(),NULL,NULL,SW_SHOWNORMAL);
  }

    
  else if (key == 'P') {// Plot
	pdfPlot(1000,1000,"TreeThrasher.pdf",0);
/*	//GLint vp[]={0,0,4096,4096};

	FILE * fp;
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec=50000;

    hv->setDynamicFrameTime(tv);
	fp = fopen("out.pdf","wb");
	
	int buffSize=4096*4096*3;
    
    gl2psBeginPage("TreeThrasher PLOT","TreeThrasher Viewer",vp,GL2PS_PDF,GL2PS_BSP_SORT,GL2PS_DRAW_BACKGROUND|GL2PS_OCCLUSION_CULL,GL_RGBA,0,NULL,0,0,0,buffSize,fp,"out.eps");

	hv->reshape(4096,4096);
	hv->drawStringInit();
	hv->drawFrame();
	    onidle();
	
	gl2psEndPage();
    fclose(fp);

	hv->reshape(width,height);  
	tv.tv_sec = 0;
	tv.tv_usec=50000;
	hv->setDynamicFrameTime(tv);
*/
  }

  hv->redraw();
}

void   pdfPlot(int x,int y, string filename, int seconds) {// Plot
	GLint vp[]={0,0,x,y};  

	FILE * fp;
	timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec=50000;

    hv->setDynamicFrameTime(tv);
	fp = fopen(filename.c_str(),"wb");
	//int buffSize=1024*1024*3;
	int buffSize=x*y*3;
    //XXXgl2psBeginPage("TAED PLOT","TAED Viewer",vp,GL2PS_PDF,GL2PS_SIMPLE_SORT,GL2PS_USE_CURRENT_VIEWPORT,GL_RGBA,0,NULL,0,0,0,buffSize,fp,"out.eps");
	//XXXgl2psBeginPage("TAED PLOT","TAED Viewer",vp,GL2PS_PDF,GL2PS_BSP_SORT,GL2PS_USE_CURRENT_VIEWPORT,GL_RGBA,0,NULL,0,0,0,buffSize,fp,"out.eps");
//	gl2psBeginPage("TAED PLOT","TAED Viewer",vp,GL2PS_PDF,GL2PS_BSP_SORT,GL2PS_DRAW_BACKGROUND|GL2PS_OCCLUSION_CULL,GL_RGBA,0,NULL,0,0,0,buffSize,fp,"out.eps");
    gl2psBeginPage("TreeThrasher PLOT","TreeThrasher Viewer",vp,GL2PS_PDF,GL2PS_BSP_SORT,GL2PS_DRAW_BACKGROUND|GL2PS_OCCLUSION_CULL,GL_RGBA,0,NULL,0,0,0,buffSize,fp,"out.eps");
	//XXXgl2psBeginPage("TAED PLOT","TAED Viewer",vp,GL2PS_PDF,GL2PS_NO_SORT,GL2PS_DRAW_BACKGROUND,GL_RGBA,0,NULL,0,0,0,buffSize,fp,"out.eps");
	hv->reshape(x,y);
	hv->drawStringInit();
	//float lblsz=hv->getLabelSize();
	//hv->setLabelSize(lblsz *1.2);
	hv->drawFrame();
	    onidle();
	//hv->drawSphere();
	//for(size_t i=0;i<200;i++){
	//	onidle();
	//}
	
	gl2psEndPage();
    fclose(fp);
	//hv->setLabelSize(lblsz);
	hv->reshape(width,height);  
	tv.tv_sec = 0;
	tv.tv_usec=50000;
	hv->setDynamicFrameTime(tv);

  }



void selectCB(const string & id, int /*shift*/, int /*control*/) {
  hv->setSelected(id, 1);
  hv->setSelected(prevsel, 0);
  
  if (id.find('|') != id.npos) {
	//string top=id.substr(0,id.find('|')); //sjk2011 debug
    //	hv->gotoNode(top, HV_ANIMATE);     //sjk2011 debug
    // it's an edge, not a node
    hv->gotoPickPoint(HV_ANIMATE);
  }else{
  //hv->setSelected(id, 1);
  //hv->setSelected(prevsel, 0);
	hv->gotoNode(id, HV_ANIMATE);
  }
  //hv->redraw();  //sjk2012 removal
  strcpy(prevsel, id.c_str());
} //selectCB

BOOL bSetupPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR *ppfd; 
    PIXELFORMATDESCRIPTOR pfd = { 
	sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
	1,                              // version number 
	PFD_DRAW_TO_WINDOW |            // support window 
	PFD_SUPPORT_OPENGL |            // support OpenGL 
	PFD_DOUBLEBUFFER,               // double buffered 
	PFD_TYPE_RGBA,                  // RGBA type 
	32,                             // 24-bit color depth 
	0, 0, 0, 0, 0, 0,               // color bits ignored 
	1,                              // no alpha buffer 
	0,                              // shift bit ignored 
	1,                              // no accumulation buffer 
	0, 0, 0, 0,                     // accum bits ignored 
	32,                             // 32-bit z-buffer	 
	//0,                              // no stencil buffer 
	//0,                              // no auxiliary buffer 
	8,                              // no stencil buffer //sjk2013
	8,                              // no auxiliary buffer 
	PFD_MAIN_PLANE,                 // main layer 
	0,                              // reserved 
	0, 0, 0                         // layer masks ignored 
    }; 
    int pixelformat;

    pfd.cColorBits = GetDeviceCaps(hdc,BITSPIXEL);
    ppfd = &pfd;
    pixelformat = ChoosePixelFormat(hdc, ppfd); 
    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) {
        printf("ChoosePixelFormat failed");
        return FALSE; 
    }
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) {
        printf("SetPixelFormat failed");
        return FALSE; 
    }
    return TRUE; 
}


void create(HWND hWnd, int w, int h) {
  HDC hdc = ::GetDC(hWnd);
  hv =  new HypView(hdc,hWnd);
  //hv->setLineWidth(16.0);// multi-sized lines //sjk2011
  //lineWidth=4;           // multi-sized lines designato
  
  hv->clearSpanPolicy();
  hv->addSpanPolicy(HV_SPANHIER);
  hv->addSpanPolicy(HV_SPANLEX);
  hv->addSpanPolicy(HV_SPANBFS);

  hv->bindCallback(HV_LEFT_CLICK, HV_PICK);
  hv->bindCallback(HV_PASSIVE, HV_HILITE);
  hv->bindCallback(HV_LEFT_DRAG, HV_TRANS);
  hv->bindCallback(HV_RIGHT_DRAG, HV_ROT);
  hv->bindCallback(HV_MIDDLE_DRAG, HV_SPIN);
  hv->setPickCallback(selectCB);
  hv->setMotionCull(0);
  hv->setPassiveCull(2);
  //  hv->setLabelFont("-*-helvetica-bold-r-normal--12-*");
  hv->setSphere(1);
  hv->setDrawShape(3);
  hv->setLabels(HV_LABELLONG);
  hv->setGotoStepSize(.0833);
  hv->setCenterShow(0);
  hv->setLineWidth(1.0);
  hv->setGenerationNodeLimit(99);// prevents the ghost boxes by default
  hv->toggleLeafColorScheme();
  //hv->setLineWidth(16.0);// multi-sized lines //sjk2011
  //lineWidth=4;           // multi-sized lines designator
  created = 1;
  hv->redraw();//sjk2011  
  
  //delayLoad=false;
} //create

int saveFile(const char * fname){
	vector <char> newickChars;
	string newickString;
    newickChars= hv->getSpanGraph();
	for(size_t i=0;i<newickChars.size();i++){
		newickString.push_back(newickChars[i]);
	}
    FILE * nFile;
	nFile=fopen(fname,"w");
	if (nFile!=NULL){
		fputs(newickString.c_str(),nFile);// graph here..
	    fclose(nFile);
		return 1;
	}
	return 0;
} //saveFile
int openfile(const char *fname) {
	//if (delayLoad){delayFname=fname;delayOccured=true; return 1;}
  //CSetStatusLoadFile status_window;  //sjk2011 debug
  HDC hdc = hv->getWidget();
  bSetupPixelFormat(hdc);
  hv->resetPath();
  HGLRC ctx = wglCreateContext(hdc);
  if (!ctx) {
    printf("wglCreateContext Failed\n");
    exit(1);
  }
  hv->afterRealize(ctx, width, height);

  ifstream inFile(fname);
  if (!inFile) {
    return(0);
  }
  if (! (hv->setGraph(inFile))) exit(0);
  hv->startGraph=hv->getSpanGraph();
  hv->isChordateTree();

  // hv->newLayout(NULL);

  hv->setGroupKey(0); 
  // start out coloring by mime types
//  hv->setColorGroup(0, "html", 0.0, 1.0, 1.0);
//  hv->setColorGroup(0, "image", 1.0, 0.0, 1.0);
//  hv->setColorGroup(0, "router", 0, 1, 1);
//  hv->setColorGroup(0, "text", .90, .35, .05);
//  hv->setColorGroup(0, "source", .42, 0, .48);
//  hv->setColorGroup(0, "application", .99, .64, .25);
//  hv->setColorGroup(0, "audio", .91, .36, .57);
//  hv->setColorGroup(0, "video", .91, .36, .57);
//  hv->setColorGroup(0, "other", 0, .35, .27);
//  hv->setColorGroup(0, "vrml", .09, 0, 1);
//  hv->setColorGroup(0, "host", 1.0, 1.0, 1.0);
//  hv->setColorGroup(0, "invisible", 0, 0, 0);
  //sjk 2011 (4096 colors 12 bits -simple)
  for(size_t i=0;i<4096;i++){
	  int b =i % 16;
	  int g=i/16;
	  g=g%16;
	  int r=i/256;
	  char value[40];
	  sprintf(value,"color%d",i);
	  hv->addColorGroup(0,value,r/15.0,g/15.0,b/15.0);//change to ADD
  } //fori
  /*
  //
  // tree position
  // hv->setColorGroup(1, "host", 1.0, 1.0, 1.0);
  // hv->setColorGroup(1, "main", 0, 1, 1);
  // hv->setColorGroup(1, "orphan", 1.0, 0.0, 1.0);
  // hv->setColorGroup(1, "external", 1.0, 1.0, 1.0);
   */
  hv->setKeepAspect(0);

  //  hv->gotoCenterNode(HV_JUMP);
  struct timeval idletime;
  idletime.tv_sec = 1;
  idletime.tv_usec = 0;
  hv->setIdleFrameTime(idletime);
  gotfile = 1;
  return 1;
} // END OPENFILE(CONST CHAR *FNAME)

std::string sanitizeWebAddress(std::string input){
	if (input.find('#')==input.npos) {return input;}
	int pos=input.find('#');
	input.replace(pos,1,"%20");
	return (sanitizeWebAddress(input));
}

std::string extractGiFromWebAddress(std::string input){
	//return input; //sjkdebug
	size_t minGi=1;  //Even #2 is in there
	size_t maxGi=16; //  Assume number is clearly something else at this point.
	size_t tempStart=0;
	size_t tempSize=0;
	std::vector <char> result;
	bool giTagFound=false;
	bool acTagFound=false;
	size_t size=input.size();
	size_t first=0;
	size_t last=0;
	size_t contig;
	//return ("XYZZY");
	while (first<size){
		char current=input.c_str()[first];
		contig=contigDigitCount(first,input);
		if (('I'==current)||('i'==current&& first>0)){  //catch a real GI number if it can see something close
			if (('G'==input.c_str()[first-1])||('g'==input.c_str()[first-1])){
				giTagFound=true;
				first++;
				continue;
			}
		}
		if(isDigit(current)){	
			if ((contig>=minGi)&&(contig<=maxGi)){
				if (giTagFound) {
					return input.substr(first,contig);
				}else if (0==tempSize){// we want the first one only
					tempStart=first;
					tempSize=contig;
					first+=contig;
					continue;  //hoping that we will catch a GI:xxxxxxx and ignore this
					// though it will catch "12345678#Gibbon_Chromosome_7" as the number 7
					// this gives flexibility to catch "Gi_Number:987654321"
					// if all GI's are in front, this continue can become " return(first,contig);"
				}
			}else{// odd sized number
				first+=contig;
			}
			
		}// not a digit
		first++;	

	}
	if (0<tempSize){
		return input.substr(tempStart,tempSize);	
	}
	return (input);
}
bool isDigit(char input){
		if (input=='1') return true;	if (input=='5') return true;	if (input=='9') return true;
		if (input=='2') return true;	if (input=='6') return true;	if (input=='0') return true;
		if (input=='3') return true;	if (input=='7') return true;	
	      if (input=='4') return true;	if (input=='8') return true;
		return false;
}
size_t contigDigitCount(size_t start,std::string input){
	size_t size=input.size();
	if (start>size){return 0;}
	for(size_t i=start;i<size;i++){
		if (!isDigit(input.c_str()[i])){return (i-start);}
	}
	return size-start;
}