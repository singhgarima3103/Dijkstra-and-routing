#ifndef __GRAPH_H_
#define __GRAPH_H_

#include<iostream>
#include<map>
#include<set>
using namespace std;

// class to represent undirected graph
class Graph
{
public:
	int numVertices;
	map<int, map<int, int> > vertices; //internal map is adjacency list of the vertex stored as key
	Graph(int n) :numVertices(n){}
};

#endif __GRAPH_H_

