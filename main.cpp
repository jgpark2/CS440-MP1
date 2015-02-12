#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <unistd.h> //for sleep
#include <stack>
#include <vector>
#include <math.h>

using namespace std;

#define DEBUG true

struct cell{
	
	bool wall;
	bool visited;
	cell* previous;
	
	int x;
	int y;

	double straight_dist;
	
	cell() : visited(false), previous(NULL), straight_dist(0) {}
};

struct maze_props{

	int init_row;
	int init_col;
	int goal_row;
	int goal_col;
	int num_rows;
	int num_cols;
	//
	cell* start;
	cell* goal;
};

struct node{
	node* parent;

	int row;
	int col;

	node* childA;
	node* childB;
	node* childC;
	node* childD;
};

/*
int calc_path_cost(node* curr_node, maze_props &props, int curr_cost)
{
	if(curr_node->row == props.goal_row && curr_node->col == props.goal_col)
	{
		return curr_cost;
	}


}
*/

void print_solution(cell** maze, maze_props props) {
	cout << "Solution: " << endl;
	
	if(!(props.goal)->visited) {
		cout << "Could not find path from starting point to finish point!" << endl;
		return;
	}
	
	cell* current_cell = props.goal;
	
	vector<cell*> path;
	int path_length = 0;
	
	while(current_cell != NULL) {
		if (DEBUG)
			cout << current_cell->x << "," << current_cell->y << "    ";
		usleep(1500);
		path.push_back(current_cell);
		++path_length;
		current_cell = current_cell->previous;
	}
	
	cout << endl;
	cout << "Found path of length " << path_length << endl;
	for(int i = path_length-1; i>=0; i--) {
		cout << "(" << path[i]->x << "," << path[i]->y << ") -> " ;
	}
	cout << endl;
}


void print_progress(maze_props props, cell** maze, int expansions)
{
	//Progress In Text
	for(int i=0; i<props.num_rows; i++) {
		for (int j=0; j<props.num_cols; j++) {
			if(i==props.start->y && j==props.start->x)
			{
				cout << 'P';
				continue;
			}
			if(i==props.goal->y && j==props.goal->x)
			{
				cout << 'G';
				continue;
			}

			cout << ((maze[i][j].wall)? '%':(maze[i][j].visited? '.':' ' )) ;
		}
		cout << endl;
	}
	//cout << "Current depth: " << depth << endl;
	cout << "Nodes expanded: " << expansions << endl;
	//cout << "Cost: " << cost << endl;

	for(int i=0; i<12; i++)
	{
		cout << endl;
	} 
	usleep(35000); //sleep 0.25 sec
}

bool frontierCheckPush_bfs(queue<cell*>& frontier, cell** maze, maze_props props, cell* previous_cell, int y, int x) {
	if (x<0 || y<0 || x>props.num_cols-1 || y>props.num_rows-1) {
		return false;
	}
	
	cell* candidate_cell = &(maze[y][x]);
	
	if (candidate_cell->visited)
		return false;
		
	if (!candidate_cell->wall) {
		frontier.push(candidate_cell);
		candidate_cell->previous = previous_cell;
		return true;
	}
	
	return false;
}

//the only difference with the above is that the first parameter is a stack instead of a queue
bool frontierCheckPush_dfs(stack<cell*>& frontier, cell** maze, maze_props props, cell* previous_cell, int y, int x) {

	if (x<0 || y<0 || y>props.num_rows-1 || x>props.num_cols-1) {
		return false;
	}

	
	cell* candidate_cell = &(maze[y][x]);
	
	if (candidate_cell->visited)
		return false;
		
	if (!candidate_cell->wall) {
		frontier.push(candidate_cell);
		candidate_cell->previous = previous_cell;
		return true;
	}
	
	return false;
}

double calc__linear_dist(cell* curr, cell* dest)
{
	int arg_1 = dest->x - curr->x;
	int arg_2 = dest->y - curr->y;
	arg_1 = pow(arg_1, 2);
	arg_2 = pow(arg_2, 2);
	return sqrt(arg_1 + arg_2);
}


void process_line(cell** maze, string curr_line, int curr_height, maze_props &props)
{

	maze[curr_height] = new cell[curr_line.length()];
	int curr_col = 0;
	for(string::iterator it = curr_line.begin(); it != curr_line.end(); it++)
	{
		char test = *it;
		maze[curr_height][curr_col].x = curr_col;
		maze[curr_height][curr_col].y = curr_height;
		
		if(test == '%')
		{
			//wall
			maze[curr_height][curr_col].wall = true;

		}
		else if(test == 'P')
		{
			//starting point
			maze[curr_height][curr_col].wall = false;
			
			props.start = &(maze[curr_height][curr_col]);
			//
			props.init_row = curr_height;
			props.init_col = curr_col;
		}
		else if(test == '.')
		{
			//goal
			maze[curr_height][curr_col].wall = false;
			
			props.goal = &(maze[curr_height][curr_col]);
			//
			//props.goal_row = curr_height;
			//props.goal_col = curr_col;
		}
		else
		{
			//open space 
			maze[curr_height][curr_col].wall = false;
		}
		
		//maze[curr_height][curr_col].visited = false;
		curr_col++;
	}

}

/* Doesn't return a solution yet, since it doesn't keep track of the nodes taken */
void bfs(cell** maze, maze_props props) {

	cell* current_cell;
	
	queue< cell* > frontier;
	frontier.push( props.start );

	//int depth = 0; //not keeping track
	int expansions = 0;
	//int cost = 0;
	
	while(!frontier.empty()) {
		current_cell = frontier.front();
		current_cell->visited = true;
		frontier.pop();
		
		expansions++; 
		
		if (current_cell == props.goal) {
			//We done did eet
			//continue;
			break;
		}
		
		//Add all adjacent cells to frontier
		int cx = current_cell->x, cy = current_cell->y;
		
		frontierCheckPush_bfs(frontier, maze, props, current_cell, cy,	cx+1) ;
		frontierCheckPush_bfs(frontier, maze, props, current_cell, cy+1,cx	) ;
		frontierCheckPush_bfs(frontier, maze, props, current_cell, cy,	cx-1) ;
		frontierCheckPush_bfs(frontier, maze, props, current_cell, cy-1,cx	) ;
		
		//Progress In Text
		if(DEBUG)
			print_progress(props, maze, expansions);//, found);
	}
	
	print_solution(maze, props);
}


void dfs(cell** maze, maze_props props)
{
	cell* current_cell;
	
	stack< cell* > frontier;
	frontier.push( props.start );
	int expansions = 0;

	while(!frontier.empty())
	{
		//cout << "while" << endl;
		current_cell = frontier.top();
		frontier.pop();
		expansions++;

		if(current_cell == props.goal)
		{
			break;
		}

		if(current_cell->visited==false)
		{
			//cout << "iteration" << endl;
			current_cell->visited=true;
			int cx = current_cell->x, cy = current_cell->y;
			frontierCheckPush_dfs(frontier, maze, props, current_cell, cy,	cx+1) ;
			frontierCheckPush_dfs(frontier, maze, props, current_cell, cy+1,cx	) ;
			frontierCheckPush_dfs(frontier, maze, props, current_cell, cy,	cx-1) ;
			frontierCheckPush_dfs(frontier, maze, props, current_cell, cy-1,cx	) ;
		}

		if(DEBUG)
			print_progress(props, maze, expansions);
	}
	print_solution(maze, props);

}



int main(void)
{

	int num_cols = 0;
	int num_rows = 0;

	ifstream myfile ("smallMaze.txt");
	//cout << "Unable to open file." << endl;
	//std::ofstream outfile ("output_small.txt");

	cell** maze;
	maze_props props;
	//props.init_row = -1;
	//props.init_col = -1;
	//props.goal_row = -1;
	//props.goal_col = -1;
	//node* root = NULL;
	
	if(myfile.is_open())
	{
		string curr_line;

		if(getline(myfile, curr_line) == NULL) {
			cout << "Error: no lines read " << endl;
			return 1;
		}
		
		num_cols = curr_line.length();
		maze = new cell*[num_cols];
		process_line(maze, curr_line, num_rows, props);
		num_rows++;

		while(getline(myfile, curr_line))
		{
			process_line(maze, curr_line, num_rows, props);
			num_rows++;
		}
	}
	props.num_rows = num_rows;
	props.num_cols = num_cols;



	if(DEBUG) {
		cout << "The init is at row: " << props.start->x << " and col: " << props.start->y << endl;
		cout << "The goal is at row: " << props.goal->x << " and col: " << props.goal->y << endl;
	}
	
	
	for(int i=0; i<num_rows; i++) {
		for (int j=0; j<num_cols; j++) {
			cout << ((maze[i][j].wall)? 'W':' ') ;
		}
		cout << endl;
	}
	
	//bfs(maze, props);
	dfs(maze, props);


	//double test_dist = calc__linear_dist(props.start, props.goal);
	//cout << "Linear distance from start to goal: " << test_dist << endl;
}
