#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include"FibonacciHeap.h"
#include"Graph.h"

FibonacciHeap * fHeap;

void dijkstra(Graph * g,int srcNodeLabel,int destNodeLabel)
{
	fHeap = new FibonacciHeap();//create heap object
	FibHeapNode * fnode;
	string path; //used for printing later
	vector<int>distFromSource(g->numVertices); //array to hold dist of all vertices from the source
	vector<int>prev(g->numVertices);// stores previous vertex to every vertex in shortest path from source to said vertex
	map<int,FibHeapNode*> FheapNodeList;//store FibHeapNode pointers with vertex as key to be used in decrease key later

	distFromSource[srcNodeLabel] = 0;
	map<int,int> srcAdjacencyList; //adjacency list of source

	map<int,int>::iterator innerIt;
	for(innerIt= g->vertices[srcNodeLabel].begin();innerIt!=g->vertices[srcNodeLabel].end();innerIt++)
	{
		srcAdjacencyList[innerIt->first] = innerIt->second;//populate src adjacency list
	}

	map<int,map<int,int> >::iterator it;
	

	for(it= g->vertices.begin();it!=g->vertices.end();it++)	
	{
		if(it->first != srcNodeLabel)
		{
			if(srcAdjacencyList.find(it->first) != srcAdjacencyList.end())
			{
				//belongs to adjacency list
				distFromSource[it->first] = srcAdjacencyList[it->first];//dist is edge weight initially
				prev[it->first] = srcNodeLabel;//prev is source initially 
				
			}
			else
			{
				//for all other nodes
				distFromSource[it->first] = 2000;
				prev[it->first] = NULL;
			}  
				
		}
		fnode = fHeap->insert(it->first,distFromSource[it->first]);//put node in heap
		FheapNodeList.insert(pair<int, FibHeapNode *>(it->first,fnode));//add pointer to map

	}

	while(!fHeap->empty())
	{
		FibHeapNode * min = fHeap->minimum();
		int minKey = min->key();
		int minKeyLabel = min->data();
		fHeap->removeMin();
		it = g->vertices.find(minKeyLabel)	;
		for(innerIt = (it->second).begin(); innerIt!= (it->second).end() ; innerIt++)
		{
			int alt = distFromSource[minKeyLabel] + innerIt->second;
			if(alt < distFromSource[innerIt->first])
			{
				distFromSource[innerIt->first] = alt;
				prev[innerIt->first]  = minKeyLabel;
				fHeap->decreaseKey((FheapNodeList.find(innerIt->first))->second, alt); //retrieve node pointer from map and pass to decrease key
			}
		}
	}

	path.insert(0,to_string(destNodeLabel));
	cout<<distFromSource[destNodeLabel]<<"\n";
	int node = prev[destNodeLabel];
	while(node != srcNodeLabel)
	{
		path.insert(0,to_string(node)+" ");
		node = prev[node];
	}
	path.insert(0,to_string(srcNodeLabel)+" ");
	cout<<path;
}

int main(int __argc, char ** __argv)
{
	if(__argc != 4){
		cout<<"incomplete commands";
		return 0;
	}
	
	//file containing graph info
	char * fileName = __argv[1];
	
	//process source vertex
	istringstream ss(__argv[2]);
	int srcNodeLabel;
	ss>> srcNodeLabel;
	
	//process destination vertex
	istringstream ss2(__argv[3]);
	int destNodeLabel;
	ss2>>destNodeLabel;

	ifstream fin;// create a file-reading object
	fin.open(fileName);
	if (!fin.good()) 
		return 1; // exit if file not found
  
	int v1, v2,w1,numVertices,numEdges;
	
	fin>> numVertices>> numEdges;
	
	Graph * g = new Graph(numVertices); //create Graph object

	while (fin >> v1 >> v2 >>w1 ) //process v1 v2 and w(both endpoints and edge weight)
	{
		//populate graph
		g->vertices[v1].insert(pair<int,int>(v2,w1)); 
		g->vertices[v2].insert(pair<int,int>(v1,w1));
	
	}

	dijkstra(g,srcNodeLabel,destNodeLabel);
	
	//free memory utilised
	delete(g);
	delete(fHeap);

	return 0;
}