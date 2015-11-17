#include <vector>
#include <string>

typedef std::vector < std::string > VS;

class nhx{
	public:
		nhx(std::vector <char> input){parseNewick(input);}
		VS getNames();
		VS getBranchLen();
		VS getDnds();
		VS getLvhist();
		
	//	std::vector <std::string> getNewThingHere();



	private:
		void parseNewick(std::vector <char>);
		void addEntry(std::string,int);
		void genLvhist();
		VS names;
		std::vector<int> level;
		VS branchLen;
		VS dnds;
		VS lvhist;
		std::vector <int> depthData;
		
};
