#include<iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include"FibonacciHeap.h"
#include"Graph.h"
#include<bitset>

Graph * g;
FibonacciHeap * fHeap;
int numVertices;
int srcNodeLabel;
int destNodeLabel;
int ** vertexToIPMap; //2d array to store IP of each vertex
bool setRoot = false;

//class for binary trie
class BinTrie
{
public:
	int data;	//stores 0/1 for internal nodes and vertex number for leaf nodes
	bool isLeaf;
	BinTrie * left, * right;
	BinTrie(bool isleaf)
	{
		isLeaf = isleaf;
		left = right = NULL;
	}
	BinTrie(int d,bool isleaf)
	{
		data = d;
		isLeaf =  isleaf;
		left = right = NULL;
	}
};

//returns pointer to trie for ownerVertex
BinTrie * constructBinTrie(int ownerVertex, vector<int> prev)
{
	BinTrie * root = new BinTrie(false);
	BinTrie * traverse ;//used to traverse the bin trie
	int k =0;// to iterate over vertices


	//loop for all vertices except owner
	while(k < g->numVertices)
	{
		
		if(k != ownerVertex)
		{
			traverse = root;
			int i=0;
			
			while(i<32)
			{
				if(vertexToIPMap[k][i] == 0)
				{
					if(traverse->left == NULL)
					{
						traverse->left = new BinTrie(false); // if doesnt exist create new node
					}
					
					traverse = traverse->left;
				}
				else
				{
					if(traverse->right == NULL)
					{
						traverse->right = new BinTrie(false);// if doesnt exist create new node
					}
					
					traverse = traverse->right;
				}
				i++;
			}
			
			int node = prev[k];
			if(node != ownerVertex){
				while(prev[node] != ownerVertex)
				{
					node = prev[node];
				}
			
			traverse->data =  node ; //next hop is the node just after source in shortest path
			traverse->isLeaf = true;
			}
			else
			{
				 traverse->data =  k ; //next hop is destination itself
				traverse->isLeaf = true;
			}

		}
		
		k++;
	}
	return root;
}


//traverse final trie based on prefix matching of dest node IP and return next hop
int traverseTrie(int destNode,BinTrie * trie)
{
	BinTrie * traverse = trie;
	if(traverse)
	{
	   int k =0;
	   while(traverse->isLeaf != true )
		{
			if(vertexToIPMap[destNode][k]==0)
			{
				cout<<"0";
				traverse= traverse->left;
			}
			else
			{
				cout<<"1";
				traverse= traverse->right;
			}

			k++;
		}	
	}
	return traverse->data; // return next hop
}

//modification of post order traversal to condense bin trie
void postOrder(BinTrie * root)
{
	
		if((root) == NULL)
			return;
	
		if((root)->left != NULL)
			postOrder(((root)->left));
		if(&(root)->right != NULL)
			postOrder(((root)->right));

		 if((root)->left != NULL && (root)->right!= NULL)
		{
			if((root)->left->isLeaf && (root)->right->isLeaf)
			{
				if((root)->left->data == (root)->right->data)
				{
					(root)->data = (root)->left->data;
					(root)->left = (root)->right = NULL;
					(root)->isLeaf = true;
				}
			}
		}
		else if((root)->left == NULL && (root)->right!=NULL)
		{
			if((root)->right->isLeaf)
			{
				(root)->data = (root)->right->data;
				(root)->right = NULL;
				(root)->isLeaf = true;
			}
		}
		else if((root)->right == NULL && (root)->left!=NULL)
		{
			if((root)->left->isLeaf)
			{
				(root)->data = (root)->left->data;
				(root)->left = NULL;
				(root)->isLeaf = true;
			}
		}
			
}

//delete subtries who have same next hop by calling next hop
void condenseBinTrie( BinTrie * trie)
{
	postOrder(trie);
}

//variation of dijkstra which returns previous array
vector<int> dijkstra(Graph * g,int srcNode,int destNode)
{

	fHeap = new FibonacciHeap();//create heap object
	FibHeapNode * fnode;
	string path; //used for printing later
	vector<int>distFromSource(g->numVertices); //array to hold dist of all vertices from the source
	vector<int>prev(g->numVertices);// stores previous vertex to every vertex in shortest path from source to said vertex
	map<int, FibHeapNode*> FheapNodeList;//store FibHeapNode pointers with vertex as key to be used in decrease key later

	distFromSource[srcNode] = 0;
	map<int, int> srcAdjacencyList; //adjacency list of source

	map<int, int>::iterator innerIt;
	for (innerIt = g->vertices[srcNode].begin(); innerIt != g->vertices[srcNode].end(); innerIt++)
	{
		srcAdjacencyList[innerIt->first] = innerIt->second;//populate src adjacency list
	}

	map<int, map<int, int> >::iterator it;


	for (it = g->vertices.begin(); it != g->vertices.end(); it++)
	{
		if (it->first != srcNode)
		{
			if (srcAdjacencyList.find(it->first) != srcAdjacencyList.end())
			{
				//belongs to adjacency list
				distFromSource[it->first] = srcAdjacencyList[it->first];//dist is edge weight initially
				prev[it->first] = srcNode;//prev is source initially 

			}
			else
			{
				//for all other nodes
				distFromSource[it->first] = 2000;
				prev[it->first] = NULL;
			}

		}
		fnode = fHeap->insert(it->first, distFromSource[it->first]);//put node in heap
		FheapNodeList.insert(pair<int, FibHeapNode *>(it->first, fnode));//add pointer to map

	}

	while (!fHeap->empty())
	{
		FibHeapNode * min = fHeap->minimum();
		int minKey = min->key();
		int minKeyLabel = min->data();
		fHeap->removeMin();
		it = g->vertices.find(minKeyLabel);
		for (innerIt = (it->second).begin(); innerIt != (it->second).end(); innerIt++)
		{
			int alt = distFromSource[minKeyLabel] + innerIt->second;
			if (alt < distFromSource[innerIt->first])
			{
				distFromSource[innerIt->first] = alt;
				prev[innerIt->first] = minKeyLabel;
				fHeap->decreaseKey((FheapNodeList.find(innerIt->first))->second, alt); //retrieve node pointer from map and pass to decrease key
			}
		}
	}

	if(destNode == destNodeLabel && srcNode == srcNodeLabel)
	{
		cout<<distFromSource[destNodeLabel]<<"\n" ;
	}
	return prev;
}

//to route the packet from source to destination(traverse tries of vertices on the way)
void route(int srcNode, int destNode, map<int, BinTrie *> binTrieMap)
{

	int nextHop = traverseTrie(destNode,binTrieMap[srcNode]);
	cout<<" ";
	while( nextHop != destNode)
	{
		  nextHop = traverseTrie(destNode,binTrieMap[nextHop]) ;
		  cout<<" ";
	}
}

int main(int __argc, char ** __argv)
{
	if(__argc != 5){
		cout<<"incomplete commands";
		return 0;
	}

	
	char * fileName = __argv[1];
	char * fileName2 = __argv[2];

	istringstream ss(__argv[3]);
	
	ss>> srcNodeLabel;
	
	
	istringstream ss2(__argv[4]);
	
	ss2>>destNodeLabel;

	// create a file-reading object
	ifstream fin;
	fin.open(fileName); // open a file
	if (!fin.good()) 
		return 1; // exit if file not found
  
	int v1, v2,w1,numEdges;
	
	fin>> numVertices>> numEdges;

	if(srcNodeLabel > numVertices || srcNodeLabel < 0)
	{
	   cout<<"error, out of bounds source vertex";
	   return 0 ;
	}
		

	if(destNodeLabel > numVertices || destNodeLabel < 0)
	{
	   cout<<"error, out of bounds destination vertex";
	   return 0 ;
	}

	g = new Graph(numVertices);
 
	//allocate memory to 
	vertexToIPMap = (int **)malloc(numVertices * sizeof(int *));

	for(int i=0; i<numVertices;i++)
		vertexToIPMap[i] = (int *) malloc(32*sizeof(int));

	
	while (fin >> v1 >> v2 >>w1 )
	{
		//insert into graph
		g->vertices[v1].insert(pair<int,int>(v2,w1));
		g->vertices[v2].insert(pair<int,int>(v1,w1));
		
	}

	//read input file 2

	// create a file-reading object
	ifstream fin2;
	fin2.open(fileName2); // open a file
	if (!fin2.good()) 
		return 1; // exit if file not found

	
  	   int v = 0;
	   string ipLine;
	  // read each line of the file
	  while ( fin2>> ipLine)
	  {

		// process the tokens

		  size_t pos = 0;
		  string token[4];
		  string delimiter = ".";
		  int i = 0;
		  while ((pos = ipLine.find(delimiter)) != std::string::npos) {
			  token[i] = ipLine.substr(0, pos);
			  ipLine.erase(0, pos + delimiter.length());
			  i++;
		  }
		  token[i] = ipLine;

		int k=0;
		int ip[32];
		for (int i = 0; i < 4; i++) // n = #of tokens
		{
			int N = atoi(token[i].c_str());
			 int O[8];//The output array
			  bitset<8> A=N;//A will hold the binary representation of N 
			  for(int i=0,j=7;i<8;i++,j--)
			  {
				 //Assigning the bits one by one.
				 O[i]=A[j];

				 ip[k]=O[i];
				 k++;
			  }
			
		}
		
		int w =0;
		while(w<32)
		{
			vertexToIPMap[v][w] =ip[w]; //populate map
			w++	;
		}
			
		v++;
		
	  }

	vector<int> prev = dijkstra(g, srcNodeLabel, destNodeLabel);	//calculate prev array for source vertex

	map<int,BinTrie *> vertexToBinTrie; //map to store pointer to bin tree with owner vertex as key

	vertexToBinTrie.insert(pair<int, BinTrie *>(srcNodeLabel,constructBinTrie(srcNodeLabel,prev))); //construct and populate bin trie map

	condenseBinTrie( vertexToBinTrie[srcNodeLabel]); //condense the trie for efficiency

	int node = prev[destNodeLabel];

	//loop to construct and condense bin tries of all vertices
	while(node != srcNodeLabel)
	{
		vertexToBinTrie.insert(pair<int, BinTrie *>(node,constructBinTrie(node,dijkstra(g,node,destNodeLabel))));

		condenseBinTrie(vertexToBinTrie[node]);
		node = prev[node];

	}

	route(srcNodeLabel,destNodeLabel,vertexToBinTrie); //for routing packet from source to destination
	
	//free memory
	delete(g);
	delete(fHeap);
	free(vertexToIPMap);

	return 0;
}