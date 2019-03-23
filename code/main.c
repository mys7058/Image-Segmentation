//  main.c
//  Image Segmentation V2
//
//
//  This program allows the enter to enter a number of vertices, a number of edges connecting the vertices and a constant c
//  The user then enters each edge using the format " V1 V2 Weight " V1 = the first vertex, V2 = the second vertex, Weight = the weight of the edge connecting V1 and V2
//  The program then breaks down the graph into a smaller set of subgraphs (components) so long as the Dissimilarity (D) between two components is greater than the Confidence (H) of either component
//      D(c1,c2) > H(c1) && D(c1,c2) > H(c2)
//  Each of the components are then printed to the screen with a space sepertating each vertex in the component and each component is on a new line
//

#include <stdio.h>
#include <stdlib.h>

//Holds each element in a component
struct component
{
	int identity; //the vertex number (0 to NumberOfVertices-1)
	int weight; //the weight of edge connecting the previous node to this one
	int confidence; //the confidence value of the whole component (ever element this is linked to)
	struct component *next; //pointer to the element after this one
};

//Holds each weighted edge entered by the user
struct edge
{
	int origin; //where the edge begins
	int destination; //where the edge ends
	int weight; // the weight of the edge
};

//Links the two components together if there is an edge connecing the two
void linkComponents(struct component *origin, struct component *destination, int weight)
{
	//we follow the linked list until the last node
	while (origin->next != NULL)
	{
		origin = origin->next;
	}
	//then we connect the second component to the first
	origin->next = destination;

	//the weight given to the first node in the second component is the weight of the edge connecting the two components
	destination->weight = weight;
}

//Calculates the confidence of the component
void confidence(struct component *origin, int constant)
{
	struct component *head = origin;
	int confidence = 0;
	int greatestWeight = 0;
	int totalNumberOfNodes = 0;

	//We go through each of the elements in a component from beginning to end to sum up the greatest edge weight of the component
	//As we pass through an element we increment a counter that represents the number of elements in a component
	while (origin != NULL)
	{
		if (origin->weight > greatestWeight)
		{
			greatestWeight = origin->weight;
		}
		totalNumberOfNodes++;
		origin = origin->next;
	}

	//We then take the total weight of the component and the number of nodes to calculate the confidence of the component
	confidence = greatestWeight + (constant / totalNumberOfNodes);

	//We then insert the confidence of the entire component into each element of the component
	while (head != NULL)
	{
		head->confidence = confidence;
		head = head->next;
	}
}

//Finds the dissimilarity between two components
int Dissimilarity(struct edge *edge, struct component *to, struct component *from)
{
	//If we do not find an edge weight connecting the first and second components then we return a large value representing infinity
	int dissimilarity = 1000000;
	struct component *reset = from;

	//We start with the first element in the first component and check for any connected edges between it and any element in the second component
	while (to != NULL)
	{
		//Starts at the first element in the second component and continues until the last element in the second component
		while (from != NULL)
		{
			//If we find a connecting edge, we check to see if it is less than the current Dissimlarity value
			//If it is lower then we set it as the new Dissimilarty value and continue checking for a lower value
			if (edge->weight < dissimilarity && edge->weight > 0)
			{
				dissimilarity = edge->weight;
			}
			//This take us to the next element in the second component
			from = from->next;
		}
		//This sends us back to the first element in the second component
		from = reset;
		//This takes us to the next element in the first component
		to = to->next;
	}
	//Returns the lowest edge connecting an element from the first component to the second
	//If not there are not any connecting edges then we return a large value representing infinity
	return dissimilarity;
}

//Prints the list of elements in a component per the specifications in the PDF
void printComponent(struct component *n)
{
	while (n != NULL)
	{
		printf("%d", n->identity);
		n = n->next;
		if (n != NULL)
		{
			printf(" ");
		}
	}
	printf("\n");
}

int main()
{
	int vertices, edges, constant;
	int v1, v2, weight;
	int maxWeight = 0;

	//User enters the number of vertices, number of edges, and the constant 'c' used to calculate the confidence of a component
	scanf("%d %d %d", &vertices, &edges, &constant);

	//Creates an array of pointers equal to the number of edges to be entered
	//Each pointer points to an edge structure that holds the weight of an edge and the vertices it connects
	struct edge *edgeArray[edges];
	int i = 0;
	for (i = 0; i < edges; i++)
	{
		//User enters the two vertices the edge connects, and the weight of the edge
		scanf("%d %d %d", &v1, &v2, &weight);

		//The variable maxWeight keeps track of the largest weight entered for later use
		if (weight > maxWeight)
		{
			maxWeight = weight;
		}
		//This is to ensure the path originates from the smaller vertex and terminates at the larger one
		if (v1 > v2)
		{
			int temp = v1;
			v1 = v2;
			v2 = temp;
		}

		//Each index in the array is a pointer that points to an edge structure that holds the two connected vertices and the weight of the edge connecting them
		edgeArray[i] = (struct edge*) malloc(sizeof(struct edge));
		edgeArray[i]->origin = v1;
		edgeArray[i]->destination = v2;
		edgeArray[i]->weight = weight;
	}

	//Creates an array of pointers equal to the number of vertices
	//Each pointer points to a component structure that keeps track of the elements in the structure, the weights of all edges connecting the elements and the confidence of the entire component
	struct component *vertexArray[vertices];

	//Creates a component structure at each index and intializes the values in each
	for (i = 0; i < vertices; i++)
	{
		vertexArray[i] = (struct component*) malloc(sizeof(struct component));
		vertexArray[i]->identity = i;
		vertexArray[i]->weight = 0;
		vertexArray[i]->confidence = constant;
		vertexArray[i]->next = NULL;
	}

	//Starting from 0 and ending at the highest entered edge weight value
	int min = 0;
	for (min = 0; min <= maxWeight; min++)
	{
		//We then check each index in the array of edges to see if there is an edge weight that matches
		for (i = 0; i < edges; i++)
		{
			//If a match is found, and it is not an edge connecting a vertex to the same vertex we then check to see if the two components should merge into one
			if (edgeArray[i]->weight == min && vertexArray[edgeArray[i]->origin] != vertexArray[edgeArray[i]->destination])
			{
				//We check the Dissimilarity of the two components on either end of the edge against their Confidence values
				//If the Dissimilarity is higher than either of the Confidence values, then we merge the two components into one
				if (Dissimilarity(edgeArray[i], vertexArray[edgeArray[i]->origin], vertexArray[edgeArray[i]->destination]) < vertexArray[edgeArray[i]->origin]->confidence &&
					Dissimilarity(edgeArray[i], vertexArray[edgeArray[i]->origin], vertexArray[edgeArray[i]->destination]) < vertexArray[edgeArray[i]->destination]->confidence)
				{
					//The last element in the first component now points to the first element in the second component
					linkComponents(vertexArray[edgeArray[i]->origin], vertexArray[edgeArray[i]->destination], edgeArray[i]->weight);

					//The confidence of the new larger component is then calculated and inserted into each element in the new component
					confidence(vertexArray[edgeArray[i]->origin], constant);

					//Then each index in the vertex array that has an element in the new component now points at the new large component
					//ie: Component = (0,1,2) Then index 0,1,and 2 of the vertex array now point to Component
					struct component *pointer = vertexArray[edgeArray[i]->destination];
					while (pointer != NULL)
					{
						vertexArray[pointer->identity] = vertexArray[edgeArray[i]->origin];
						pointer = pointer->next;
					}
				}
			}
		}
	}

	//After all the components are created, we then remove the duplicate pointers
	//If multiple indexes in the vertex array pointed to the same component, it is changed so only the first one points the component and the rest become null pointers
	for (i = 0; i < vertices; i++)
	{
		int j = i + 1;
		while (j < vertices)
		{
			if (vertexArray[i] == vertexArray[j])
			{
				vertexArray[j] = NULL;
			}
			j++;
		}
	}

	//Oututs the different components
	for (i = 0; i < vertices; i++)
	{
		//If the index of the vertex array is now a NULL pointer, then we do no call the print function
		if (vertexArray[i] != NULL)
		{
			printComponent(vertexArray[i]);
		}
	}
	return 0;
}

