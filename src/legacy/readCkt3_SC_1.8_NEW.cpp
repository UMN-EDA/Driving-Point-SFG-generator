//////////////////TRANSISTOR LEVEL CIRCUITS//////////////////////////////////////////


//////////////////////////ONLY FOR MULTIPHASE CIRCUITS//////////////////////////////////////
#include <iostream>  // basic C++ input/output (e.g., cin, //cout)
#include <fstream>	// needed to open files in C++
#include <sstream>   // needed if you are using sstream (C++)
#include <stdio.h>	// needed to open files in C
#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>
#include <string>
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
struct node_switching
{
	string node;
	string mod_node;
	node_switching(string s1, string s2)
	{
		node=s1;
		mod_node=s2;
	}
};
vector<vector<node_switching>> VNODES;
vector<node_switching> VND;

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
void display_NodeID_Tree();
struct Transistor
{
	string ID;
	string gate;
	string drain;
	string source;
	vector<string> neighbours;
	Transistor(string I,string g,string d, string s)
	{
		ID=I;
		gate=g;
		drain=d;
		source=s;
	}
	int if_exists_neighbour(string n)
	{
		int flag=0;
		for(auto i = neighbours.begin();i!=neighbours.end();i++)
		{
			if((*i)==n)
			{
				flag=1;
				break;
			}
		}
		if(flag==1)
			return 1;
		else
			return 0;
	}
	string returnNodeType(string n)
	{
		if(n==gate and n==drain)
			return "gateDrain";
		else if(n==drain)
			return "drain";
		else if(n==source)
			return "source";
		else if(n==gate)
			return "gate";
		else
			return "Not Found";
	}
	void updateNeighbours(string n)
	{
		neighbours.push_back(n);
	}
	string returnGate()
	{
		return gate;
	}
	string returnDrain()
	{
		return drain;
	}
	string returnSource()
	{
		return source;
	}
	string returnId()
	{
		return ID;
	}
	void displayTrans()
	{
		cout<<"\nTRANSISTOR : "<<ID;
		cout<<"\nGATE : "<<gate;
		cout<<"\nDRAIN : "<<drain;
		cout<<"\nSOURCE : "<<source;
		cout<<"\nCONNECTED TO TRANSISTORS : ";
		for(auto i= neighbours.begin();i!=neighbours.end();i++)
			cout<<(*i)<<" ";
	}
};
vector <Transistor> TRANS;
struct TNodes
{
	string Nodes;
	vector <string> ConTrans;
	TNodes(string N)
	{
		Nodes=N;
	}
	void updateConTrans(string M)
	{
		ConTrans.push_back(M);
	}
};
vector <string> TransNodes;
vector <TNodes> DictNodes;
vector <vector<Transistor>> ALLTRANS;
struct sfg_edge_marker
{
	string node;
	string edge;
	double taken;
	sfg_edge_marker(string a, string b)	
	{
		node=a;
		edge=b;
		taken=0;
	}
};

class circuit{
	double Node;
	string Phase; //Stores ID for phase (0 for Combined SFG)	
	//string type; // CAN BE "I" means input node, "O", output node, "N", neutral node
	vector<string> branches;
public:
	circuit(double n, string P)
	{
		Node=n;
		Phase=P;
	}
	void updatePhase(string P)
	{
		Phase=P;
	}
	string returnPhase()
	{
		return Phase;
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
			branches.push_back(b);
			
		}
	}
	void  update_sub_branch(vector<string> b)
	{
		////TO BE UPDATED ACCORDING TO SWITCH POSITION
		for(auto j=b.begin();j!=b.end();j++)
		{
			if((*j)[0]!='S')
				branches.push_back(*j);	
		}
	}
	double returnNode()
	{
		return Node;
	}
	void displayNode()
	{
		cout<<"\n"<<Node;
	}
	void displayPhase()
	{
		cout<<"\nPHASE = "<<Phase;
	}
	void displayBranches()
	{
		//cout<<"\n";
		for(auto i=branches.begin();i!=branches.end();i++)
		{
			cout<<"\t"<<*i;
		}
	}
	vector<string> return_branches()
	{
		return branches;
	}
	string returnDPImp()
	{
		string s;
		cout<<"\nCALLED DP ELASTANCE";
		for(auto i=branches.begin();i!=branches.end();i++)
		{
			cout<<"\n"<<(*i);
			string g=(*i);
			if(g.at(0)=='C')
				s=s+"+s."+(g);
			else
				s=s+"+"+(g);
		}
		s.erase(0,1);
		string st="1/("+s+")";
		return st;
		
	}
};
map<double,circuit> CKT;
vector<double> V;
class SFGNodes;
class Nodes{
	double NodeID;
	int flag;	//Flag is used to distinguish nodes which are not connected to any voltage sources
			//If connected to voltage source flag=0 else 1
	string Phase; //Stores ID for phase (0 for Combined SFG)
	vector<edge_marker> input;		
	vector<edge_marker> output;
	int dfs_mark;
	int visited;
public:
	Nodes(double ID, int F, string P)
	{
		NodeID=ID;
		flag=F;
		Phase=P;
		dfs_mark=0;
		V.push_back(NodeID);
		visited=0;
	}
	void displayPhase()
	{
		cout<<"\nPHASE = "<<Phase;
	}
	void updatePhase(string P)
	{
		Phase=P;
	}
	string returnPhase()
	{
		return Phase;
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
	void update_io()	//Get rid of duplicates
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

class SFGNodes{
	string NodeID;
	int flag;	//Flag is used to distinguish nodes which are not connected to any voltage sources
			//If connected to voltage source flag=0 else 1
	string Phase; //Stores ID for phase (0 for Combined SFG)
	vector<sfg_edge_marker> input;		
	vector<sfg_edge_marker> output;
	int magic_Elastance;
	int dfs_mark;
	int visited;
public:
	SFGNodes(string ID, int F, string P)
	{
		NodeID=ID;
		flag=F;
		Phase=P;
		dfs_mark=0;
		//V.push_back(NodeID);
		visited=0;
		magic_Elastance=0;
	}
	void remove_duplicate_nodes()
	{
		sort(input.begin(), input.end(), [](const sfg_edge_marker& a, const sfg_edge_marker& b) {
		    return a.node < b.node;
		});
		input.erase(std::unique(input.begin(), input.end(), [](const sfg_edge_marker& a, const sfg_edge_marker& b) {
		    return a.node == b.node;
		}), input.end());

		sort(output.begin(), output.end(), [](const sfg_edge_marker& a, const sfg_edge_marker& b) {
		    return a.node < b.node;
		});
		output.erase(std::unique(output.begin(), output.end(), [](const sfg_edge_marker& a, const sfg_edge_marker& b) {
		    return a.node == b.node;
		}), output.end());

	}
	void combine_edges_from_same_node()
	{
		//First combining input edges
		//Only if it has more than 1 input branch
		if(input.size()>1)
		{
			//Fix one node
			string tempNode = input[0].node;
			//Magic node ID
			string magNode = "-"+NodeID;
			//If its coming from its magic node
			if(tempNode == magNode)		
			{
				string dp_imp;	//String to add up all imp/adm
				for(auto i = input.begin()+1;i!=input.end();i++)
				{
					if((*i).node == tempNode)
					{
						if((*i).edge.at(0)=='1')
						{
							dp_imp = dp_imp + "+" + (*i).edge.substr(3,(*i).edge.length()-1);
							dp_imp.pop_back();
						}
						else
							dp_imp = dp_imp + "+" + (*i).edge;
					}
					
				}
				for(auto j = input.begin();j!=input.end();j++)
				{
					if((*j).node == tempNode)
					{
						(*j).edge.pop_back();
						(*j).edge += dp_imp;
						dp_imp.clear();
						(*j).edge += ")";
					}
				}
				
			}
		}
		//Second combining output edges
		//Only if it has more than 1 output branch
		if(output.size()>1)
		{
			//Fix one node
			string tempNode = output[0].node;
			//Magic node ID
			string magNode = NodeID.substr(1);
			//If its coming from its magic node
			if(tempNode == magNode)		
			{
				string dp_imp;	//String to add up all imp/adm
				for(auto i = output.begin()+1;i!=output.end();i++)
				{
					if((*i).node == tempNode)
					{
						if((*i).edge.at(0)=='1')
						{
							dp_imp = dp_imp + "+" + (*i).edge.substr(3,(*i).edge.length()-1);
							dp_imp.pop_back();
						}
						else 
							dp_imp = dp_imp + "+" + (*i).edge;
					}
					
				}
				for(auto j = output.begin();j!=output.end();j++)
				{
					if((*j).node == tempNode)
					{
						(*j).edge.pop_back();
						(*j).edge += dp_imp;
						dp_imp.clear();
						(*j).edge += ")";
					}
				}
				
			}
		}

	}
	vector<sfg_edge_marker> returnInputs()
	{
		return input;
	}
	vector<sfg_edge_marker> returnOutputs()
	{
		return output;
	}
	void dis_node()
	{
		cout<<"\n\n\n NODE : "<<NodeID;
	}
	void clear_input()
	{
		input.clear();
	}
	void clear_output()
	{
		output.clear();
	}
	void update_magic_elastance(int n)
	{
		magic_Elastance=n;
	}	
	int return_magic_elastance()
	{
		return magic_Elastance;
	}
	void displayPhase()
	{
		cout<<"\nPHASE = "<<Phase;
	}
	void updatePhase(string P)
	{
		Phase=P;
	}
	string returnPhase()
	{
		return Phase;
	}
	string returnSpecialEdge(string d)
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
	vector<sfg_edge_marker> return_output()
	{
			return output;
	}
	vector<sfg_edge_marker> return_input()
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
	void update_input(sfg_edge_marker vi)
	{
		input.push_back(vi);
	}
	void remove_input_branch(string s)
	{
		for(auto i=input.begin();i!=input.end();i++)
		{
			if((*i).node==s)
				input.erase(i);
		}
	}
	void modify_output(string s1, string s2)
	{
		for(auto i=output.begin();i!=output.end();i++)
		{
			if((*i).node==s1)
				(*i).node=s2;
		}
	}
	void modify_input(string s1, string s2)
	{
		for(auto i=input.begin();i!=input.end();i++)
		{
			if((*i).node==s1)
				(*i).node=s2;
		}
	}
	void remove_output_branch(string s)
	{
		for(auto i=output.begin();i!=output.end();i++)
		{
			if((*i).node==s)
				output.erase(i);
		}
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

	void update_output(sfg_edge_marker vo)
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
	void update_io()	//Get rid of duplicates
	{
		if(input.size()>1)
		{
			if(input[0].edge==input[1].edge)
			{
				sfg_edge_marker E =input[0];
				input.clear();
				input.push_back(E);
			}
		}
		if(output.size()>1)
		{	if(output[0].edge==output[1].edge)
			{
				sfg_edge_marker E =output[0];
				output.clear();
				output.push_back(E);
			}
		}
	}
};

int phases;
void subcircuit(string);
///////////////////////////////////////////CIRCUIT PARSE PHASE- 1/////////////////////////////////////////////////////
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
	getline(infile,line);				//Reads first line
	//cout<<"\n"<<line;
	int n1=stoi(line);				//Convert string to double (n1 stores no. of phases)
	phases=n1;					//Stores no. of phases in a global variable
	getline(infile,line);				//Reads next line
	double n=stod(line);				//Convert string to double (n stores no. of nodes)
	cout<<"\n n="<<n;				//Display no. of nodes in the circuit
	double count=1;
	while(count<=n)
	{
		double d=pow(2,count);
		string sd=to_string(d);
		cout<<"\nCREATING NODE IN THE GRAPH: "<<d;
		VND.push_back(node_switching(sd,"-"));
		SFG.insert(pair<double, Nodes> (d,Nodes(d,1,"0"))); //Create a node for the SFG representation (Node IDs in the power of 2)
		CKT.insert(pair<double, circuit> (d,circuit(d,"0")));//Create a node in the "Circuit" class to store each node branches	
		count++;	//Update count variable to update the next node ID
	}
	VNODES.push_back(VND);
	VND.clear();	
	SFG.insert(pair<double, Nodes> (-100,Nodes(-100,0,"0"))); //Create the input (-100) node
	SFG.insert(pair<double, Nodes> (100,Nodes(100,0,"0")));  //Create the output (100) node
	cout<<"\nCREATING INPUT NODE IN THE GRAPH: "<<-100;
	cout<<"\nCREATING OUTPUT NODE IN THE GRAPH: "<<100;
	cout<<"\nCREATING GROUND NODE IN THE GRAPH: "<<1;
	SFG.insert(pair<double, Nodes> (pow(2,0),Nodes(pow(2,0),0,"0")));  //create the ground node
	CKT.insert(pair<double, circuit> (-100,circuit(-100,"0"))); //Create the input node in the Circuit Class to store its details
	CKT.insert(pair<double, circuit> (100,circuit(100,"0"))); //Create the output node in the Circuit Class to store its details
	CKT.insert(pair<double, circuit> (pow(2,0),circuit(pow(2,0),"0"))); //Create the ground node in the Circuit Class to store its details
	getline(infile,line);			// Takes one line at a time
	while (infile.good() && line!="END")
	{
		if(line[0]!='M' && line[0]!='P')	//Lines defining anything other than transistors
		{
			char ch='_';				//Character "("
			int f1=line.find_first_of(ch);		//Find the first occurence of _
			int f2=line.find_last_of(ch);		//Find the last occurence of _
			string source_node=line.substr(0,f1);	//Extracts the source node like 10 as in 10_C2_11
			int k=sizeof(line);			
			cout<<"\n"<<line;
			string edge_weight=line.substr(f1+1,f2-f1-1); //Extracts the edge weight/element like C2 as in 10_C2_11
			string destination_node=line.substr(f2+1,k-f2); //Extracts the deatination node like 11 as in 10_C2_11
			//////UPDATE IN CIRCUIT MAP
			double f;
			double t;
			if(source_node == "VI")
			{
				f=-100;				// If the source node is input node Vi
			}
			else
			{
				f=stod(source_node);		// Converts the extracted source node from string to double date type
				f=pow(2,f);
			}
			if(destination_node == "VO")
			{
				t=100;				// If the destination node is output node Vo
			}
			else
			{
				t=stod(destination_node);	// Converts the extracted destination node from string to double
				t=pow(2,t);
			}
			auto m=CKT.find(f);			//Finds the source node in the map using its ID
			auto n=CKT.find(t);			//Finds the destination node in the map using its ID
			m->second.updateBranches(edge_weight);	//Updates the edge weights corresponding to the source node
			n->second.updateBranches(edge_weight); 	//Updates the edge weights corresponding to the destination node
			///////////////////////////
			//Updating flag of nodes connected to voltage sources	
			if(source_node == "VI")	//The purpose of Flag is to check which nodes are directly connected to voltage source		
			{
				double nid=stod(destination_node);
				map<double,Nodes>::iterator i;
				i=SFG.find(pow(2,nid));	
				i->second.updateFlag(0);	//Since the node is connected to Input node Vi (so flag=0)
			}		
			//if(edge_weight == "OP+"|| edge_weight == "OP-")
			//{
			//	double nid=stod(destination_node);
			//	map<double,Nodes>::iterator i;
			//	i=SFG.find(pow(2,nid));	
			//	i->second.updateFlag(0);	//Since the node is connected to the output of an Opamp (so flag=0)
			//}				
			cout<<source_node<<"\t"<<edge_weight<<"\t"<<destination_node;		//Display nodes and edge weights
			//SFG.insert(pair<double, nodes> (t,Graph(t,"INPUT",temp,1,0,0.002,0)));  //For input nodes only	
			getline(infile,line);			// Takes one line at a time
		}
		else if(line[0]=='M' || line[0]=='P')	//Lines defining the Transistors
		{
			char ch='_';				//Character "("
			int f1=line.find_first_of(ch);		//Find the first occurence of _
			int ucount = 0;
			int f2=line.find(ch,f1+1);			//Find the 2nd occurence of _
			int f3=line.find_last_of(ch);		//Find the last occurence of _
			string tran_name=line.substr(0,f1); //Find the name of transistor
			string gate=line.substr(f1+1,f2-f1-1);	//Extracts the gate node like GG as in MX_GG_DD_SS
			string drain=line.substr(f2+1,f3-f2-1);	//Extracts the drain node like DD as in MX_GG_DD_SS
			int kline=line.length();
			string source=line.substr(f3+1,kline-f3-1);	//Extracts the source node like SS as in MX_GG_DD_SS
			cout<<"\nSOURCE FOUND"<<source;
			cout<<"\nTRANSISTOR FOUND : "<<tran_name<<" GDS "<<gate<<" "<<drain<<" "<<source;
		//	TRANS.push_back(Transistor());
			cout<<"\n"<<line;
		
			getline(infile,line);			// Takes one line at a time
		}
	}
	SFG.clear();
	CKT.clear();
}

////////////////////////////////////////////////FORMATON OF SUBCIRCUIT MAPS////////////////////////////////////////////////////////////////////
vector<map<double, circuit>> CKTX; // vector of maps to store subcircuits SFG
vector<map<double, Nodes>> SFGX; // vector of maps to store subcircuits CKT
map<string, SFGNodes> SFGFINAL; // Map to store final signal flow graph
vector<double> NodeIDs; //Vector to keep track of node ids and avoid duplication of node IDs
vector<double> sfgIDs; //Vector to keep track of actual node ids corresponding to SFG
void add_magic_source();
void subcircuit(string file_name)
{
	//std::ofstream myfile;
	//myfile.open("sub_circuit.txt");
	string stc;
	SFGFINAL.insert(pair<string, SFGNodes> ("-100",SFGNodes("-100",0,"0"))); //Create a node for output node
	NodeIDs.push_back(-100);		
	sfgIDs.push_back(-100);
	SFGFINAL.insert(pair<string, SFGNodes> ("100",SFGNodes("100",0,"0"))); //Create a node for output node
	NodeIDs.push_back(100);		
	sfgIDs.push_back(100);
	SFGFINAL.insert(pair<string, SFGNodes> ("1",SFGNodes("1",0,"0"))); //Create a node for output node
	NodeIDs.push_back(1);		
	sfgIDs.push_back(1);	
	cout<<"\nNO. OF PHASES = %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<phases;
	for(int i=1;i<=phases;i++)
	{
		map<double, circuit> ckt;
		map<double, Nodes> sfg;
		stc=to_string(i);
		stc='S'+stc;		//Keep an eye on the SWITCH ID (S1, S2, S3, ...)/ PHASE ID
		cout<<"\n)))))))))))))))))))"<<stc;
		std::string line;				//This is the string type LINE that will store the line by line read data from file
		std::ifstream infile(file_name);		
		if(!infile)						//To check for successful opening of file 
		{
			////cout<<"Could not open File : "<<file_name;
			exit(1);
		}
		getline(infile,line);				//Reads first line containing no. of phases
		//cout<<"\n"<<line;
		double n1=stod(line);				//Convert string to double (n1 stores no. of phases)
		phases=n1;					//Stores no. of phases in a global variable
		getline(infile,line);				//Reads 2nd line
		double n=stod(line);				//Convert string to double (n stores no. of nodes)//No of Nodes
		cout<<"\n n="<<n;				//Display no. of nodes in the circuit
		double count=1;
		getline(infile,line);				// Takes one line at a time
		sfg.insert(pair<double, Nodes> (-100,Nodes(-100,0,"0"))); //Create a node for input node
		sfg.insert(pair<double, Nodes> (100,Nodes(100,0,"0"))); //Create a node for output node
		ckt.insert(pair<double, circuit> (-100,circuit(-100,stc))); //Create a node for input node in CIRCUIT class
		ckt.insert(pair<double, circuit> (100,circuit(100,stc))); //Create a node for output node in CIRCUIT class	
		sfg.insert(pair<double, Nodes> (1,Nodes(1,0,"0"))); //Create a node for ground node
		ckt.insert(pair<double, circuit> (1,circuit(1,stc))); //Create a node for ground node in CIRCUIT class		
		int control=1; // For debugging purpose
		int line_count=0;
		while (infile.good() && line!="END")		//While loop to iterate over all the lines on text file
		{
			if(line[0]!='M' && line[0]!='P')			//If its not a transistor line
			{
				double snode, dnode;
				char ch='_';				//Character "_"
				int f1=line.find_first_of(ch);		//Find the first occurence of _
				int f2=line.find_last_of(ch);		//Find the last occurence of _
				string source_node=line.substr(0,f1);	//Extracts the source node like 10 as in 10_C2_11
				int k=sizeof(line);			
				cout<<"\n"<<line;
				string edge_weight=line.substr(f1+1,f2-f1-1); //Extracts the edge weight/element like C2 as in 10_C2_11
				string destination_node=line.substr(f2+1,k-f2); //Extracts the deatination node like 11 as in 10_C2_11
				cout<<"\nSOURCE NODE OBTAINED FROM CKT FILE : "<<source_node;
				cout<<"\nEDGE WEIGHT OBTAINED FROM CKT FILE : "<<edge_weight;
				cout<<"\nDESTINATION NODE OBTAINED FROM CKT FILE : "<<destination_node;
				//if(line_count==0)
				//	myfile<<"\nVI_1_";
				if(edge_weight[0]!='S')		//no edge weight other than SWITCHES can start with "S"
				{	
					//////UPDATE IN CIRCUIT MAP
					double f;
					double t;
					if(line_count==0 && destination_node!="0") //Only for the first line of branch connected to VI && If the branch is not connected to the ground
					{
						f=-100;				// If the source node is input node Vi
						t=stod(destination_node); // Converts the extracted destination node from string to double date type
						t=pow(2,t);		//Taking power of 2
						string s11=to_string(t);	//Converting to string
						string s=s11+"_"+stc;	//Appending the phase ID to the string of node ID
						sfg.insert(pair<double, Nodes> (t,Nodes(t,0,stc))); //Create a node for destination node
						SFGFINAL.insert(pair<string, SFGNodes> (s,SFGNodes(s,0,stc))); //Create a node for destination node in Final SFG MAP
						cout<<"\nCHECK HERE FOR DATA ENTERED IN CKT MAP "<<t;
						ckt.insert(pair<double, circuit> (t,circuit(t,stc))); //Create a node for destination node in CIRCUIT class				
						VND.push_back(node_switching(s11, s));		//Vector of nodes
						//VND.push_back(node_switching("-100","-100"));
						auto i=SFGFINAL.find("-100");
						//i->second.update_output(sfg_edge_marker(s,edge_weight));
						auto j=SFGFINAL.find(s);
						//j->second.update_input(sfg_edge_marker("-100",edge_weight));
						//NodeIDs.push_back(t);
						//cout<<"\nT = "<<t <<" & S = "<<s;
						//sfgIDs.push_back(s);
					}
					else if(line_count==1)	//Only for the 2nd line of branch connected to VO
					{
						t=100;				// If the destination node is output node Vo
						f=stod(source_node);		// Converts the extracted source node from string to double date type
						f=pow(2,f);		//Taking power of 2
						string s11=to_string(f);	//Converting to string
						string s=s11+"_"+stc;	//Appending the Phase ID to the Node ID
						sfg.insert(pair<double, Nodes> (f,Nodes(f,1,stc))); //Create a node for source node
						SFGFINAL.insert(pair<string, SFGNodes> (s,SFGNodes(s,1,stc))); //Create a node for source node in Final SFG MAP
						VND.push_back(node_switching(s11, s)); //Pushing it to the vector of nodes
						//VND.push_back(node_switching("100","100"));
						string temp="-"+s;	//Making the Auxilliary Node ID (Negative of ID)
						SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc)));	//Creating the Auzilliary node 
						cout<<"\nCHECK HERE FOR DATA ENTERED IN CKT MAP "<<f;
						ckt.insert(pair<double, circuit> (f,circuit(f,stc))); //Create a node for source node in CIRCUIT class
						auto i=SFGFINAL.find("100");
						//i->second.update_input(sfg_edge_marker(s,edge_weight));
						auto j=SFGFINAL.find(s);
						//j->second.update_output(sfg_edge_marker("100",edge_weight));
						
						//NodeIDs.push_back(100);
						//sfgIDs.push_back(100);
						//NodeIDs.push_back(f);
						//cout<<"\nF = "<<f <<" & S = "<<s;
						//sfgIDs.push_back(s);
					}
					else if(line_count>1)// & destination_node!="0")	//For the rest of the lines of branches
					{
											
						t=stod(destination_node);	// Converts the extracted destination node from string to double
						cout<<"\nDESTINATION NODE CONSIDERED FOR OPERATIONS "<<destination_node;
						t=pow(2,t);	//Taking power of 2 for destination node
						f=stod(source_node);		// Converts the extracted source node from string to double date type
						f=pow(2,f);	//Taking power of 2 for source node
						auto i=sfg.find(f);	
						auto j=sfg.find(t);
						if(i==sfg.end())
						{
							sfg.insert(pair<double, Nodes> (f,Nodes(f,1,stc))); //Create a node for destination node
							cout<<"\nCHECK HERE FOR DATA ENTERED IN CKT MAP "<<f;
							ckt.insert(pair<double, circuit> (f,circuit(f,stc))); //Create a node for destination node in CIRCUIT class				
						}
						if(j==sfg.end())
						{
							sfg.insert(pair<double, Nodes> (t,Nodes(t,1,stc))); //Create a node for destination node
							cout<<"\nCHECK HERE FOR DATA ENTERED IN CKT MAP "<<t;
							ckt.insert(pair<double, circuit> (t,circuit(t,stc))); //Create a node for destination node in CIRCUIT class				
						}
						string s11=to_string(f);
						string s1, s2;
						if(f!=1)
							s1=s11+"_"+stc;	//Appending the phase ID if its not a GROUND NODE
						else
							s1="1";	//gROUND node
						string s22=to_string(t);
						if(t!=1)
							s2=s22+"_"+stc;	//Appending the phase ID if its not a ground node
						else
							s2="1";	//ground node
						auto k=SFGFINAL.find(s1);	//Finding if source node exists in the Graph
						auto l=SFGFINAL.find(s2);	//Finding if destination node exists in the Graph
						if(k==SFGFINAL.end())		//If source node is not present
						{
							SFGFINAL.insert(pair<string, SFGNodes> (s1,SFGNodes(s1,1,stc))); //Create a node for source node in Final SFG MAP
							VND.push_back(node_switching(s11, s1));	//Push the newly created node in the vector of all nodes
							string temp="-"+s1;
							SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc))); 	//Creating magic node (current node) of the newly created node (source)
						}
						if(l==SFGFINAL.end())		//If destination node is not present
						{
							//if(edge_weight == "OP+"|| edge_weight == "OP-")	//If edge weight is OPAMP 
							//{
							//	SFGFINAL.insert(pair<string, SFGNodes> (s2,SFGNodes(s2,0,stc))); //Create a node for destination node in Final SFG
							//	VND.push_back(node_switching(s22, s2));	//Push the newly created destination node in the vector of all nodes
	////CALL THE 5T-OTA FUNCTION MAKE THE CORRESPONDING GRAPH NODES AND EDGES
							//	ota_formation(s1,s2,stc);
							//}
							//else
							{
								SFGFINAL.insert(pair<string, SFGNodes> (s2,SFGNodes(s2,1,stc))); //Create a node for destination node in Final SFG with flag 1
								string temp="-"+s2; //Name for the magic node
								VND.push_back(node_switching(s22, s2)); //Push the destination (not magic node) node to the vector of all nodes
								SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc)));  //Create the magic node
							}
							
						}
						else if(l!=SFGFINAL.end())	//If destination node is already present
						{
							if(edge_weight=="OP+" || edge_weight=="OP-")	//If edge weight is OPAMP
							{	
								l->second.updateFlag(0);	//Update the flag of destination node to 0 if its not...
								string tt="-"+s2;		//Look for the previously created magic node of destination node
								auto ll=SFGFINAL.find(tt);
								if(ll!=SFGFINAL.end())	//If its present					
									SFGFINAL.erase(ll);	//Remove it ..because it doesnt need magic node
	////CALL THE 5T-OTA FUNCTION MAKE THE CORRESPONDING GRAPH NODES AND EDGES
								//ota_formation(s1,s2,stc);
							}
						}
						string ss1,ss2;
						for(auto c=VND.begin();c!=VND.end();c++)
						{
							int n1=s1.find_first_of("_");	//Finding _ in nodeID of Source node
							int n2=s2.find_first_of("_");	//Finding _ in nodeID of Destination node
							string gg=s1.substr(0,n1);		//Finding the number in Node ID i.e. XX.XXXX in XX.XXXX_S1
							string hh=s2.substr(0,n2);		//Finding the number in Node ID i.e. XX.XXXX in XX.XXXX_S1
							if((*c).node==gg)
								ss1=(*c).mod_node;			//Finding the modified Source Node ID
							if((*c).node==hh)	
								ss2=(*c).mod_node;			//Finding the modified Destination Node ID

						}
						auto ii=SFGFINAL.find(ss1);		//Then finding the modified source Node
						auto jj=SFGFINAL.find(ss2);		//Then finding the modified destination Node
						int ni=ss1.find_first_of(".");	//Finding the decimal . in the node ID
						int nj=ss1.find_first_of(".");	//Finding the decimal . in the node ID
						string fsi=ss1.substr(0,ni);	//Extracting the node ID before decimal i.e. XX in XX.YYYY
						string tsj=ss2.substr(0,nj);	//Extracting the node ID before decimal i.e. XX in XX.YYYY
						int flags=ii->second.getFlag();	//Extracts the flag of Source from the map
						int flagd=jj->second.getFlag();	//Extracts the flag of Destination from the map
						//int control=1;
						//if(control==1)
						cout<<"\nFLAG OF SOURCE NODE = "<<flags;
						cout<<"\nFLAG OF DESTINATION NODE = "<<flagd;
						



						//display_NodeID_Tree();

					}
					cout<<"\nUPDATING BRANCHES FOR NODES "<<f<<" & "<<t;
					auto m=ckt.find(f);			//Finds the source node in the map using its ID
					auto n=ckt.find(t);			//Finds the destination node in the map using its ID
					m->second.updateBranches(edge_weight);	//Updates the edge weights corresponding to the source node
					n->second.updateBranches(edge_weight); 	//Updates the edge weights corresponding to the destination node
					///////////////////////////
					//Updating flag of nodes connected to voltage sources			
					//if(edge_weight == "OP+" || edge_weight == "OP-")
					//{
					//	double nid=stod(destination_node);
					//	double d=pow(2,nid);
					//	map<double,Nodes>::iterator i;
					//	string s=to_string(d);
					//	s=s+"_"+stc;
					//	auto j=SFGFINAL.find(s);
					//	j->second.updateFlag(0);
					//	i=sfg.find(d);	
					//	i->second.updateFlag(0);	//Since the node is connected to the output of an Opamp (so flag=0)
					//}				
					cout<<source_node<<"\t"<<edge_weight<<"\t"<<destination_node;		//Display nodes and edge weights
					//SFG.insert(pair<double, nodes> (t,Graph(t,"INPUT",temp,1,0,0.002,0)));  //For input nodes only	
					//getline(infile,line);			// Takes one line at a time
				}
				else if(edge_weight==stc)// && control==0)
				{
					if(destination_node!="0")// && control==0)		//If the destination node is not GROUND
					{
						double s,d;
						s=stod(source_node);		//Converts string to double
						d=stod(destination_node);   //Converts string to double
						double x= pow(2,s)+pow(2,d);	//Add the node ID's when switch is ON
						string snd1=to_string(pow(2,s));
						string snd2=to_string(pow(2,d));
						string tofind_s=snd1;
						string tofind_d=snd2;
						string snd1x=snd1+"-"+stc;					
						string snd2x=snd2+"-"+stc;


						auto sndi1=SFGFINAL.find(snd1x);
						auto sndi2=SFGFINAL.find(snd2x);
						string ID,ID1,ID2, new_node, new_node1, new_node2, mss, mdd;	
						int type=0;
						if(sndi1==SFGFINAL.end() && sndi2!=SFGFINAL.end())
						{
							for(auto i=VND.begin();i!=VND.end();i++)
							{
								if((*i).node==snd1)
								{
									ID=(*i).mod_node;
									break;
								}
							}
							int f=line.find_last_of(".");		//Find the last occurence of _
							new_node=ID.substr(0,f);	//Extracts the source node like 10 as in 10_C2_11			
							//cout<<"\nHHHHHHHHHHHHHHHHHHHHHH "<<new_node;
							double ss=stod(new_node);
							x=ss+pow(2,d);
							type=1;
							tofind_s=ss;
						}
						else if(sndi1!=SFGFINAL.end() && sndi2==SFGFINAL.end())
						{
							
							for(auto i=VND.begin();i!=VND.end();i++)
							{
								if((*i).node==snd2)
								{
									ID=(*i).mod_node;
									break;
								}
							}
							int f=line.find_last_of(".");		//Find the last occurence of _
							new_node=ID.substr(0,f);	//Extracts the source node like 10 as in 10_C2_11			
							//cout<<"\nHHHHHHHHHHHHHHHHHHHHHH "<<new_node;
							double dd=stod(new_node);
							x=dd+pow(2,s);
							type=2;
							tofind_d=dd;
						}
						else if(sndi1==SFGFINAL.end() && sndi2==SFGFINAL.end())
						{
							
							for(auto i=VND.begin();i!=VND.end();i++)
							{
								if((*i).node==snd1)
								{
									ID1=(*i).mod_node;
									cout<<"\nID 1 ="<<ID1<<" CORRESPONDING TO NODE ID "<<(*i).node<<" & SND 1 = "<<snd1;
									break;
								}
							}
							for(auto i=VND.begin();i!=VND.end();i++)
							{
								if((*i).node==snd2)
								{
									ID2=(*i).mod_node;
									cout<<"\nID 2 ="<<ID2<<" CORRESPONDING TO NODE ID "<<(*i).node<<" & SND 2 = "<<snd2;
									break;
								}
							}
							int f=line.find_last_of(".");		//Find the last occurence of _
							new_node1=ID1.substr(0,f);	//Extracts the source node like 10 as in 10_C2_11			
							new_node2=ID2.substr(0,f);	//Extracts the source node like 10 as in 10_C2_11			
							//cout<<"\nHHHHHHHHHHHHHHHHHHHHHH "<<new_node;
							cout<<"\nNEW NODE 1 ="<<new_node1<<" & NEW NODE 2 = "<<new_node2;					
							double ss=stod(new_node1);
							double dd=stod(new_node2);
							x=dd+ss;
							type=3;
							tofind_s=ss;
							tofind_d=dd;
						}
						sfg.insert(pair<double, Nodes> (x,Nodes(x,1,stc))); //Create a node for output node
						ckt.insert(pair<double, circuit> (x,circuit(x,stc))); //Create a node for input node in CIRCUIT class
						string sx1=to_string(x);
						string sx=sx1+"_"+stc;
		
						string news, newd;
						int flag1=0,flag2=0;
						
						for(auto i=VND.begin();i!=VND.end();i++)
						{					
							if((*i).node==snd1)
							{
								news=(*i).mod_node;
								(*i).mod_node=sx;
								cout<<"\nMODIFIED NODE ID OF "<<(*i).node<<" UPDATED TO "<<sx;
								flag1=1;
							}
							if((*i).node==snd2)
							{
								newd=(*i).mod_node;
								(*i).mod_node=sx;
								cout<<"\nMODIFIED NODE ID OF "<<(*i).node<<" UPDATED TO "<<sx;
								flag2=1;
							}
						}
						///SPECIAL OPERATION FOR MULTIPLE SWITCHES COMBINING MULTIPLE NODES
						string old_s,old_d;
						int sp_s=0,sp_d=0;					
						for(auto i=VND.begin();i!=VND.end();i++)
						{
							if((*i).mod_node==ID1)
							{
								sp_s=1;
								old_s=(*i).mod_node;
								(*i).mod_node=sx;
							}
							if((*i).mod_node==ID2)
							{
								sp_d=1;
								old_d=(*i).mod_node;
								(*i).mod_node=sx;
							}
						}	
						if(sp_s==1)	
						{
							auto b=SFGFINAL.find(old_s);
							vector<sfg_edge_marker> in_s=b->second.return_input();
							vector<sfg_edge_marker> out_s=b->second.return_output();
							for(auto is=in_s.begin();is!=in_s.end();is++)
							{
								auto js=SFGFINAL.find((*is).node);
								js->second.modify_output(ID1,sx);
							}
							for(auto is=out_s.begin();is!=out_s.end();is++)
							{
								auto js=SFGFINAL.find((*is).node);
								js->second.modify_input(ID1,sx);
							}
						}
						if(sp_d==1)	
						{
							auto b=SFGFINAL.find(old_d);
							vector<sfg_edge_marker> in_s=b->second.return_input();
							vector<sfg_edge_marker> out_s=b->second.return_output();
							for(auto is=in_s.begin();is!=in_s.end();is++)
							{
								auto js=SFGFINAL.find((*is).node);
								js->second.modify_output(ID1,sx);
							}
							for(auto is=out_s.begin();is!=out_s.end();is++)
							{
								auto js=SFGFINAL.find((*is).node);
								js->second.modify_input(ID1,sx);
							}
						}
						////////////////////////////////////////////////////////////////////
						auto tts=SFGFINAL.find(news);	
						auto uus=SFGFINAL.find(newd);	
						int fts= tts->second.getFlag();
						int fus= uus->second.getFlag(); 
						string temp="-"+sx;
						if(fts==1 || fus==1)
						{
							SFGFINAL.insert(pair<string, SFGNodes> (sx,SFGNodes(sx,1,stc))); //Create a node in Final SFG MAP
							SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc))); 
						}
						else if(fts==0 || fus==0)
						{
							SFGFINAL.insert(pair<string, SFGNodes> (sx,SFGNodes(sx,0,stc))); //Create a node in Final SFG MAP
						}
						if(flag1!=1)					
							VND.push_back(node_switching(snd1, sx));
						if(flag2!=1)
							VND.push_back(node_switching(snd2, sx));
						
						char ch='.';
						int ns=news.find_first_of(ch);
						int nd=newd.find_first_of(ch);
						string newss=news.substr(0,ns);
						string newdd=newd.substr(0,nd);
						auto k=ckt.find(stod(newss));	//Finds source node in CKT map
						auto l=ckt.find(stod(newdd));	//Finds destination node in CKT map
						auto t=sfg.find(stod(newss));	//Finds source node in SFG map
						auto u=sfg.find(stod(newdd));	//Finds destination node in SFG map
						
						string st=snd1;
						string st1=st;
						string su=snd2;
						string su1=su;
						st=st+"_"+stc;
						su=su+"_"+stc;
	///////////////////////////////////////////////////////////////////////////////////
						for(auto i=VND.begin();i!=VND.end();i++)
						{
							cout<<(*i).node<<"<=>"<<(*i).mod_node<<"\t";
						}
	///////////////////////////////////////////////////////////////////////////////////
						st=newd;
						su=news;
						cout<<"\n st USED FOR SEARCHING IS  "<<st;					
						cout<<"\n su USED FOR SEARCHING IS  "<<su;	
						cout<<"\n"<<news<<"\t"<<newd;				
						auto tt=SFGFINAL.find(st);	//Source
						auto uu=SFGFINAL.find(su);	//Destination
						int ft= tt->second.getFlag();
						int fu= uu->second.getFlag();
						cout<<"\nFT=  "<<ft<<" FU= "<<fu<<" at Phase "<<stc;
						auto xx=SFGFINAL.find(sx);
						if(xx==SFGFINAL.end())
							cout<<"\n\n\nWARNING at "<<__LINE__<<"\n\n\n";
						if(ft==0 || fu==0)		
						{	
							xx->second.updateFlag(0);
							auto te=SFGFINAL.find(temp);
							if(te!=SFGFINAL.end())
								SFGFINAL.erase(te);
						}
						if(fu==1)
						{
							string jju="-"+su;
							auto jj=SFGFINAL.find(jju);
							if(jj!=SFGFINAL.end())
								SFGFINAL.erase(jj);
						}
						if(ft==1)
						{
							string jjt="-"+st;
							auto jj=SFGFINAL.find(jjt);
							if(jj!=SFGFINAL.end())
								SFGFINAL.erase(jj);
						}	
						vector<sfg_edge_marker> Vsi=uu->second.return_input();
						for(auto i=Vsi.begin();i!=Vsi.end();i++)
						{
							cout<<"\n INPUTS OF SOURCE NODE"<<(*i).node<<"\t"<<(*i).edge;
							auto j=SFGFINAL.find((*i).node);
							if(j==SFGFINAL.end())
								cout<<"\nWORKING "<<__LINE__;
							else
								j->second.modify_output(uu->first,sx);
						}
						vector<sfg_edge_marker> Vso=uu->second.return_output();
						for(auto i=Vso.begin();i!=Vso.end();i++)
						{
							cout<<"\nOUTPUTS OF SOURCE NODE"<<(*i).node<<"\t"<<(*i).edge;
							auto j=SFGFINAL.find((*i).node);
							if(j==SFGFINAL.end())
								cout<<"\n"<<(*i).node<<"\t Does not exists";
							else
								j->second.modify_input(uu->first,sx);
						}
						vector<sfg_edge_marker> Vdi=tt->second.return_input();
						cout<<"\n"<<tt->first;
						for(auto i=Vdi.begin();i!=Vdi.end();i++)
						{
							cout<<"\n INPUTS OF DESTINATION NODE"<<(*i).node<<"\t"<<(*i).edge;
							auto j=SFGFINAL.find((*i).node);
							if(j==SFGFINAL.end())
								cout<<"\n\n\nWARNING at "<<__LINE__<<"\n\n\n";
							else
								j->second.modify_output(tt->first,sx);
						}
						vector<sfg_edge_marker> Vdo=tt->second.return_output();
						cout<<"\n"<<tt->first;
						for(auto i=Vdo.begin();i!=Vdo.end();i++)
						{
							cout<<"\n OUTPUTS OF DESTINATION NODE"<<(*i).node<<"\t"<<(*i).edge;
							auto j=SFGFINAL.find((*i).node);
							if(j==SFGFINAL.end())
								cout<<"\n\n\nWARNING at "<<__LINE__<<"\n\n\n";
							else
								j->second.modify_input(tt->first,sx);
						}
						vector<sfg_edge_marker> in,out;		//Temporary vector
						in.insert(in.begin(),Vsi.begin(),Vsi.end());	//Push the entire source node input branches in the temp vector
						in.insert(in.end(),Vdi.begin(),Vdi.end());	//Push the entire destination node input branches at the end of temp vector
						out.insert(out.begin(),Vso.begin(),Vso.end());	//Push the entire source node output branches in the temp vector
						out.insert(out.end(),Vdo.begin(),Vdo.end());	//Push the entire destination node output branches at the end of temp vector
						for(auto i=in.begin();i!=in.end();i++)
						{
							//xx->second.update_input(*i);
						}				
						for(auto i=out.begin();i!=out.end();i++)
						{
							//xx->second.update_output(*i);
						}
						SFGFINAL.erase(tt);
						for(auto i=VND.begin();i!=VND.end();i++)
						{
							if((*i).node==st1)
							{
							//	VND.erase(i);
								break;
							}
						}
						SFGFINAL.erase(uu);
						for(auto i=VND.begin();i!=VND.end();i++)
						{
							if((*i).node==su1)
							{
							//	VND.erase(i);
								break;
							}
						}

						//Temporary vector
						vector<string> b;	
						if(k==ckt.end())
						{
							cout<<"\n\n\nWARNING at "<<__LINE__<<"\n\n\n";
						}
						else
						{
							vector<string> sb=k->second.return_branches(); 	//Extracts the branches of source node
							b.insert(b.begin(),sb.begin(),sb.end());	//Push the entire source node branches in the temp vector
						}
						if(l==ckt.end())
						{
							cout<<"\n\n\nWARNING at "<<__LINE__<<"\n\n\n";
						}
						else
						{
							vector<string> db=l->second.return_branches();	//Extracts the branches of destination node
							b.insert(b.end(),db.begin(),db.end());	//Push the entire destination node branches at the end of temp vector
						}
						
						
						
						auto m=ckt.find(x);	//Finds the new combined node
						m->second.update_sub_branch(b);	//Update the branch of new node with the combined temp vector containing branches of both source and destination
						sfg.erase(t);	//Erased the source node from SFG 
						sfg.erase(u);	//Erased the destination from SFG
						ckt.erase(k);	//Erased the source from CKT
						ckt.erase(l);	//Erased the destination from CKT
					}
					else if(destination_node=="0")			//If the Destination node is GROUND
					{
						double s=stod(source_node);		//Source node
						double d=stod(destination_node);	//Destination node
						auto k=ckt.find(pow(2,s));	//Finds the source node
						auto l=sfg.find(pow(2,d));	
						string ss=to_string(pow(2,s));
						string sss=ss;
						ss=ss+"_"+stc;
						string dd=to_string(pow(2,d));
						dd="1";	
						cout<<"\nCHECK HERE ????????????????????";
						cout<<"\nSOURCE ="<<ss<<"&& destination  = "<<dd;				
						auto ii=SFGFINAL.find(ss);
						vector<sfg_edge_marker> inp;
						if(ii==SFGFINAL.end())
							cout<<"\nWARNING "<<__LINE__;
						else	
						{	
							cout<<"\nCREATING";
							inp=ii->second.return_input();
						}
						auto jj=SFGFINAL.find(dd);
						vector<sfg_edge_marker> out;
						if(jj==SFGFINAL.end())
							cout<<"\nWARNING "<<__LINE__;
						else
						{
							cout<<"\nCREATING";
							out=jj->second.return_output();
						}
						for(auto i=inp.begin();i!=inp.end();i++)
						{
							auto j=SFGFINAL.find((*i).node);
							if(j==SFGFINAL.end())
								cout<<"\n"<<(*i).node<<"\t Does not exists";
							else
								j->second.modify_input(ii->first,"1");
						}
						for(auto i=out.begin();i!=out.end();i++)
						{
							auto j=SFGFINAL.find((*i).node);
							if(j==SFGFINAL.end())
								cout<<"\n"<<(*i).node<<"\t Does not exists";
							j->second.modify_output(ii->first,"1");
						}
						//vector<sfg_edge_marker> out=k
						int ff=ii->second.getFlag();
						if(ff==1)
						{
							string temp="-"+ii->first;
							auto bb=SFGFINAL.find(temp);
							SFGFINAL.erase(bb);
						}
						SFGFINAL.erase(ii);
						
						for(auto i=VND.begin();i!=VND.end();i++)
						{
							if((*i).node==sss)
							{
								//(*i).mod_node=="0.000000_"+stc;
								VND.erase(i);
								break;
							}
						}
						ckt.erase(k);	//Erase the source node
						if(l!=sfg.end())										
							sfg.erase(l);
						auto j=ckt.find(pow(2,s));	//Finds the destination node
						//j->second.update_sub_branch(V);	//Copy the branches of source node into the destination node
					}	
				}
				getline(infile,line);
				line_count++;
			}
			else if(line[0]=='M'|| line[0]=='P')
			{
				char ch='_';				//Character "("
				int f1=line.find_first_of(ch);		//Find the first occurence of _
				int ucount = 0;
				int f2=line.find(ch,f1+1);			//Find the 2nd occurence of _
				int f3=line.find_last_of('_');		//Find the last occurence of _

 				string tran_name=line.substr(0,f1); //Find the name of transistor
				string gate=line.substr(f1+1,f2-f1-1);	//Extracts the gate node like GG as in MX_GG_DD_SS
				string drain=line.substr(f2+1,f3-f2-1);	//Extracts the drain node like DD as in MX_GG_DD_SS
				int kline=line.length();
				string source=line.substr(f3+1,kline-f3-1);	//Extracts the source node like SS as in MX_GG_DD_SS
				cout<<"\n"<<line;
				cout<<"\nLENGTH OF LINE : "<<kline;				
				cout<<"\n"<<f3+1<< " & "<<kline-f3-1;
				cout<<"\nSOURCE FOUND : "<<source;				
				cout<<"\nTRANSISTOR FOUND : "<<tran_name<<" GDS "<<gate<<" "<<drain<<" "<<source;
				tran_name=tran_name+stc;
				

				double gat=stod(gate);
				double dra=stod(drain);
				double sou=stod(source);
				gat=pow(2,gat);
				dra=pow(2,dra);
				sou=pow(2,sou);				
				string gg=to_string(gat);
				string dd=to_string(dra);
				string ss=to_string(sou);

				string gg1,dd1, ss1;

				if(gat!=1)
					gg1=gg+"_"+stc;	//Appending the phase ID if its not a GROUND NODE
				else
					gg1="1";		//gROUND node

				if(dra!=1)
					dd1=dd+"_"+stc;	//Appending the phase ID if its not a GROUND NODE
				else
					dd1="1";		//gROUND node

				if(sou!=1)
					ss1=ss+"_"+stc;	//Appending the phase ID if its not a GROUND NODE
				else
					ss1="1";		//gROUND node

				TRANS.push_back(Transistor(tran_name,gg1,dd1,ss1));

				auto ig = find(TransNodes.begin(), TransNodes.end(), gg1);
    			if (ig == TransNodes.end() && gg1!="1") 
				{
					TransNodes.push_back(gg1);
					DictNodes.push_back(TNodes(gg1));
					
										
				}
				for(auto m=DictNodes.begin();m!=DictNodes.end();m++)
				{
					if((*m).Nodes==gg1)	
					{
						auto n=find((*m).ConTrans.begin(),(*m).ConTrans.end(),tran_name);
						if(n==(*m).ConTrans.end())						
							(*m).updateConTrans(tran_name);
					}
				}
				auto id = find(TransNodes.begin(), TransNodes.end(), dd1);
    			if (id == TransNodes.end() && dd1!="1") 
				{
					TransNodes.push_back(dd1);
					DictNodes.push_back(TNodes(dd1));
				}
				for(auto m=DictNodes.begin();m!=DictNodes.end();m++)
				{
					if((*m).Nodes==dd1)	
					{
						auto n=find((*m).ConTrans.begin(),(*m).ConTrans.end(),tran_name);
						if(n==(*m).ConTrans.end())						
							(*m).updateConTrans(tran_name);
					}
				}
				auto is = find(TransNodes.begin(), TransNodes.end(), ss1);
    			if (is == TransNodes.end() && ss1!="1") 
				{
					TransNodes.push_back(ss1);
					DictNodes.push_back(TNodes(ss1));
				}
				for(auto m=DictNodes.begin();m!=DictNodes.end();m++)
				{
					if((*m).Nodes==ss1)	
					{
						auto n=find((*m).ConTrans.begin(),(*m).ConTrans.end(),tran_name);
						if(n==(*m).ConTrans.end())						
							(*m).updateConTrans(tran_name);
					}
				}
				
				auto k=SFGFINAL.find(gg1);	//Finding if GATE node exists in the Graph
				auto l=SFGFINAL.find(dd1);	//Finding if DRAIN node exists in the Graph
				auto m=SFGFINAL.find(ss1);	//Finding if SOURCE node exists in the Graph
				if(k==SFGFINAL.end())		//If GATE node is not present then create it and also its auxilliary node
				{
					SFGFINAL.insert(pair<string, SFGNodes> (gg1,SFGNodes(gg1,1,stc))); //Create a node for source node in Final SFG MAP
					VND.push_back(node_switching(gg, gg1));	//Push the newly created node in the vector of all nodes
					string temp="-"+gg1;
					SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc))); 	//Creating magic node (current node) of the newly created node (source)
				}
				if(l==SFGFINAL.end())		//If DRAIN node is not present then create it and also its auxilliary node
				{
					SFGFINAL.insert(pair<string, SFGNodes> (dd1,SFGNodes(dd1,1,stc))); //Create a node for source node in Final SFG MAP
					VND.push_back(node_switching(dd, dd1));	//Push the newly created node in the vector of all nodes
					string temp="-"+dd1;
					SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc))); 	//Creating magic node (current node) of the newly created node (source)
				}
				if(m==SFGFINAL.end())		//If SOURCE node is not present then create it and also its auxilliary node
				{
					SFGFINAL.insert(pair<string, SFGNodes> (ss1,SFGNodes(ss1,1,stc))); //Create a node for source node in Final SFG MAP
					VND.push_back(node_switching(ss, ss1));	//Push the newly created node in the vector of all nodes
					string temp="-"+ss1;
					SFGFINAL.insert(pair<string, SFGNodes> (temp,SFGNodes(temp,0,stc))); 	//Creating magic node (current node) of the newly created node (source)
				}
				
				getline(infile,line);
				line_count++;
				
			}
		}
		ALLTRANS.push_back(TRANS);
		//TRANS.clear();
		VNODES.push_back(VND);
		VND.clear();
		CKTX.push_back(ckt);		//push the new map to the vector of maps
		SFGX.push_back(sfg);		//push the new map to the vector of maps
		sfg.clear();	//Clears the new maps for reuse
		ckt.clear();	//Clears the new maps for reuse
	}
	//add_magic_source();
}
///////////////////////FINDING CONNECTED TRANSISTORS///////////////////////////////////////////
void connectedTransistors()
{
	for(auto i=TRANS.begin();i!=TRANS.end()-1;i++)
	{
		for(auto j=TRANS.begin()+1;j!=TRANS.end();j++)
		{
			if(((*j).gate==(*i).gate && (*j).gate!="1" && (*i).gate!="1") || 
		       ((*j).drain==(*i).drain && (*j).drain!="1" && (*i).drain!="1") || 				   
			   ((*j).source==(*i).source && (*j).source!="1" && (*i).source!="1") || 
			   ((*j).gate==(*i).drain && (*j).gate!="1" && (*i).drain!="1") || 
			   ((*j).gate==(*i).source && (*j).gate!="1" && (*i).source!="1") || 
			   ((*j).drain==(*i).gate && (*j).drain!="1" && (*i).gate!="1") || 
			   ((*j).drain==(*i).source && (*j).drain!="1" && (*i).source!="1") || 
			   ((*j).source==(*i).gate && (*j).source!="1" && (*i).gate!="1") || 
			   ((*j).source==(*i).drain && (*j).source!="1" && (*i).drain!="1"))
			{
				if((*i).ID!=(*j).ID)				
				{
					if ((*i).if_exists_neighbour((*j).ID)!=1)
					{
						(*i).updateNeighbours((*j).ID);
					}
					if ((*j).if_exists_neighbour((*i).ID)!=1)
					{
						(*j).updateNeighbours((*i).ID);
					}
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////
void update_input_output();		//Updates the input and output branches of each nodes
void intra_branches(string file_name)
{
	string stc;
	for(int i=0;i<phases;i++)
	{
		stc=to_string(i);
		stc='S'+stc;		//Keep an eye on the SWITCH ID (S1, S2, S3, ...)/ PHASE ID
		cout<<"\n)))))))))))))))))))"<<stc;
		std::string line;				//This is the string type LINE that will store the line by line read data from file
		std::ifstream infile(file_name);		
		if(!infile)						//To check for successful opening of file 
		{
			////cout<<"Could not open File : "<<file_name;
			exit(1);
		}
		getline(infile,line);				//Reads 1st Line
		getline(infile,line);				//Reads 2nd Line
		getline(infile,line);				//Reads 3rd Line
		{
			double snode, dnode;
			char ch='_';				//Character "("
			int f1=line.find_first_of(ch);		//Find the first occurence of _
			int f2=line.find_last_of(ch);		//Find the last occurence of _
			string source_node=line.substr(0,f1);	//Extracts the source node like 10 as in 10_C2_11
			int k=sizeof(line);			
			cout<<"\n"<<line;
			string edge_weight=line.substr(f1+1,f2-f1-1); //Extracts the edge weight/element like C2 as in 10_C2_11
			string destination_node=line.substr(f2+1,k-f2); //Extracts the deatination node like 11 as in 10_C2_11
			cout<<"\nSOURCE NODE "<<source_node;
			cout<<"\nEDGE WEIGHT "<<edge_weight;
			cout<<"\nDESTINATION NODE "<<destination_node;
			double t=stod(destination_node);	// Converts the extracted destination node from string to double
			t=pow(2,t);
			string st=to_string(t);
			string mod_st;
			for(auto n=VNODES[i+1].begin();n!=VNODES[i+1].end();n++)
			{
				if((*n).node==st)
					mod_st=(*n).mod_node;
			}
			auto mod_d=SFGFINAL.find(mod_st);
			mod_d->second.update_input(sfg_edge_marker("-100","1"));
		}
		getline(infile,line);				//Reads 4th Line
		{
			double snode, dnode;
			char ch='_';				//Character "("
			int f1=line.find_first_of(ch);		//Find the first occurence of _
			int f2=line.find_last_of(ch);		//Find the last occurence of _
			string source_node=line.substr(0,f1);	//Extracts the source node like 10 as in 10_C2_11
			int k=sizeof(line);			
			cout<<"\n"<<line;
			string edge_weight=line.substr(f1+1,f2-f1-1); //Extracts the edge weight/element like C2 as in 10_C2_11
			string destination_node=line.substr(f2+1,k-f2); //Extracts the deatination node like 11 as in 10_C2_11
			double f=stod(source_node);	// Converts the extracted source node from string to double
			f=pow(2,f);
			string sf=to_string(f);
			string mod_sf;
			for(auto n=VNODES[i+1].begin();n!=VNODES[i+1].end();n++)
			{
				cout<<"\n"<<(*n).node<<"\t"<<(*n).mod_node;
			}
			for(auto n=VNODES[i+1].begin();n!=VNODES[i+1].end();n++)
			{
				if((*n).node==sf)
					mod_sf=(*n).mod_node;
			}
			char ch1='.';
			int c1=mod_sf.find_first_of(ch);	
			double ff=stod(mod_sf.substr(0,c1));
			auto mod_s=SFGFINAL.find(mod_sf);
			cout<<"\nMOD _S="<<mod_sf;
			auto mod_d=SFGFINAL.find("100");
			int flags=mod_s->second.getFlag();
			cout<<"\nFLAG OF SOURCE IS  "<<flags;
			if(flags ==1)	//If source needs auxiliary voltage source but destination is already having voltage source
			{
				//auto l=CKTX[i].find(f);
				string str;
				for(auto k=CKTX[i].begin();k!=CKTX[i].end();k++)
				{
					double d=k->second.returnNode();
					if(d==ff)
					{
						cout<<"\n\n\nJJJJJJJJJJJJJJJJ\n\n\n\n";
						k->second.displayNode();
						k->second.displayBranches();
						k->second.displayPhase();
						str=k->second.returnDPImp();
					}
				}
				string mod_sf2="-"+mod_sf;
				mod_s->second.update_input(sfg_edge_marker(mod_sf2,str));
				auto mod_s2= SFGFINAL.find(mod_sf2);
				mod_s2->second.update_output(sfg_edge_marker(mod_sf,str));
				mod_s2->second.update_magic_elastance(1);
				mod_d->second.update_input(sfg_edge_marker(mod_sf,edge_weight));	//DRIVING POINT ELASTANCE
				mod_s->second.update_output(sfg_edge_marker("100",edge_weight));	
				cout<<"\n\n\n\n\n\n";
				mod_s->second.dis_node();
				cout<<"\n\n\n\n\n\n";
			}
			else if(flags==0)
			{
				cout<<"\n\n\n\n\n\n";
				mod_s->second.dis_node();
				cout<<"\n\n\n\n\n\n";
				mod_s->second.update_output(sfg_edge_marker("100",edge_weight));
				mod_d->second.update_input(sfg_edge_marker(mod_sf,edge_weight));
			}
		}	
		getline(infile,line);				//Reads 5th line onwards using While loop until END
		//getline(infile,line);				// Takes one line at a time
		//int line_count=0;
		while (infile.good() && line!="END")
		{
			if(line[0]!='M' && line[0]!='P')
			{
				double snode, dnode;
				char ch='_';				//Character "("
				int f1=line.find_first_of(ch);		//Find the first occurence of _
				int f2=line.find_last_of(ch);		//Find the last occurence of _
				string source_node=line.substr(0,f1);	//Extracts the source node like 10 as in 10_C2_11
				int k=sizeof(line);			
				cout<<"\n"<<line;
				string edge_weight=line.substr(f1+1,f2-f1-1); //Extracts the edge weight/element like C2 as in 10_C2_11
				string destination_node=line.substr(f2+1,k-f2); //Extracts the deatination node like 11 as in 10_C2_11
				int flagoo=1;
				if(edge_weight[0]!='S' && destination_node!="0")
				{	
					cout<<"\nLINE TAKEN FOR CONSIDERATION "<<line;
					//////UPDATE IN CIRCUIT MAP
					double f;
					double t;
					f=stod(source_node);		// Converts the extracted source node from string to double date type
					f=pow(2,f);					
					t=stod(destination_node);	// Converts the extracted destination node from string to double
					t=pow(2,t);
					string sf=to_string(f);
					string st=to_string(t);
					string mod_sf, mod_st;
					for(auto n=VNODES[i+1].begin();n!=VNODES[i+1].end();n++)
					{
						if((*n).node==sf)
						{
							flagoo=1;
							mod_sf=(*n).mod_node;
						}
						if((*n).node==st)
						{
							flagoo=1;
							mod_st=(*n).mod_node;
						}
						//else
						{
						//	flagoo=0;
						}	
					}
					if(!mod_sf.empty() || !mod_st.empty())
					{
						int flags,flagd;
						cout<<"\nBEFORE TAKING OUT NUMBER "<<mod_sf;
						char ch='.';
						int c1=mod_sf.find_first_of(ch);	
						string ts=mod_sf.substr(0,c1);
						cout<<"\nTS ==  "<<ts;
						double ff=stod(ts);
						cout<<"\nNUMBER"<<ff;
						//ff=stod(ff);
						int c2=mod_st.find_first_of(ch);	
						double tt=stod(mod_st.substr(0,c2));
						auto mod_s=SFGFINAL.find(mod_sf);
						if(t==1)
							mod_st="1";
						auto mod_d=SFGFINAL.find(mod_st);
						flags=mod_s->second.getFlag();
						flagd=mod_d->second.getFlag();
						cout<<"\nFLAG S ="<<flags<<" , FLAG D ="<<flagd;
						if(flags==0 && flagd==0)
						{
							mod_s->second.update_output(sfg_edge_marker(mod_st,edge_weight));
							mod_d->second.update_input(sfg_edge_marker(mod_sf,edge_weight));
						}
						else if(flags ==0 && flagd==1)	//If source has a voltage source but destination needs auxiliary source
						{
							string mod_st2;
							mod_st2="-"+mod_st;
							auto mod_d2= SFGFINAL.find(mod_st2);
							if(mod_d2==SFGFINAL.end())
								cout<<"\n DOES NOT EXISTS";				
							else
							{
								mod_s->second.update_output(sfg_edge_marker(mod_st2,edge_weight));
								mod_d2->second.update_input(sfg_edge_marker(mod_sf,edge_weight));
								//auto l=CKTX[i].find(t);
								cout<<"\n"<<t;
								//cout<<"\n"<<l->first;
								string str;
								for(auto k=CKTX[i].begin();k!=CKTX[i].end();k++)
								{
									double d=k->second.returnNode();
									if(d==tt)
									{
										cout<<"\n\n\nJJJJJJJJJJJJJJJJ\n\n\n\n";
										k->second.displayNode();
										k->second.displayBranches();
										k->second.displayPhase();
										str=k->second.returnDPImp();
									}
								}
								int nd2=mod_d2->second.return_magic_elastance();
								//if(nd2!=1)
								{
									mod_d2->second.update_output(sfg_edge_marker(mod_st,str));
									mod_d2->second.update_magic_elastance(1);
								}	
								string ss=mod_d2->first;
								cout<<"\nCHECK HEREEEEEEE"<<ss<< "&& "<<str;
								mod_d->second.update_input(sfg_edge_marker(ss,str));	//DRIVING POINT ADMITTANCE
							}
						}
						else if(flags ==1 && flagd==0)	//If source needs auxiliary voltage source but destination is already having voltage source
						{
							if(edge_weight=="OP+" || edge_weight =="OP-")	//If the element in between the two nodes is an OPAMP
							{
								//auto l=CKTX[i].find(f);
								string str;
								for(auto k=CKTX[i].begin();k!=CKTX[i].end();k++)
								{
									double d=k->second.returnNode();
									if(d==ff)
									{
										cout<<"\n\n\nJJJJJJJJJJJJJJJJ\n\n\n\n";
										k->second.displayNode();
										k->second.displayBranches();	
										k->second.displayPhase();
										str=k->second.returnDPImp();
									}
								}
								//string str=l->second.returnDPImp();
								string mod_sf2="-"+mod_sf;
								mod_s->second.update_input(sfg_edge_marker(mod_sf2,str));
								auto mod_s2= SFGFINAL.find(mod_sf2);
								int ns2=mod_s2->second.return_magic_elastance();
								//if(ns2!=1)
								{
									mod_s2->second.update_output(sfg_edge_marker(mod_sf,str));
									mod_s2->second.update_magic_elastance(1);
								}

								mod_d->second.update_input(sfg_edge_marker(mod_sf,edge_weight));	//DRIVING POINT ELASTANCE
								mod_s->second.update_output(sfg_edge_marker(mod_st,edge_weight));
								
							}
							else				//If the element in-between the two nodes is not an OPAMP (This part is still not configured properly because I haven't come accross any circuit that has this kind of Flag status situation because these kinds of cases are usually feedback loops and source and destination nodes gets swapped
							{
								cout<<"\n NOT ENCOUNTERED THIS KIND OF CIRCUIT";				
							}				
						}
						else if(flags ==1 && flagd==1)		//If both source and destination needs auxiliary voltage sources
						{
		//						auto ls=CKTX[i].find(f);
		//						auto ld=CKTX[i].find(t);
							string strs,strd;
							for(auto k=CKTX[i].begin();k!=CKTX[i].end();k++)
							{
								double d=k->second.returnNode();
								if(d==ff)
								{
									cout<<"\n\n\nJJJJJJJJJJJJJJJJ\n\n\n\n";
									k->second.displayNode();
									k->second.displayBranches();
									k->second.displayPhase();
									strs=k->second.returnDPImp();
								}
							}
							for(auto k=CKTX[i].begin();k!=CKTX[i].end();k++)
							{
								double d=k->second.returnNode();
								if(d==tt)
								{
									cout<<"\n\n\nJJJJJJJJJJJJJJJJ\n\n\n\n";
									k->second.displayNode();
									k->second.displayBranches();	
									k->second.displayPhase();
									strd=k->second.returnDPImp();
								}
							}
							string mod_sf2="-"+mod_sf;
							string mod_st2="-"+mod_st;
							auto mod_s= SFGFINAL.find(mod_sf);
							auto mod_d= SFGFINAL.find(mod_st);
							auto mod_s2= SFGFINAL.find(mod_sf2);
							auto mod_d2= SFGFINAL.find(mod_st2);
							mod_s->second.update_output(sfg_edge_marker(mod_st2,edge_weight));
							mod_s->second.update_input(sfg_edge_marker(mod_sf2,strs));
							int ns2=mod_s2->second.return_magic_elastance();
							//if(ns2!=1)
							{
								mod_s2->second.update_output(sfg_edge_marker(mod_sf,strs));	
								mod_s2->second.update_magic_elastance(1);
							}
							mod_s2->second.update_input(sfg_edge_marker(mod_st,edge_weight));
							int nd2=mod_d2->second.return_magic_elastance();
							//if(nd2!=1)
							{
								mod_d2->second.update_output(sfg_edge_marker(mod_st,strd));
								mod_d2->second.update_magic_elastance(1);
							}

							mod_d2->second.update_input(sfg_edge_marker(mod_sf,edge_weight));
							mod_d->second.update_input(sfg_edge_marker(mod_st2,strd));
							mod_d->second.update_output(sfg_edge_marker(mod_sf2,edge_weight));	//DRIVING POINT ELASTANCE
						}
					}
				}
				getline(infile,line);
			}
			else if(line[0]=='M' || line[0]=='P')
				getline(infile,line);
		}
	}
	update_input_output();
}


void update_transbranches()
{
	//Considering one node at a time
	for(auto i=DictNodes.begin();i!=DictNodes.end();i++)
	{
		string node=(*i).Nodes;		//The node	
		string DPIMP="1/(";
		string mnode;
		cout<<"\nCONSIDERING THE NODE : "<<node;
		auto in=SFGFINAL.begin();
		auto min=SFGFINAL.begin();
		vector<string> contrans=(*i).ConTrans;	//All the transistors connected to it
		for(auto l=contrans.begin();l!=contrans.end();l++)
		{	
			vector<Transistor> CopyTrans=TRANS;
			cout<<"\nCONSIDERING THE TRANSISTOR : "<<*l;
			auto j=CopyTrans.begin();
			for(j=CopyTrans.begin();j!=CopyTrans.end();j++)
			{
				cout<<"\nCHECKING THE TRANSISTOR "<<(*j).ID;
				if((*j).ID==(*l))
				{
					cout<<"\nREMOVING "<<(*j).ID;
					//CopyTrans.erase(j);
					break;
				}
			}
			string type=(*j).returnNodeType(node);
			cout<<"\nCHECKING NODE TYPE IN THE TRANSISTOR FOUND "<<type;
			string name=(*l);
			int namef=name.find_last_of('S');
			string pha=name.substr(namef+1);//Checking phase of node 
			int phase=stoi(pha);
			cout<<"\nPHASE = "<<phase;
			int fin = node.find('_');
			string subn = node.substr(0, fin);  //Extract the ID before _ in XX.XXX_SX
			cout<<"\nPURE ID = "<<subn;
			cout<<"\nPREVIOUS NODE ID : "<<node;
			for(auto i=VNODES[phase].begin();i!=VNODES[phase].end();i++)
			{
				if((*i).node==subn)
					node=(*i).mod_node;		//Get the modified NODE ID
			}
			cout<<"\nMODIFIED NODE ID : "<<node;
			in=SFGFINAL.find(node);
			mnode="-"+node;
			min=SFGFINAL.find(mnode);

			////////////////UPDATING THE DPIMP //////////////////////////////
			if(type=="gate" || type=="gateDrain")
			{
				DPIMP=DPIMP+"+s.Cgs"+name;
			}
			if(type=="drain")
			{
				DPIMP=DPIMP+"+gds"+name+"+s.Cds"+name;
			}
			if(type=="source")
			{
				DPIMP=DPIMP+"+gds"+name;
			}
			
			
			/////////////////////////////////////////////////////////////////
			if(type=="gate" || type == "gateDrain")
			{
				cout<<"\nTYPE"<<type;
				string d=(*j).returnDrain();	//Drain node ID
				string s=(*j).returnSource();	//Source node ID
				int fd = d.find('_');		//Finds _ in XX.XXXX_SX
				string subd = d.substr(0, fd);  //Extract the ID before _ in XX.XXX_SX
				int fs = s.find('_');		//Finds _ in XX.XXXX_SX
				string subs = s.substr(0, fs);  //Extract the ID before _ in XX.XXX_SX
				if(d!="1")		//If drain is not GROUND
				{
					for(auto i=VNODES[phase].begin();i!=VNODES[phase].end();i++)
					{
						if((*i).node==subd)
							d=(*i).mod_node;		//Get the modified NODE ID
					}
				}
				if(s!="1")		//If source is not GROUND
				{
					for(auto i=VNODES[phase].begin();i!=VNODES[phase].end();i++)
					{
						if((*i).node==subs)
							s=(*i).mod_node;		//Get the modified NODE ID
					}
				}
				//Effects of its gm on its magic sources (Effect on Self magic source nodes)
		//		if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
		//		{	
		//			cout<<"\nHIHI"<<node;
		//			in->second.update_input(sfg_edge_marker(mnode,("-gm"+name)));
		//			if(min==SFGFINAL.end())
		//				cout<<"\nDAMMMMMMNNNNN";				
		//			else					
		//				min->second.update_output(sfg_edge_marker(node,("-gm"+name)));
		//		}
		//		if(name[0]=='P') //If pmos then increase in Vgs, decreases Id and hence increases cuurent thorough Magic source
		//		{	
		//			cout<<"\nHIHI"<<node;
		//			in->second.update_input(sfg_edge_marker(mnode,("gm"+name)));
		//			if(min==SFGFINAL.end())
		//				cout<<"\nDAMMMMMMNNNNN";				
		//			min->second.update_output(sfg_edge_marker(node,("gm"+name)));
		//		}

				//Checking effect of Gate node on Drain node(if drain node is not GROUND)
				if(d!="1" && node!=d)
				{
					auto id=SFGFINAL.find(d);
					string md="-"+d;
					auto mid=SFGFINAL.find(md);
					if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence increases cuurent to Magic source
					{	
						cout<<"\nHIHI"<<node;
						in->second.update_output(sfg_edge_marker(md,("-gm"+name)));
						if(mid==SFGFINAL.end())
							cout<<"\nDAMMMMMMNNNNN";				
						else					
							mid->second.update_input(sfg_edge_marker(node,("-gm"+name)));
					}
					if(name[0]=='P') //If pmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
					{	
						
						in->second.update_output(sfg_edge_marker(md,("-gm"+name)));
						if(mid==SFGFINAL.end())
							cout<<"\nDAMMMMMMNNNNN";				
						else					
							mid->second.update_input(sfg_edge_marker(node,("-gm"+name)));
					}
				}
				if(d!="1" && node==d)
				{
					if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence increases cuurent to Magic source
					{	
						
						DPIMP = DPIMP + "+gm" + name;
						if(type =="gateDrain")
							DPIMP = DPIMP + "+gds" + name;
						//in->second.update_input(sfg_edge_marker(mnode,("gm"+name)));
						//if(min==SFGFINAL.end())
						//	cout<<"\nDAMMMMMMNNNNN";				
						//else					
						//	min->second.update_output(sfg_edge_marker(node,("gm"+name)));
					}
					if(name[0]=='P') //If pmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
					{	
						
						DPIMP = DPIMP + "+gm" + name;
						if(type =="gateDrain")
							DPIMP = DPIMP + "+gds" + name;
						//in->second.update_input(sfg_edge_marker(mnode,("gm"+name)));
						//if(min==SFGFINAL.end())
						//	cout<<"\nDAMMMMMMNNNNN";				
						//else					
						//	min->second.update_output(sfg_edge_marker(node,("gm"+name)));
					}
					
				}
				//Checking effect of Gate node on source node
				if(s!="1" && node!=s)
				{
					auto is=SFGFINAL.find(s);
					string ms="-"+s;
					auto mis=SFGFINAL.find(ms);
					if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
					{	
						
						in->second.update_output(sfg_edge_marker(ms,("gm"+name)));
						if(mis==SFGFINAL.end())
							cout<<"\nDAMMMMMMNNNNN";				
						else					
							mis->second.update_input(sfg_edge_marker(node,("gm"+name)));
					}
					if(name[0]=='P') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
					{	
						
						in->second.update_output(sfg_edge_marker(ms,("gm"+name)));
						if(mis==SFGFINAL.end())
							cout<<"\nDAMMMMMMNNNNN";				
						else					
							mis->second.update_input(sfg_edge_marker(node,("gm"+name)));
					}
				}
			}
			else if(type=="source")
			{
				cout<<"\nTYPE"<<type;
				string d=(*j).returnDrain();
				string g=(*j).returnGate();
				int fd = d.find('_');
				string subd = d.substr(0, fd);  //Extract the ID before _ in XX.XXX_SX
				int fg = g.find('_');
				string subg = g.substr(0, fg);  //Extract the ID before _ in XX.XXX_SX
				if(d!="1")	
				{
					for(auto i=VNODES[phase].begin();i!=VNODES[phase].end();i++)
					{
						if((*i).node==subd)
							d=(*i).mod_node;		//Get the modified NODE ID
					}
				}
				if(g!="1")	
				{
					for(auto i=VNODES[phase].begin();i!=VNODES[phase].end();i++)
					{
						if((*i).node==subg)
							g=(*i).mod_node;		//Get the modified NODE ID
					}
				}
				DPIMP = DPIMP + "+s.Cgs" + name;
                DPIMP = DPIMP + "+s.Cds" + name;
				//DPIMP = DPIMP +"+
				//Effects of its gm on its magic sources
		//		if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
		//		{	
		//			cout<<"\nHIHI"<<node;
		//			in->second.update_input(sfg_edge_marker(mnode,("gm"+name)));
		//			if(min==SFGFINAL.end())
		//				cout<<"\nDAMMMMMMNNNNN";				
		//			else					
		//				min->second.update_output(sfg_edge_marker(node,("gm"+name)));
		//		}
		//		if(name[0]=='P') //If pmos then increase in Vgs, decreases Id and hence increases cuurent thorough Magic source
		//		{	
		//			cout<<"\nHIHI"<<node;
		//			in->second.update_input(sfg_edge_marker(mnode,("-gm"+name)));
		//			if(min==SFGFINAL.end())
		//				cout<<"\nDAMMMMMMNNNNN";				
		//			min->second.update_output(sfg_edge_marker(node,("-gm"+name)));
		//		}
				//Checking effect of Source node on Drain node(if drain node is not GROUND)
				if(d!="1" && node!=d)
				{
					auto id=SFGFINAL.find(d);
					string md="-"+d;
					auto mid=SFGFINAL.find(md);
					if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
					{	
						
						in->second.update_output(sfg_edge_marker(md,("gm"+name)));
						if(mid==SFGFINAL.end())
							cout<<"\nDAMMMMMMNNNNN";				
						else					
							mid->second.update_input(sfg_edge_marker(node,("gm"+name)));
					}	
					if(name[0]=='P') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
					{	
						
						in->second.update_output(sfg_edge_marker(md,("gm"+name)));
						if(mid==SFGFINAL.end())
							cout<<"\nDAMMMMMMNNNNN";				
						else					
							mid->second.update_input(sfg_edge_marker(node,("gm"+name)));
					}
				}
				//Checking effect of Source node on Gate node (If source and gate are not sorted) I DONT THINK THERE WILL BE ANY EFFECT
			//	if(g!="1" && node!=g)
			//	{
			//		auto ig=SFGFINAL.find(g);
			//		string mg="-"+g;
			//		auto mig=SFGFINAL.find(mg);
			//		if(name[0]=='M') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
			//		{	
			//			cout<<"\nHIHI"<<node;
			//			in->second.update_output(sfg_edge_marker(mg,("gm"+name)));
			//			if(mig==SFGFINAL.end())
			//				cout<<"\nDAMMMMMMNNNNN";				
			//			else					
			//				mig->second.update_input(sfg_edge_marker(node,("gm"+name)));
			//		}
			//		if(name[0]=='P') //If nmos then increase in Vgs, increases Id and hence reduces cuurent to Magic source
			//		{	
			//			cout<<"\nHIHI"<<node;
			//			is->second.update_output(sfg_edge_marker(ms,("-gm"+name)));
			//			if(mis==SFGFINAL.end())
			//				cout<<"\nDAMMMMMMNNNNN";				
			//			else					
			//				mis->second.update_input(sfg_edge_marker(s,("gm"+name)));
			//		}
			//	}
			}
			
		}
		DPIMP=DPIMP+")";
		cout<<"\nDPIMP FOUND  : "<<DPIMP;
		DPIMP.erase(3,1);
		in->second.update_input(sfg_edge_marker(mnode,DPIMP));
		cout<<"Updating input and output branches here";
		min->second.update_output(sfg_edge_marker(node,DPIMP));
	}
}
vector <string> VoltageNodes;
vector <string> MagicNodes;
void update_VI_VO()
{
	auto m=SFGFINAL.find("100");
	m->second.clear_input();
	m->second.clear_output();
	auto n=SFGFINAL.find("-100");
	n->second.clear_input();
	n->second.clear_output();
	
	for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
	{
			
		if(i->first != "100" && i->first != "-100" && i->first != "1" )
		{	 
			auto it = find(TransNodes.begin(), TransNodes.end(), i->first);
			if(it==TransNodes.end())
			{
				string s=i->first;
				if(s[0]!='-')
					VoltageNodes.push_back(i->first); ///Maintains a list of all voltage nodes	
				else if(s[0]=='-')
					MagicNodes.push_back(i->first);
				vector<sfg_edge_marker> I=i->second.return_input();
				for(auto j=I.begin();j!=I.end();j++)
				{
					if((*j).node=="-100")
						m->second.update_output(sfg_edge_marker((*j).node,(*j).edge));
				}
				vector<sfg_edge_marker> O=i->second.return_output();
				for(auto j=O.begin();j!=O.end();j++)
				{
					if((*j).node=="100")
						n->second.update_input((*j));
				}
			}
		}
	}
}
void findTransConnectedToNodes(string s)
{
	for(auto i=TRANS.begin();i!=TRANS.end();i++)
	{
		
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void add_magic_source()
{
	for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
	{
		//cout<<"\nNODE ID: "<<i->first;
		int f=i->second.getFlag();
		string d=i->first;
		d="-"+d;
		string p=i->second.returnPhase();
		if(f==1)
		{
			SFGFINAL.insert(pair<string, SFGNodes> (d,SFGNodes(d,0,p))); 
		}
	}
}


void update_self_loops();		//Update the self loops
vector<double> specialNodes;		
//////////////////////////GETTING RID OF UNITY GAIN FEEDBACK///////////////////////////////
void update_self_loops()		//This function get rids of unity gain feedback 
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




////////////////////////////////////////////////////////////////////////////////////////////
void update_input_output()
{
	for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
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
		i->second.displayPhase();
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
void display_subcircuits()
{
	cout<<"\n$$$$$$$$$$$PRINTING SUB CIRCUITS DETAILS";
	for(int i=0;i<phases;i++)
	{
		if(!SFGX.empty())
		{
			for(auto j=SFGX[i].begin();j!=SFGX[i].end();j++)
			{
				cout<<"\n*****************************************************";
				cout<<"\nNODE ID: "<<j->first;
				j->second.displayPhase();
				j->second.displayFlag();
				cout<<"\n";
				j->second.displayInputs();
				cout<<"\n";
				j->second.displayOutputs();
			}
		}
		if(!CKTX.empty())
		{
			cout<<"\nCHECK HERE FOR CIRCUIT DETAILS IN CKTX MAP";
			for(auto k=CKTX[i].begin();k!=CKTX[i].end();k++)
			{
				cout<<"\nNODE ID: "<<k->first;
				k->second.displayBranches();
				string s=k->second.returnDPImp();
				cout<<"\n"<<s;
				k->second.displayPhase();
			}
		}
		
	}
}
void display_final_graph()
{
	cout<<"\nFINAL SIGNAL FLOW GRAPH";
	for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
	{
		cout<<"\n*****************************************************";
		cout<<"\nNODE ID: "<<i->first;
		i->second.displayFlag();
		cout<<"\n";
		i->second.displayInputs();
		cout<<"\n";
		i->second.displayOutputs();
	}
 	ofstream outputFile("FullGraph.txt"); // Open the output file
	if (outputFile.is_open()) 
	{
		for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
		{	
		    //outputFile << "\n\nNODE IDs: "<<i->first;
			vector <sfg_edge_marker> in=i->second.returnInputs();
			vector <sfg_edge_marker> out=i->second.returnOutputs();
		    //outputFile << "\nINPUT BRANCHES";	    
			if(!in.empty())
			{	
				for(auto j=in.begin();j!=in.end();j++)	
					outputFile <<(*j).node<<" "<<(*j).edge<<" "<<i->first<<endl;
			}
			
		    //outputFile << "\nOUTPUT BRANCHES";	    
			if(!out.empty())
			{
				for(auto j=out.begin();j!=out.end();j++)		    
					outputFile <<i->first<<" "<<(*j).edge<<" "<<(*j).node<<endl;
			}
		}
	    outputFile.close();
		cout << "Output file has been written successfully.";
    } 
	else 
	{
       	cout << "Unable to open the output file.";
    }

}
void modify_multi_edged_node_pair() //Combining multiple edges between a pair of nodes
{
	for(auto i = SFGFINAL.begin(); i != SFGFINAL.end(); i++)
	{
		i->second.combine_edges_from_same_node();

		
	}
	for(auto i = SFGFINAL.begin(); i != SFGFINAL.end(); i++)
	{
		i->second.remove_duplicate_nodes();
	}
}
void display_NodeID_Tree()
{
	int count =0;
	for(auto i=VNODES.begin();i!=VNODES.end();i++)
	{
		cout<<"\nPHASE ID: "<<count<<"\n";
		for(auto j=(*i).begin();j!=(*i).end();j++)
		{
			cout<<(*j).node<<"<=>"<<(*j).mod_node<<"\t";
		}
		count++;
	}
}
void display_cktx()
{

	cout<<"\n\n\n DISPLAYING CIRCUIT X AND ITS BRANCHES";
	int count=0;
	for(auto i=CKTX.begin();i!=CKTX.end();i++)
	{
		//cout<<"\n PHASE  = "<<count<<"\n";
		for(auto j=(*i).begin();j!=(*i).end();j++)
		{
			//cout<<"\n";
			//j->first;
			//cout<<"\n";
			j->second.displayNode();
			cout<<"\t";
			j->second.displayPhase();	
			cout<<"\t";
			j->second.displayBranches();
		}
		count++;
	}
	cout<<"\n\n\n";
}
vector<vector<string>> NodeDirectory;
void createNodeDirectory()
{
	int count=0;
	vector<string> s;
	while(count<phases)
	{
		for(auto i =VoltageNodes.begin();i!=VoltageNodes.end();i++)
		{
			string node=(*i);
			char ch='S';
			int f1=node.find_first_of('.');
			int f2=node.find_last_of(ch);
			int k=sizeof(node);
			string ID=node.substr(0,f1);		//Extracts NODE ID from XXX.00000_SY where XXX is the Node Id
			string phase=node.substr(f2+1,k-f2);	//Extracts Phase ID from XXX.00000_SY where Y is the Phase Id
			double p=stod(phase);	//Converts phase id to double type		
			if(p==count+1)
				s.push_back(node);
		}
		count++;
		NodeDirectory.push_back(s);
		s.clear();
	}
	//Displaying Node Directory
	for(auto i=NodeDirectory.begin();i!=NodeDirectory.end();i++)
	{
		cout<<"\n";
		for(auto j=(*i).begin();j!=(*i).end();j++)
			cout<<"\t"<<*j;
	}
}
void inter_branches_2()
{
	vector<string> caps;
	createNodeDirectory();
	for(auto i=VoltageNodes.begin();i!=VoltageNodes.end();i++)
	{
		cout<<"\n\n\n"<<(*i)<<" TAKEN FOR CONSIDERATION ";	
		string node=(*i);
		char ch='S';
		int f1=node.find_first_of('.');
		int f2=node.find_last_of(ch);
		int k=sizeof(node);
		string ID=node.substr(0,f1);		//Extracts NODE ID from XXX.00000_SY where XXX is the Node Id
		string phase=node.substr(f2+1,k-f2);	//Extracts Phase ID from XXX.00000_SY where Y is the Phase Id
		
		double p=stod(phase);	//Converts phase id to double type
		cout<<"\nPHASE = "<<p;
		double pnext;
		if(p<phases)
			pnext=p+1;
		else if(p==phases)
			pnext=1;
		cout<<"\nNEXT PHASE = "<<pnext;
		double id=stod(ID); //Converts node id to double type
		cout<<"\nID OF CURRENT NODE = "<<id;	
		auto j=CKTX[p-1].find(id);	//iterator to Cktx map element corresponding to Node ID XXX in respective phase p-1=Y
		caps=j->second.return_branches();	//Returns all the capcitors associated with the Node XXX in phase Y
		cout<<"\n";
		for(auto ca=caps.begin();ca!=caps.end();ca++)
		{
			cout<<*ca<<"\t";
		}
		string s=*i;
		auto d=SFGFINAL.find(s);	//Finds voltage node in SFGFINAL Graph
		int flagd=d->second.getFlag(); //Checks if the voltage node is magic or not
		if(flagd==1)
		{
			s="-"+(*i);
			d=SFGFINAL.find(s);
		}
		vector<sfg_edge_marker> vin;	
		vector<sfg_edge_marker> vout;
		vin=d->second.return_input(); //Extracts inputs branches
		vout=d->second.return_output();	//Extracts output branches
		for(auto c=caps.begin();c!=caps.end();c++)
		{
			int cap_flag=0;
			int in_flag=0,out_flag=0,special_flag=0;
			cout<<"\n\nCapacitor considered = "<<*c;
			for(auto j=vin.begin();j!=vin.end();j++)
			{
				if((*j).edge == (*c))
				{
					cout<<"\nFOUND THE CAPACITOR IN THE INPUT BRANCH";
					in_flag=1;
					break;
				}
			}
			if(in_flag==0)
			{
				for(auto j=vout.begin();j!=vout.end();j++)
				{
					if((*j).edge == (*c))
					{
						cout<<"\nFOUND THE CAPACITOR IN THE OUTPUT BRANCH";
						out_flag=1;
						break;
					}
				}
			}
			if(in_flag==0 && out_flag==0)
			{
				special_flag=1;
				cout<<"\nCapacitor is not connected to input or output branches, that means it must be grounded";
			}

			cout<<"\n";
			//Checking the status of the capcitor in the next phase from CKTX
			string pnext_node;
			for(auto k=CKTX[pnext-1].begin();k!=CKTX[pnext-1].end();k++)
			{
				if(k->second.returnNode()!=1)
				{	
					int cktx_flag=0;
					cout<<"\nLooking for capacitor "<<*c<<" in NEXT phase "<<pnext<<" at node "<<k->second.returnNode();
					vector<string> b=k->second.return_branches();
					for(auto ib=b.begin();ib!=b.end();ib++)
					{
						cout<<"\n"<<*ib;
						if(*ib==*c)
						{
							cktx_flag=1;
							double nd=k->second.returnNode();
							string pn=to_string(pnext);
							char ch='.';
							int f=pn.find_first_of(ch);
							pn=pn.substr(0,f);
							pnext_node=to_string(nd)+"_S"+pn;
							cout<<"\nCapacitor "<<*c<<" found in phase "<<pnext<<" connected to node "<<k->second.returnNode()<<" which is denoted by "<<pnext_node;
							break;
						}
					}
					if(cktx_flag==1)
					{
						cap_flag=1;
						break;
					}
					else
					{
						cap_flag=0;
						cout<<"\nCapacitor does not exits in the next phase ";
						//break;
					}	
				}
			}
			if(cap_flag==1)			
			{
				//Looking for the pnext_node in the SFGFINAL
				auto d2=SFGFINAL.find(pnext_node);	//Finds voltage node in SFGFINAL Graph
				int flagd2=d2->second.getFlag(); //Checks if the voltage node is magic or not
				if(flagd2==1)
				{
					pnext_node="-"+(pnext_node);
					d2=SFGFINAL.find(pnext_node);
				}
				cout<<"\nPNEXT_NODE"<<pnext_node;
				vector<sfg_edge_marker> vin2;	
				vector<sfg_edge_marker> vout2;
				vin2=d2->second.return_input(); //Extracts inputs branches
				vout2=d2->second.return_output();	//Extracts output branches
				int in_flag2=0,out_flag2=0,special_flag2=0;
				//cout<<"\n\nCapacitor considered = "<<*c;
				for(auto j=vin2.begin();j!=vin2.end();j++)
				{
					if((*j).edge == (*c))
					{
						cout<<"\nFOUND THE CAPACITOR IN THE INPUT BRANCH IN NEXT PHASE";
						in_flag2=1;
						break;
					}
				}
				if(in_flag2==0)
				{
					for(auto j=vout2.begin();j!=vout2.end();j++)
					{
						cout<<"\n size= "<<vout2.size()<<" , "<<(*j).node<<" , "<<(*j).edge;
						if((*j).edge == (*c))
						{
							cout<<"\nFOUND THE CAPACITOR IN THE OUTPUT BRANCH IN NEXT PHASE";
							out_flag2=1;
							cout<<"\nFLAGD2 = "<<flagd2;
							if(flagd2 == 0)
							{
								string s=(*j).node;
								cout<<"\n"<<s;
								auto mf=SFGFINAL.find(s);
								if(s[0]=='-')
								{
									pnext_node=s;
									cout<<"\nUPDATED PNEXT_NODE "<<pnext_node;
								}
							}
								
							break;
						}
						else
							cout<<"\nLet's take a look at the next one";
					}
				}
				if(in_flag2==0 && out_flag2==0)
				{
					special_flag2=1;
					cout<<"\nCapacitor is not connected to input or output branches, that means it must be grounded in the Next phase also";
				}
				//Time to verify that pnext_node is the desired node...or it may be the neighbouring node that connects the capcitor to pnext_node
				if(flagd!=1)
				{
					
				}

				//Time to draw the branch in SFG
				if(special_flag==1 and special_flag2==1)
				{
					
					if(pnext_node[0]!='-')
						cout<<"\n Capacitor "<<*c<<" will not impact any magic source";
					else if(pnext_node[0]=='-')
					{
						cout<<"\n Branch drawn from "<<node<<" to "<<pnext_node<<" with edge weight = z^-1"<<*c;
						auto f1=SFGFINAL.find(node);					
						auto f2=SFGFINAL.find(pnext_node);
						string edge_weight="z^-1"+*c;
						f1->second.update_output(sfg_edge_marker(pnext_node,edge_weight));
						f2->second.update_input(sfg_edge_marker(node,edge_weight));
					}
				}
				else if(in_flag==1 && out_flag2==1)
				{
					if(pnext_node[0]=='-')
					{
						cout<<"\nBranch drawn from "<<node<<" to "<<pnext_node<<" with edge weight = z^-1"<<*c;
						auto f1=SFGFINAL.find(node);					
						auto f2=SFGFINAL.find(pnext_node);
						string edge_weight="z^-1"+*c;
						f1->second.update_output(sfg_edge_marker(pnext_node,edge_weight));
						f2->second.update_input(sfg_edge_marker(node,edge_weight));
					}
					else
					{
						cout<<"\nNo branches created because in_flag="<<in_flag<<" & out_flag2= "<<out_flag2<<" & Flagd2= "<<flagd2<<" pnext_node is "<<pnext_node;
					}
				}
				else if(in_flag==1 && in_flag2==1)
				{
					if(pnext_node[0]=='-')
					{
						cout<<"\nBranch drawn from "<<node<<" to "<<pnext_node<<" with edge weight = z^-1"<<*c;
						auto f1=SFGFINAL.find(node);					
						auto f2=SFGFINAL.find(pnext_node);
						string edge_weight="z^-1"+*c;
						f1->second.update_output(sfg_edge_marker(pnext_node,edge_weight));
						f2->second.update_input(sfg_edge_marker(node,edge_weight));
					}
				}
				else if(out_flag==1 && in_flag2==1)
				{
					if(pnext_node[0]=='-')
					{
						cout<<"\nBranch drawn from "<<node<<" to "<<pnext_node<<" with edge weight = -z^-1"<<*c;
						auto f1=SFGFINAL.find(node);					
						auto f2=SFGFINAL.find(pnext_node);
						string edge_weight="-z^-1"+*c;
						f1->second.update_output(sfg_edge_marker(pnext_node,edge_weight));
						f2->second.update_input(sfg_edge_marker(node,edge_weight));
					}
				}
				else if(in_flag==1 && special_flag2==1)
				{
					if(pnext_node[0]=='-')
					{
						cout<<"\nBranch drawn from "<<node<<" to "<<pnext_node<<" with edge weight = -z^-1"<<*c;
						auto f1=SFGFINAL.find(node);					
						auto f2=SFGFINAL.find(pnext_node);
						string edge_weight="z^-1"+*c;
						f1->second.update_output(sfg_edge_marker(pnext_node,edge_weight));
						f2->second.update_input(sfg_edge_marker(node,edge_weight));
					}
				}
				else if(out_flag==1 && out_flag2==1)
				{
					if(pnext_node[0]=='-')
					{
						cout<<"\nBranch drawn from "<<node<<" to "<<pnext_node<<" with edge weight = -z^-1"<<*c;
						auto f1=SFGFINAL.find(node);					
						auto f2=SFGFINAL.find(pnext_node);
						string edge_weight="-z^-1"+*c;
						f1->second.update_output(sfg_edge_marker(pnext_node,edge_weight));
						f2->second.update_input(sfg_edge_marker(node,edge_weight));
					}
				}
			}

		}
	}
}
void update_input_output_node()
{
	auto k=SFGFINAL.find("-100");
	k->second.clear_input();
	k->second.clear_output();
	for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
	{
		vector<sfg_edge_marker> inp=i->second.return_input();
		for(auto j=inp.begin();j!=inp.end();j++)
		{
			if((*j).node == "-100")
			{
				k->second.update_output(sfg_edge_marker(i->first,(*j).edge));
			}
		}
	}
	auto l=SFGFINAL.find("100");
	l->second.clear_input();
	l->second.clear_output();
	for(auto i=SFGFINAL.begin();i!=SFGFINAL.end();i++)
	{
		vector<sfg_edge_marker> out=i->second.return_output();
		for(auto j=out.begin();j!=out.end();j++)
		{
			if((*j).node == "100")
			{
				l->second.update_input(sfg_edge_marker(i->first,(*j).edge));
			}
		}
	}	
}
void inter_branches_1()
{		
	vector<string> caps;
	createNodeDirectory();
	for(auto i=VoltageNodes.begin();i!=VoltageNodes.end();i++)
	{
		cout<<"\n\n\n"<<(*i)<<" TAKEN FOR CONSIDERATION ";	
		string node=(*i);
		char ch='S';
		int f1=node.find_first_of('.');
		int f2=node.find_last_of(ch);
		int k=sizeof(node);
		string ID=node.substr(0,f1);		//Extracts NODE ID from XXX.00000_SY where XXX is the Node Id
		string phase=node.substr(f2+1,k-f2);	//Extracts Phase ID from XXX.00000_SY where Y is the Phase Id
		
		double p=stod(phase);	//Converts phase id to double type
		cout<<"\nPHASE = "<<p;
		double pnext;
		if(p<phases)
			pnext=p+1;
		else if(p==phases)
			pnext=1;
		cout<<"\nNEXT PHASE = "<<pnext;
		double id=stod(ID); //Converts node id to double type
		cout<<"\nID OF CURRENT NODE = "<<id;	
		auto j=CKTX[p-1].find(id);	//iterator to Cktx map element corresponding to Node ID XXX in respective phase p-1=Y
		caps=j->second.return_branches();	//Returns all the capcitors associated with the Node XXX in phase Y
		cout<<"\n";
		for(auto ca=caps.begin();ca!=caps.end();ca++)
		{
			cout<<*ca<<"\t";
		}

		auto d=SFGFINAL.find(*i);	//Finds voltage node in SFGFINAL Graph
		int flagd=d->second.getFlag(); //Checks if the voltage node is magic or not
		if(flagd==1)
		{
			string s="-"+(*i);
			d=SFGFINAL.find(s);
		}
		vector<sfg_edge_marker> vin;	
		vector<sfg_edge_marker> vout;
		vin=d->second.return_input(); //Extracts inputs branches
		vout=d->second.return_output();	//Extracts output branches
		
		for(auto c=caps.begin();c!=caps.end();c++)
		{	
			int inf=0,opf=0;
			//Need to find the actual NODE ID of the node
			string actual_ID_1;
			int first_ID_flag=0;
			cout<<"\n\nCapacitor considered = "<<*c;
			while(first_ID_flag==0)
			{
				for(auto ii=VNODES[p].begin();ii!=VNODES[p].end();ii++)
				{
					if((*ii).mod_node==*i)	
					{
						actual_ID_1=(*ii).node;		//Actual NODE ID of SOURCE
						cout<<"\nACTUAL ID OF CURRENT NODE = "<<actual_ID_1;
						cout<<"\nFINDING THE MODIFIED ID IN THE NEXT PHASE";
						for(auto jj=VNODES[pnext].begin();jj!=VNODES[pnext].end();jj++)
						{
							if((*jj).node==actual_ID_1)
							{
								string s=(*jj).mod_node;
								cout<<"\nMODIFIED ID FOUND IS = "<<s;
								auto m=SFGFINAL.find(s);
								int flagm=m->second.getFlag(); //Checks if the voltage node is magic or not
									if(flagm==1)
								{
									s="-"+s;
									m=SFGFINAL.find(s);
									cout<<"\nLOOKING FOR NODE "<<s;
								}
								vector<sfg_edge_marker> in=m->second.return_input();
								vector<sfg_edge_marker> on=m->second.return_output();
								cout<<"\nCHECKING IF THE NODE HAS ANY INPUT BRANCH THAT CONTAINS THE GIVEN CAPCITANCE";	
								for(auto ini=in.begin();ini!=in.end();ini++)
								{
									if((*ini).edge==*c)
									{
										inf=1;
										cout<<"\nYES, CAPCITOR "<<*c<<" IS CONNECTED TO INPUT BRANCH OF NODE "<<m->first; 
										break;
									}
								}	
								if(inf==0)
								{
									cout<<"\nCHECKING IF THE NODE HAS ANY OUTPUT BRANCH THAT CONTAINS THE GIVEN CAPCITANCE";
									for(auto ino=on.begin();ino!=on.end();ino++)
									{
										if((*ino).edge==*c)
										{
											inf=1;
											cout<<"\nYES, CAPCITOR "<<*c<<" IS CONNECTED TO OUTPUT BRANCH OF NODE "<<m->first; 
											break;
										}
									}
								}
							}
							if(inf==1)
							{
								first_ID_flag=1;
								cout<<"\nVALID ACTUAL NODE ID IS FINALLY FOUND"<<actual_ID_1;
								//break;
							}
							if(jj==VNODES[pnext].end()-1 && inf==0)
							{
								//cout<<"\nCapacitor "<<*c<<" is connected to ground";
								first_ID_flag=2;
								//break;
							}
						
						}
					}
					if(first_ID_flag==1)
					{
						cout<<"\nFINALLY FOUND VALID NODE"<<actual_ID_1;
						break;
					}
					if(first_ID_flag==2)
					{
						cout<<"\nCAPACITOR "<<*c<<" IS NOT CONNECTED TO THE CURRENT CONSIDERED ACTUAL NODE ID "<<actual_ID_1;
						//break;
					}
					if(ii==VNODES[p].end()-1  &&  first_ID_flag==2)								
					{
						cout<<"\nCAPACITOR "<<*c<<" IS CONNECTED TO THE GROUND";
						cout<<"\nACTUAL NODE ID "<<actual_ID_1;
					}
				}
			}
			//if(first_ID
			//Takes one capcitor at a time
		}
		
	}
}


//////////////////////////////FINDING FORWARD PATHS////////////////////////////////////////
vector<string> Current_Path;
vector<vector<string>> All_Path;
void dfs(string x, string y);
void find_paths()
{
	cout<<"\nFIND PATHS FUNCTION WORKING";
	dfs("-100","100");
	//cout<<"\nFORWARD PATHS DISPLAYED BELOW";
	for(auto i=All_Path.begin();i!=All_Path.end();i++)
	{
		//cout<<"\n";
		for(auto j=(*i).begin();j!=(*i).end();j++)
		{
			cout<<(*j)<<"\t";
		}
	}
}
void dfs(string x, string y)
{
	auto i= SFGFINAL.find(x);
	int m=i->second.return_dfs_mark();
	if(m==1)
		return;
	i->second.update_dfs_mark(1);
	Current_Path.push_back(i->first);
	if(x==y)
	{
		All_Path.push_back(Current_Path);
		auto i= SFGFINAL.find(x);
		i->second.update_dfs_mark(0);
		Current_Path.pop_back();
		return;
	}	
	vector<sfg_edge_marker> next=i->second.return_output();
	for(auto k=next.begin();k!=next.end();k++)
	{
		dfs((*k).node,y);
	}
	Current_Path.pop_back();
	i->second.update_dfs_mark(0);
}
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////FINDING CYCLES//////////////////////////////////////
vector<vector<string>> All_Loop;
void printCycle(stack<string> st, string n);
void processDFStree(stack<string> s);
void findCycles()
{
	cout<<"\nFIND CYCLES WORKING";
	//for(auto i=SFG.begin();i!=SFG.end();i++)
	{	
	//	if(i->first!=100)
		{
			//cout<<"\nSTARTING FROM NODE "<<i->first;
	//		int v=i->second.returnVisited();
	//		if(v==0)
			{
				auto i=SFGFINAL.find("-100");
				stack<string> sta;
				sta.push(i->first);
				i->second.updateVisited(1);
				processDFStree(sta);
			}
		}
	}
}
void processDFStree(stack<string> s)
{
	cout<<"\nPROCESS DFSTREE CALLED";
	string n=s.top();
	auto i=SFGFINAL.find(n);
	vector<sfg_edge_marker> VO=i->second.return_output();
	for(auto j=VO.begin();j!=VO.end();j++)
	{
		if((*j).node!="100")	
		{	auto k=SFGFINAL.find((*j).node);
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
void printCycle(stack<string> st, string n)
{
	
	//cout<<"\nPRINTING CYCLE";
	stack<string> sta2;
	sta2.push(st.top());
	st.pop();
	while(sta2.top()!=n)
	{
		sta2.push(st.top());
		st.pop();
	}
	//sta2.push(st.top());
	//st.pop();
	vector <string> D;
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

//Calculating Forward paths along with their gains
vector <string> ForwardGains;
void displayForwardPaths()
{
	ofstream outputFile("ForwardNodes.txt"); // Open the output file storing the nodes connectivity of the forward path nodes only
	ofstream outputFile2("ForwardEdges.txt"); //Open the output file storing the edges of the forward path
	ofstream outputFile3("ForwardEdgesNodes.txt"); //Open the output file storing the edges of the forward path and also nodes
	if (outputFile.is_open()) 
	{
		
		int count=1;
		for(auto i=All_Path.begin();i!=All_Path.end();i++)
		{
			string s;
			cout<<"\nDISPLAYING FORWARD PATH NO. : "<<count<<"\n";
			
			for(auto j=(*i).begin();j!=(*i).end()-1;j++)
			{
				auto k=SFGFINAL.find(*j);
				cout<<(*j)<<" ";
				outputFile<<(*j)<<" ";
				outputFile3<<(*j)<<" ";
				string s2;
				s2=k->second.returnSpecialEdge(*(j+1));
				outputFile2<<s2<<" ";
				outputFile3<<s2<<" ";
				if(s2!="1" && s2!="\0")
				{
					s=s+s2+".";
				}
			}
			outputFile<<" 100";
			outputFile3<<" 100";
			outputFile<<"\n";
			outputFile2<<"\n";
			outputFile3<<"\n";
			ForwardGains.push_back(s);
			cout<<"\n"<<"GAIN OF THIS FORWARD PATH IS = ("<<s<<")";
			count++;
		}
		
		outputFile.close();
		outputFile2.close();
		outputFile3.close();
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
vector <vector <string>> FinalLoops;
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
	ofstream outputFilesx("LoopsNodesEdges.txt"); // Open the output file storing the nodes connectivity of the forward path nodes only
	if(!All_Loop.empty())
	{
		//FinalLoops=All_Loop;

		for(auto j=All_Loop.begin();j!=All_Loop.end();j++)
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
			for(auto j=(*i).begin();j!=(*i).end();j++)
			{
				auto k=SFGFINAL.find(*j);
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
					All_Loop[j]={"1"};
				}
				
			}	
		}
		LoopGains.erase(remove_if(LoopGains.begin(),LoopGains.end(),rmv::remove("0")),LoopGains.end());
		for(auto j=All_Loop.begin();j<All_Loop.end();j++)
		{
			int flag=0;
			for(auto l=(*j).begin();l!=(*j).end();l++)
			{
				if((*l)!="1")
					flag=1;
			}
			if(flag==1)
			{
				FinalLoops.push_back((*j));
				cout<<"\n PRINTING *J ////////////////////////////////////////////////////////////////";
				for(auto l=(*j).begin();l!=(*j).end();l++)				
				{	cout<<"\t"<<*l;
				}
				cout<<"\n";
			}
		}
		int countloop=1;
		for(auto j=FinalLoops.begin();j<FinalLoops.end();j++)
		{
			cout<<"\nDISPLAYING LOOPS AFTER OPERATIONS, LOOP NO: "<<countloop<<"\n";
			for(auto l=(*j).begin();l!=(*j).end();l++)
			{
				cout<<"\t"<<*l;
				outputFilesx <<*l<<" ";
				auto k = SFGFINAL.find(*l);
				string ledge;
				if( l != (*j).end()-1)
					ledge = k->second.returnSpecialEdge(*(l+1));
				else
					ledge = k->second.returnSpecialEdge(*((*j).begin()));
				outputFilesx<<ledge<<" ";
					
			}
			countloop++;
			outputFilesx<<(*((*j).begin()));
			outputFilesx<<"\n";
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
void checkCommonNode(vector<vector<string>> Z, int r,int data[], vector<vector<string>> FinalL, vector<string> LGains, int mode)
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
		string d=(*i);
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



void CheckCombination(int Loops[], int data[], int start, int end, int index, int r, vector<vector<string>> FinalL, vector<string> LGains, int mode)
{
	if(index==r)
	{	vector<string> L;
		vector<vector<string>> Z;
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
void findCombinations(int Loops[], int n, int r, vector<vector<string>> FinalL, vector<string> LGains, int mode)
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
	//for(auto i=LoopGains.begin();i!=LoopGains.end();i++)
	{
	//	cout<<"\t"<<*i;
	}
	////////////////CALCULATION OF DELTA (DINOMINATOR OF TF)/////////////////////////////
	
	for(auto i=LoopGains.begin();i!=LoopGains.end();i++)
	{
		if(i!=(LoopGains.end()-1))
			Delta=Delta+" ("+(*i)+") "+"+";
		else
			Delta=Delta+" ("+(*i)+") ";
	}
	Delta=Delta+"}";
	cout<<"\n DELTA = "<<Delta;
	int n=FinalLoops.size();
	int *arr = new int[n];
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
	delete[] arr;
}
void findNumerator()
{
	cout<<"\nFINDING THE NUMERATOR OF THE TRANSFER FUNCTION \n";
	vector<vector<string>> Loops=FinalLoops;
	vector<string> Gains=LoopGains;
	vector<vector<string>> SelectedLoops=Loops;
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
			string d=(*j);
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
		int *arr = new int[n];
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
		delete[] arr;
		num=num+X+D+"+";
		D.clear();
		f++;
	}
		
}

void display_all_trans()
{
	for(auto i=TRANS.begin();i!=TRANS.end();i++)
	{
		(*i).displayTrans();
	}
	
}

void displayTransNodes()
{
	for(auto i=DictNodes.begin();i!=DictNodes.end();i++)
	{
		cout<<"\n"<<(*i).Nodes;
		cout<<"\nTRANSISTORS CONNECTED TO THIS NODE ARE : ";
		for(auto j=(*i).ConTrans.begin();j!=(*i).ConTrans.end();j++)
		{
			cout<<"\t"<<*j;
		}
	}
}

int main()
{	
	time_t my_time=time(NULL);
	int c=13;
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
		s="StraySensitiveSCIntegrator.isc";
	else if(c==8)
		s="SimulatedInductor.isc";
	else if(c==9)
		s="FirstOrderSCFilter.isc";
	else if(c==10)
		s="ParasiticSensitiveIntegrator.isc";
	else if(c==11)
		s="LowQSCBiquadFilter.isc";
	else if(c==12)
		s="ParasiticSensitiveIntegrator_tran.isc";
	else if(c==13)
		s="5T_OTA_Integrator_tran.isc";
	else if(c==14)
		s="5T_OTA_Single_Ended_Stack_Integrator.isc";
	else if(c == 15)
		s="CM_OTA_Integrator_tran.isc";
	else if(c == 16)
		s="2Stage_OTA.isc";
	ckt_parse_1(s);
	subcircuit(s);
	connectedTransistors();
	//update_vdd_flag();
	intra_branches(s);
	display_all_trans();	
	update_VI_VO();
	inter_branches_2();
	update_input_output_node();
	displayTransNodes();
	update_transbranches();
		//display_graph();
		//display_circuit();
		//display_subcircuits();
		//display_cktx()
		//display_NodeID_Tree();
	modify_multi_edged_node_pair();
	display_final_graph();
	//cout<<"\nNO. OF NODES IN THE GRAPH "<<SFGFINAL.size();
	int t=1;
	if(t==1)
	{
		find_paths();
		findCycles();
		displayForwardPaths();
		displayLoops();
		//findDELTA();
		//findNumerator();
		//findNumerator();
		cout<<"\nDELTA CALCULATED = "<<Delta;
		cout<<"\nNUMERATOR OF TF : "<<num;
		cout<<"\n*******************************************";
		cout<<"\n\n\nTRANSFER FUNCTION OF THE CIRCUIT "<<s<<" IS ";
		cout<<"\n\n"<<num;
		//cout<<"\n____________________________________________________________________________________________________________________________";
		//cout<<"\n\n"<<Delta<<"\n";
	}
	cout<<"\n**************************************************"<<endl;
	return 0;
}
