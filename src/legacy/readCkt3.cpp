#include <iostream>  // basic C++ input/output (e.g., cin, //cout)
#include <fstream>	// needed to open files in C++
#include <sstream>   // needed if you are using sstream (C++)
#include <stdio.h>	// needed to open files in C
#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <list>
#include <chrono> 
#include <deque>   
#include <stack>
#include <time.h>
//using namespace std::chrono;
#define MAX_FAN 8
//std::ofstream outfile("ckt_traversal.txt");					//Output text file
using namespace std;
class Nodes;						//Number of gates counter
map<double, Nodes> SFG;
struct edge_marker
{
	double node;
	string edge;
	double taken;
	edge_marker(double a, string b)	
	{
		node=a;
		edge=b;
		taken=0;
	}
};

class circuit{
	double Node;
	//string type; // CAN BE "I" means input node, "O", output node, "N", neutral node
	vector<string> branches;
public:
	circuit(double n)
	{
		Node=n;
	}
	void updateNode(double N)
	{
		Node=N;
	}
	void updateBranches(string b)
	{
		cout<<"\nSTRING OBTAINED IS "<<b<<"\n";
		if(b!="OP+" && b!="OP-" && b!="1")
		{
			if (b.rfind("C", 0) == 0)
			{
				b="s"+b;
				branches.push_back(b);
			}
			else
				branches.push_back(b);
		}
	}
	void displayBranches()
	{
		for(auto i=branches.begin();i!=branches.end();i++)
		{
			cout<<"\t"<<*i;
		}
	}
	string returnDPImp()
	{
		string s;
		auto j=branches.begin();
		s=s+*j;
		for(auto i=branches.begin()+1;i!=branches.end();i++)
		{
			s=s+"+"+*i;
		}
		string st="(1/("+s+"))";
		return st;
	}
};
map<double,circuit> CKT;
vector<double> V;
class Nodes{
	double NodeID;
	int flag;
	vector<edge_marker> input;		
	vector<edge_marker> output;
	int dfs_mark;
	int visited;
public:
	Nodes(double ID, int F)
	{
		NodeID=ID;
		flag=F;
		dfs_mark=0;
		V.push_back(NodeID);
		visited=0;
	}
	string returnSpecialEdge(double d)
	{
		int count=0;
		for (auto m=output.begin();m!=output.end();m++)
		{
			if((*m).node==d)
			{
				count++;	
			}
		}
		//cout<<"\nCOUNT ==" <<count<<" FOR d = "<<d<<"\n";
		for (auto i=output.begin();i!=output.end();i++)
		{
			if((*i).node==d)
			{
				if(count>1 && (*i).taken==0)
				{
					(*i).taken=1;
					//cout<<"\n %"<<(*i).edge<<"% \n";
					return (*i).edge;
				}
				if(count<=1)
				{
					//cout<<"\n %"<<(*i).edge<<"% \n";
					return (*i).edge;
				}
			}				
		}
			
	}
	void displayVisited()
	{
		cout<<visited;
	}
	int returnVisited()
	{
		return visited;
	}
	void updateVisited(int v)
	{
		visited =v;
	}
	vector<edge_marker> return_output()
	{
			return output;
	}
	vector<edge_marker> return_input()
	{
			return input;
	}
	int return_dfs_mark()
	{
		return dfs_mark;
	}
	void update_dfs_mark(int m)
	{
		dfs_mark=m;
	}
	void update_input(edge_marker vi)
	{
		input.push_back(vi);
	}
	void delSelfLoop()
	{
		for(auto i=output.begin();i!=output.end();i++)
		{
			if((*i).edge=="OP-")
			{
			//	output.erase(output.begin());
			}

		}
		for(auto i=input.begin();i!=input.end();i++)
		{
			if((*i).edge=="OP-")
			{
			//	input.erase(input.begin());
			}
		}
	}
	void update_Outedge()
	{
		for(auto i=output.begin();i!=output.end();i++)
		{
			if((*i).edge=="OP+")
				(*i).edge="(1/(1+A))";
		}
	}
	void update_Inedge()
	{
		for(auto i=input.begin();i!=input.end();i++)
		{
			if((*i).edge=="OP+")
				(*i).edge="(1/(1+A))";
		}
	}

	void update_output(edge_marker vo)
	{
		output.push_back(vo);
	}
	int updateFlag(int f)
	{
		flag=f;
	}
	void displayFlag()
	{
		cout<<"\tFLAG = "<<flag;
	}
	void displayInputs()
	{
		if(!input.empty())
		{
			cout<<"\nINPUTS";
			for(auto i=input.begin();i!=input.end();i++)
			{
				cout<<"\n"<<(*i).node<<"\t"<<(*i).edge;
			}	
		}
		else
			cout<<"\nNO INPUTS";
	}
	void displayOutputs()
	{
		if(!output.empty())
		{
			cout<<"\nOUTPUTS";
			for(auto i=output.begin();i!=output.end();i++)
			{
				cout<<"\n"<<(*i).node<<"\t"<<(*i).edge;
			}	
		}
		else
			cout<<"\nNO OUTPUTS";
	}
	int getFlag()
	{
		return flag;
	}
	void update_io()
	{
		if(input.size()>1)
		{
			if(input[0].edge==input[1].edge)
			{
				edge_marker E =input[0];
				input.clear();
				input.push_back(E);
			}
		}
		if(output.size()>1)
		{	if(output[0].edge==output[1].edge)
			{
				edge_marker E =output[0];
				output.clear();
				output.push_back(E);
			}
		}
	}
};
///////////////////////////////////////////CIRCUIT PARSE - 1/////////////////////////////////////////////////////
void ckt_parse_1(string file_name)
{
	cout<<"\nWORKING";
	std::string line;				//This is the string type LINE that will store the line by line read data from file
	std::ifstream infile(file_name);		
	if(!infile)						//To check for successful opening of file 
	{
		////cout<<"Could not open File : "<<file_name;
		exit(1);
	}
	getline(infile,line);				// Takes one line at a time
	//cout<<"\n"<<line;
	double n=stod(line);
	cout<<"\n n="<<n;
	double count=1;
	while(count<=n)
	{
		
		cout<<"\nCREATING NODES IN THE GRAPH";
		SFG.insert(pair<double, Nodes> (count,Nodes(count,1))); 
		CKT.insert(pair<double, circuit> (count,circuit(count)));
		count++;
	}	
	SFG.insert(pair<double, Nodes> (-100,Nodes(-100,0))); //Vi node
	SFG.insert(pair<double, Nodes> (100,Nodes(100,0)));  //Vo node
	SFG.insert(pair<double, Nodes> (0,Nodes(0,0)));  //Ground node
	CKT.insert(pair<double, circuit> (-100,circuit(-100)));
	CKT.insert(pair<double, circuit> (100,circuit(100)));
	CKT.insert(pair<double, circuit> (0,circuit(0)));
	getline(infile,line);			// Takes one line at a time
	while (infile.good() && line!="END")
	{
		char ch='_';				//Character "("
		int f1=line.find_first_of(ch);		//Find the first occurence of _
		int f2=line.find_last_of(ch);		//Find the last occurence of _
		string source_node=line.substr(0,f1);
		int k=sizeof(line);
		cout<<"\n";
		string edge_weight=line.substr(f1+1,f2-f1-1);
		string destination_node=line.substr(f2+1,k-f2-1);
		//////UPDATE IN CIRCUIT MAP
		double f;
		double t;
		if(source_node == "VI")
		{
			f=-100;
		}
		else
		{
			f=stod(source_node);
		}
		if(destination_node == "VO")
		{
			t=100;
		}
		else
		{
			t=stod(destination_node);
		}
		auto m=CKT.find(f);
		auto n=CKT.find(t);
		m->second.updateBranches(edge_weight);
		n->second.updateBranches(edge_weight);
		///////////////////////////
		//Updating flag of nodes connected to voltage sources	
		if(source_node == "VI")
		{
			double nid=stod(destination_node);
			map<double,Nodes>::iterator i;
			i=SFG.find(nid);	
			i->second.updateFlag(0);
		}		
		if(edge_weight == "OP+"|| edge_weight == "OP-")
		{
			double nid=stod(destination_node);
			map<double,Nodes>::iterator i;
			i=SFG.find(nid);	
			i->second.updateFlag(0);
		}				
		cout<<source_node<<"\t"<<edge_weight<<"\t"<<destination_node;
		//SFG.insert(pair<double, nodes> (t,Graph(t,"INPUT",temp,1,0,0.002,0)));  //For input nodes only	
		getline(infile,line);			// Takes one line at a time
	}
}
void add_magic_source()
{
	for(auto i=SFG.begin();i!=SFG.end();i++)
	{
		//cout<<"\nNODE ID: "<<i->first;
		int f=i->second.getFlag();
		double d=i->first;
		if(f==1)
		{
			SFG.insert(pair<double, Nodes> (-d,Nodes(-d,1))); 
		}
	}
}

void update_input_output();
void update_self_loops();
vector<double> specialNodes;
///////////////////////////////////////////CIRCUIT PARSE - 2/////////////////////////////////////////////////////
void ckt_parse_2(string file_name)
{
	cout<<"\nWORKING";
	std::string line;				//This is the string type LINE that will store the line by line read data from file
	std::ifstream infile(file_name);		
	if(!infile)					//To check for successful opening of file 
	{
		////cout<<"Could not open File : "<<file_name;
		exit(1);
	}
	getline(infile,line);
	int count=0;
	getline(infile,line);			// Takes one line at a time
	while (infile.good() && line!="END")
	{
		
		//cout<<"\nCOUNT= "<<count;
		//cout<<"\n"<<line;
		char ch='_';				//Character "("
		int f1=line.find_first_of(ch);		//Find the first occurence of _
		int f2=line.find_last_of(ch);		//Find the last occurence of _
		string source_node=line.substr(0,f1);
		int k=sizeof(line);
		cout<<"\n";
		string edge_weight=line.substr(f1+1,f2-f1-1);
		if(edge_weight.rfind("C",0)==0)
		{
			edge_weight="s"+edge_weight;			
		}
		string destination_node=line.substr(f2+1,k-f2-1);
		cout<<source_node<<"\t"<<edge_weight<<"\t"<<destination_node;
		if(count==0)		//For 2nd line in input file
		{
			auto i=SFG.find(-100);
			double d=stod(destination_node);
			i->second.update_output(edge_marker(d,edge_weight));
			auto j=SFG.find(d);
			j->second.update_input(edge_marker(-100,edge_weight));
		}
		if(count==1)		//For 3rd line in input file
		{
			auto i=SFG.find(100);
			double d=stod(source_node);
			i->second.update_input(edge_marker(d,edge_weight));
			auto j=SFG.find(d);
			j->second.update_output(edge_marker(100,edge_weight));
		}
		else if(count>1 & destination_node!="0")
		{
			double s=stod(source_node);
			//cout<<"\n***************"<<destination_node;
			double d=stod(destination_node);
			if(s==d)
			{
				specialNodes.push_back(s);
			}
			else
			{	
				int flags;
				int flagd;
				auto i=SFG.find(s);
				flags=i->second.getFlag();
				auto j=SFG.find(d);
				flagd=j->second.getFlag();
				if(flags==0 && flagd==0)
				{
					i->second.update_output(edge_marker(d,edge_weight));
					j->second.update_input(edge_marker(s,edge_weight));
				}
				else if(flags ==0 && flagd==1)
				{
					i->second.update_output(edge_marker(-d,edge_weight));
					auto k= SFG.find(-d);
					k->second.update_input(edge_marker(s,edge_weight));
					auto l=CKT.find(d);
					string str=l->second.returnDPImp();
					k->second.update_output(edge_marker(d,str));
					j->second.update_input(edge_marker(-d,str));	//DRIVING POINT ADMITTANCE
				}
				else if(flags ==1 && flagd==0)
				{
					if(edge_weight=="OP+" || edge_weight =="OP-")
					{
						auto l=CKT.find(s);
						string str=l->second.returnDPImp();
						i->second.update_input(edge_marker(-s,str));
						auto k= SFG.find(-s);
						k->second.update_output(edge_marker(s,str));
						j->second.update_input(edge_marker(s,edge_weight));	//DRIVING POINT ADMITTANCE
						i->second.update_output(edge_marker(d,edge_weight));
						
					}
					else
					{
						auto l=CKT.find(s);
						string str=l->second.returnDPImp();
						i->second.update_input(edge_marker(-s,str));
						auto k= SFG.find(-s);
						k->second.update_output(edge_marker(s,str));
						j->second.update_output(edge_marker(s,edge_weight));	//DRIVING POINT ADMITTANCE
						k->second.update_input(edge_marker(d,edge_weight));
					}				
				}
				else if(flags ==1 && flagd==1)
				{
					auto ls=CKT.find(s);
					auto ld=CKT.find(d);
					string strs=ls->second.returnDPImp();
					string strd=ld->second.returnDPImp();
					auto k= SFG.find(-s);
					auto m= SFG.find(-d);
					i->second.update_output(edge_marker(-d,edge_weight));
					i->second.update_input(edge_marker(-s,strs));
					k->second.update_output(edge_marker(s,strs));
					k->second.update_input(edge_marker(d,edge_weight));
					m->second.update_output(edge_marker(d,strd));
					m->second.update_input(edge_marker(s,edge_weight));
					j->second.update_input(edge_marker(-d,strd));
					j->second.update_output(edge_marker(-s,edge_weight));	//DRIVING POINT ADMITTANCE
				}
			}
		}
		getline(infile,line);			// Takes one line at a time
		count++;
		
	}
	update_input_output();
	if(!specialNodes.empty())
		update_self_loops();
}
//////////////////////////GETTING RID OF UNITY GAIN FEEDBACK///////////////////////////////
void update_self_loops()
{
	for(auto i=specialNodes.begin();i!=specialNodes.end();i++)
	{
		cout<<"\nUPDATING RESULTS OF SELF LOOP NODE :"<<*i;
		auto j=SFG.find(*i);
		vector<edge_marker> inputs=j->second.return_input();
		vector<edge_marker> outputs=j->second.return_output();
		double s=inputs[0].node;
		if(s==(*i))		
			s=inputs[1].node;		
		auto k= SFG.find(s);
		k->second.update_Outedge();
		j->second.update_Inedge();
		j->second.delSelfLoop();
	}
}

//////////////////////////////FINDING FORWARD PATHS////////////////////////////////////////
vector<double> Current_Path;
vector<vector<double>> All_Path;
void dfs(double x, double y);
void find_paths()
{
	dfs(-100,100);
	//cout<<"\nFORWARD PATHS DISPLAYED BELOW";
	for(auto i=All_Path.begin();i!=All_Path.end();i++)
	{
		//cout<<"\n";
		for(auto j=(*i).begin();j!=(*i).end();j++)
		{
		//	cout<<(*j)<<"\t";
		}
	}
}
void dfs(double x, double y)
{
	auto i= SFG.find(x);
	int m=i->second.return_dfs_mark();
	if(m==1)
		return;
	i->second.update_dfs_mark(1);
	Current_Path.push_back(i->first);
	if(x==y)
	{
		All_Path.push_back(Current_Path);
		auto i= SFG.find(x);
		i->second.update_dfs_mark(0);
		Current_Path.pop_back();
		return;
	}	
	vector<edge_marker> next=i->second.return_output();
	for(auto k=next.begin();k!=next.end();k++)
	{
		dfs((*k).node,y);
	}
	Current_Path.pop_back();
	i->second.update_dfs_mark(0);
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////FINDING CYCLES//////////////////////////////////////
vector<vector<double>> All_Loop;
void printCycle(stack<double> st, double n);
void processDFStree(stack<double> s);
void findCycles()
{
	//for(auto i=SFG.begin();i!=SFG.end();i++)
	{	
	//	if(i->first!=100)
		{
			//cout<<"\nSTARTING FROM NODE "<<i->first;
	//		int v=i->second.returnVisited();
	//		if(v==0)
			{
				auto i=SFG.find(-100);
				stack<double> sta;
				sta.push(i->first);
				i->second.updateVisited(1);
				processDFStree(sta);
			}
		}
	}
}
void processDFStree(stack<double> s)
{
	double n=s.top();
	auto i=SFG.find(n);
	vector<edge_marker> VO=i->second.return_output();
	for(auto j=VO.begin();j!=VO.end();j++)
	{
		if((*j).node!=100)	
		{	auto k=SFG.find((*j).node);
			int v=k->second.returnVisited();
			if(v==1)
			{
				printCycle(s,(*j).node);
			}
			else
			{
				s.push((*j).node);
				k->second.updateVisited(1);  
				processDFStree(s);
				s.pop();
			}
		}
	}
	//cout<<"\nUPDATING FLAG OF COMPLETED NODE : "<<i->first;
	i->second.updateVisited(10);	//10 means DONE
	s.pop();
}
void printCycle(stack<double> st, double n)
{
	
	//cout<<"\nPRINTING CYCLE";
	stack<double> sta2;
	sta2.push(st.top());
	st.pop();
	while(sta2.top()!=n)
	{
		sta2.push(st.top());
		st.pop();
	}
	//sta2.push(st.top());
	//st.pop();
	vector <double> D;
	while(!sta2.empty())
	{
		D.push_back(sta2.top());
		//cout<<"\t"<<sta2.top();
		st.push(sta2.top());
		sta2.pop();
	}
	All_Loop.push_back(D);
	D.clear();
	//cout<<"\nFINISHED ONE CYCLE";
}




////////////////////////////////////////////////////////////////////////////////////////////
void update_input_output()
{
	for(auto i=SFG.begin();i!=SFG.end();i++)
	{
		i->second.update_io();
	}
}		
void display_graph()
{
	for(auto i=SFG.begin();i!=SFG.end();i++)
	{
		cout<<"\n*****************************************************";
		cout<<"\nNODE ID: "<<i->first;
		i->second.displayFlag();
		cout<<"\n";
		i->second.displayInputs();
		cout<<"\n";
		i->second.displayOutputs();
	}
}
void display_circuit()
{
	cout<<"\nCHECK HERE FOR CIRCUIT DETAILS";
	for(auto i=CKT.begin();i!=CKT.end();i++)
	{
		cout<<"\nNODE ID: "<<i->first;
		i->second.displayBranches();
	}
}
void display_flags()
{
	for(auto i=SFG.begin();i!=SFG.end();i++)
	{
		cout<<"\nVISITED FLAG of"<<i->first<<" ";
		i->second.displayVisited();
			
	} 
}
void displayCycleVisited()
{
	for(auto i=SFG.begin();i!=SFG.end();i++)
	{
		int j=i->second.returnVisited();
		cout<<"\n"<<i->first<<"\t"<<j;

	}
}
//Calculating Forward paths along with their gains
vector <string> ForwardGains;
void displayForwardPaths()
{
	for(auto i=All_Path.begin();i!=All_Path.end();i++)
	{
		string s;
		cout<<"\nDISPLAYING FORWARD PATH : ";
		for(auto j=(*i).begin();j!=(*i).end();j++)
		{
			auto k=SFG.find(*j);
			cout<<(*j)<<"\t";
			string s2;
			s2=k->second.returnSpecialEdge(*(j+1));
			if(s2!="1" && s2!="\0")
			{
				s=s+s2+".";
			}
		}
		ForwardGains.push_back(s);
		cout<<"\n"<<"GAIN OF THIS FORWARD PATH IS = ("<<s<<")";
	}
	
}
int checkLoopGainSimilarity(string S1, string S2)
{
	//cout<<"\nCALLED";
	istringstream s1(S1);
	istringstream s2(S2);
	vector<string> x1;
	vector<string> x2;
	string m1,m2;
	while (std::getline(s1, m1, '.')) 
	{
		if (!m1.empty())
			x1.push_back(m1);
	}
	while (std::getline(s2, m2, '.')) 
	{
		if (!m2.empty())
			x2.push_back(m2);
	}
	sort(x1.begin(),x1.end());
	sort(x2.begin(),x2.end());
	if(x1==x2)
	{
	//	cout<<"\nSIMILAR LOOP";	
		return 1;
	}
	else
	{
	//	cout<<"\nDISSIMILAR LOOP";	
		return 0;
	}
}
vector <string> LoopGains;
vector <vector <double>> FinalLoops;
vector <string> Temp;
namespace rmv
{
    struct remove
    {
        string key;
        remove(string key): key(key) {}
 
        bool operator()(string const &i) {
            return i == key;
        }
    };
}
void displayLoops()
{

	if(!All_Loop.empty())
	{
		//FinalLoops=All_Loop;

		for(auto j=All_Loop.begin();j<All_Loop.end();j++)
		{
			cout<<"\nDISPLAYING LOOPS BEFORE ANY OPERATION";
			for(auto l=(*j).begin();l!=(*j).end();l++)
			{
				cout<<"\t"<<*l;
			}
		}

		
		for(auto i=All_Loop.begin();i!=All_Loop.end();i++)
		{
			string s;
			//cout<<"\n";
			for(auto j=(*i).begin();j<(*i).end();j++)
			{
				auto k=SFG.find(*j);
				//cout<<(*j)<<"\t";
				string s2;
				if(j<((*i).end()-1))	
				{
					s2=k->second.returnSpecialEdge(*(j+1));
				}
				else
				{
					auto m=(*i).begin();
					s2=k->second.returnSpecialEdge((*m));
				}
				if(s2!="1")
				{
					s=s+s2+".";
				}
			}
			LoopGains.push_back(s);
		}
		Temp=LoopGains;
		for (int i=0;i<Temp.size()-1;i++)
		{
			int s=0;
			for(int j=i+1;j<Temp.size();j++)
			{				
				s=checkLoopGainSimilarity(Temp[i], Temp[j]);
				cout<<"\nS ="<<s;
				if(s==1)
				{
					//Temp.erase(Temp.begin()+j);
					LoopGains[j]="0";
					//FinalLoops.push_back(All_Loop[i]);
					All_Loop[j]={0};
				}
				
			}	
		}
		LoopGains.erase(remove_if(LoopGains.begin(),LoopGains.end(),rmv::remove("0")),LoopGains.end());
		for(auto j=All_Loop.begin();j<All_Loop.end();j++)
		{
			int flag=0;
			for(auto l=(*j).begin();l!=(*j).end();l++)
			{
				if((*l)!=0)
					flag=1;
			}
			if(flag==1)
			{
				FinalLoops.push_back((*j));
			}
		}
		for(auto j=FinalLoops.begin();j<FinalLoops.end();j++)
		{
			cout<<"\nDISPLAYING LOOPS AFTER OPERATIONS";
			for(auto l=(*j).begin();l!=(*j).end();l++)
			{
				cout<<"\t"<<*l;
			}
		}

		for(auto i=LoopGains.begin();i!=LoopGains.end();i++)
		{
			cout<<"\n"<<*i;
		}
	}
	else
	{
		cout<<"\nNO LOOPS IN THIS CIRCUIT";
	}
}

///////////////////////////////OPERATIONS FOR TRANSFER FUNCTION CALCULATION//////////////////////////
string Delta="1 - {";
string P;
string D;
string num;
string din;
void checkCommonNode(vector<vector<double>> Z, int r,int data[], vector<vector<double>> FinalL, vector<string> LGains, int mode)
{
	int flag1=0,flag2=0,flag3=0;
	cout<<"\nPATHS / LOOPS RECEIVED FOR FINDING COMMON NODES : \n";
	for(auto g=Z.begin();g!=Z.end();g++)
	{
		for(auto h=(*g).begin();h!=(*g).end();h++)
		{
			cout<<"\t"<<*h;
		}
		cout<<"\n";
	}
	
	for(auto i=Z[0].begin();i!=Z[0].end();i++)
	{
		double d=(*i);
		for(auto j=Z.begin()+1;j!=Z.end();j++)
		{
			flag1=0;
			for(auto k=(*j).begin();k!=(*j).end();k++)
			{
				if((*k)==d)
				{
					flag1=1;
					break;
				}					
			}
			if(flag1==1)
			{
				break;
			}
		}
		if(flag1==1)
		{
			flag2=1;
			break;
		}
	}
	if(flag2==1)
	{
		cout<<"\nTHE LOOPS TOUCH EACH OTHER\n";
		flag2=0;
		//return 1;
	}
	else if(flag2!=1)
	{
		int size=LGains.size();
		cout<<"\n\n\n"<<size<<"\n\n\n";
		string s;
		cout<<"\nNON TOUCHING LOOPS";
		for(int i=0;i<size;i++)
		{
			cout<<"\n"<<LGains[i];

			s=s+"x("+LGains[i]+")";	
		}
		cout<<"\nDELTA UPDATED";
		if(mode==1)
		{	
			if(r%2==0)
				Delta=Delta+"+ {"+s+"} ";
			else 
				Delta=Delta+"- {"+s+"} ";
		}
		else if(mode==2)
		{
			if(r%2==0)
				D=D+"+ {"+s+"} ";
			else 
				D=D+"- {"+s+"} ";
		}		
	}
}



void CheckCombination(int Loops[], int data[], int start, int end, int index, int r, vector<vector<double>> FinalL, vector<string> LGains, int mode)
{
	if(index==r)
	{	vector<string> L;
		vector<vector<double>> Z;
		for(int j=0;j<r;j++)
		{
			cout<<data[j]<<" ** LOOK HERE";
			Z.push_back(FinalL[data[j]]);
			L.push_back(LGains[data[j]]);
		}
		cout<<"\n";
		checkCommonNode(Z,r,data, FinalL, L, mode);
		//cout<<"\nM = "<<m<<"\n";
		Z.clear();
		L.clear();
	}
	for(int i=start;i<=end && (end-i+1)>=(r-index);i++)
	{
		data[index]=Loops[i];
		CheckCombination(Loops,data,i+1,end,index+1,r, FinalL, LGains, mode);
	}
}
void findCombinations(int Loops[], int n, int r, vector<vector<double>> FinalL, vector<string> LGains, int mode)
{
	int data[n];
	CheckCombination(Loops, data, 0,n-1,0 ,r, FinalL, LGains, mode);
}

void findDELTA()
{
	cout<<"\n NO. OF FORWARD PATHS : "<<All_Path.size();
	cout<<"\n NO. OF FORWARD PATH GAINS : "<<ForwardGains.size();
	cout<<"\n NO. OF LOOPS : "<<FinalLoops.size();
	cout<<"\n NO. OF LOOP GAINS : "<<LoopGains.size();
	////////////////CALCULATION OF DELTA (DINOMINATOR OF TF)/////////////////////////////
	
	for(auto i=LoopGains.begin();i!=LoopGains.end();i++)
	{
		if(i!=LoopGains.end()-1)
			Delta=Delta+" ("+(*i)+") "+"+";
		else
			Delta=Delta+" ("+(*i)+") ";
	}
	Delta=Delta+"}";
	//cout<<"\n DELTA = "<<Delta;
	int n=FinalLoops.size();
	int *arr = new int(n);
	for(int i=0;i<n;i++)
	{
		arr[i]=i;
	}
	int r=2;
	while(r<=n)
	{
		findCombinations(arr,n,r,FinalLoops, LoopGains, 1);
		r++;
	}
}
void findNumerator()
{
	cout<<"\nFINDING THE NUMERATOR OF THE TRANSFER FUNCTION \n";
	vector<vector<double>> Loops=FinalLoops;
	vector<string> Gains=LoopGains;
	vector<vector<double>> SelectedLoops=Loops;
	vector<string> SelectedLoopGains=Gains;
	int flag1=0,flag2=0;
	auto f=ForwardGains.begin();
	auto lpointer=Loops.begin();
	int ind;
	string X;
	int count=0;
	for(auto i=All_Path.begin();i!=All_Path.end();i++)
	{
		X=(*f);
		for(auto j=(*i).begin();j!=(*i).end();j++)
		{
			double d=(*j);
			for(auto m=Loops.begin();m!=Loops.end();m++)
			{
				for(auto n=(*m).begin();n!=(*m).end();n++)
				{
					if((*n)==d)
					{
						lpointer=m;
						ind=distance(Loops.begin(),m);
						cout<<"\n INDEX = "<<ind;
						flag1=1;
						break;
					}
					if(flag1==1)
						break;	
				}
				if(flag1==1)
					break;	
			}
			if(flag1==1)
			{	
				SelectedLoops.erase(SelectedLoops.begin()+ind-count);
				Loops=SelectedLoops;
				SelectedLoopGains.erase(SelectedLoopGains.begin()+ind-count);
				Gains=SelectedLoopGains;
				flag1=0;
			}
		}
		int n=SelectedLoops.size();
		int *arr = new int(n);
		for(int i=0;i<n;i++)
		{
			arr[i]=i;
		}
		int r=2;
		while(r<=n)
		{
			findCombinations(arr,n,r,SelectedLoops, SelectedLoopGains, 2);
			r++;
		}
		num=num+X+D+"+";
		D.clear();
		f++;
	}
		
}
int main()
{	
	time_t my_time=time(NULL);
	int c=3;
	string s;
	if(c==1)
		s="Sallen_Key.isc";
	else if(c==2)
		s="TowThomasBQ.isc";
	else if(c==3)
		s="TwinTNotch.isc";
	else if(c==4)
		s="Boctor_LPNotch.isc";
	else if(c==5)
		s="1stRCLPF.isc";
	else if(c==6)
		s="MultiFBBP.isc";
	else if(c==7)
		s="TESTCIRCUIT.isc";
	ckt_parse_1(s);
	add_magic_source();
	ckt_parse_2(s);
	cout<<"\n**************************************************";
	display_circuit();
	cout<<"\n**************************************************";
	display_graph();
	//display_flags();
	find_paths();
	findCycles();
	//displayCycleVisited();
	displayForwardPaths();
	displayLoops();
	findDELTA();
	findNumerator();
	cout<<"\nDELTA CALCULATED = "<<Delta;
	cout<<"\nNUMERATOR OF TF : "<<num;
	cout<<"\n*******************************************";
	cout<<"\n\n\nTRANSFER FUNCTION OF THE CIRCUIT IS ";
	cout<<"\n\n"<<num;
	cout<<"\n____________________________________________________________________________________________________________________________";
	cout<<"\n\n"<<Delta<<"\n";
	cout<<ctime(&my_time);
	return 0;
}
