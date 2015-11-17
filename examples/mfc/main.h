#include <vector>

void create(HWND hWnd, int w, int h);
void afterRealize(HDC cx);
int openfile(const char *fname);
void keyboard(unsigned char key);
void setPrefix(char *input);
void setPostfix(char *input);
void gNode(char[]);
void gNextNode(char[]);
void motion(int x, int y, int shift, int control);
void mouse(int b, int s, int x, int y, int shift, int control);
void passive(int x, int y, int shift, int control);
void draw();
void resize(int w, int h);
int onidle();
int saveFile(const char *);
void tagLeft();
void tagRight();
std::string sanitizeWebAddress(std::string input);
void drawgramFileFill(int positon, char option, char value);
std::string extractGiFromWebAddress(std::string input);
bool isDigit(char input);
size_t contigDigitCount(size_t start,std::string input);


void   pdfPlot(int x,int y, std::string filename, int seconds); // Plot

//void tagLeft(char *input);
//void tagRight(char *input);
//vector <char> expandNewick(vector <char> input, int & firstNode);
//vector <char> expandNewick(vector <char> input){
//	int x=0;
//	return(expandNewick(input,x);)
//}

//vector <char> getNewickSubgraph(vector <char> inGraph, string name);


