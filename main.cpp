// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
//  main.cpp
//  Project 4
//
//  Created by Michelle Lu on 11/27/19.
//  Copyright © 2019 Michelle Lu. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <getopt.h>
#include <iostream>
#include <string.h>
#include <iomanip>

#include "MST.h"
#include "OptTSP.h"

using namespace std;

// run modes
enum MODE{MST, FASTTSP, OPTTSP, INVALID};
MODE mode = INVALID;

struct Location {
    int x;
    int y;
    bool visited = false;
};

// hold the input from file
size_t total;
vector<Pokemon> pokemons;

// Hold only the coordinates
vector<Location> locations;

// Hold the indexes of pokemons using the nearest distance heuristic
vector<size_t> fastTSP;

// hold the distance between pokenmons
// double** dist;

/**
 * get the command line arguments
 *
 */
void get_options(int argc, char** argv) {
    
    int option_index = 0, option = 0;
    opterr = false;
    
    struct option longOpts[] = {{"help", no_argument, nullptr, 'h'},
        {"mode", required_argument, nullptr, 'm'},
        { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "hm:", longOpts, &option_index))
           != -1) {
        switch (option) {
            case 'h':
                cout << "This program finds a path from the starting\n"
                << "location to the buried treasure and prints the\n"
                << "details of the hunt.\n"
                <<  "Usage: \'./poke\n\t[--help | -h]\n"
                <<                      "\t[--mode | -m] <\"MST\"|\"FASTTSP\"|\"OPTTSP\">\n"
                <<                      "\t< <inputfile>\'" << std::endl;
                exit(0);
                
            case 'm':
                if (strcmp(optarg, "MST") == 0) {
                    mode = MST;
                }
                else if (strcmp(optarg, "FASTTSP") == 0) {
                    mode = FASTTSP;
                }
                else if (strcmp(optarg, "OPTTSP") == 0) {
                    mode = OPTTSP;
                }
                else {
                    cerr << "Command mode option " << optarg << " not valid\n";
                    exit(1);
                }
                
                break;
            default:
                cerr << "Invalide command option\n";
                exit(1);
        }
    }
    if (mode == INVALID) {
        cerr << "Invalide command mode\n";
        exit(1);
    }
}

/**
 *  Read the data as map.
 *
 */
void read_map_data() {
    cin >> total;
    int x, y;
    char terrain = 'l';
    
    for (size_t i = 0; i < total; i++) {
        cin >> x >> y;
        
        if ( x < 0 && y < 0) {
            terrain = 's';
        }
        else if ((y == 0 && x <= 0) || (x == 0 && y <= 0)) {
            terrain = 'c';
        }
        else {
            terrain = 'l';
        }
        
        if (mode == FASTTSP) {
            Location loc = Location{x, y, false};
            locations.push_back(loc);
        }
        else {
            Pokemon pokemon = Pokemon{INFINITY, x, y, SIZE_MAX, false, terrain};
            pokemons.push_back(pokemon);
            
            if (mode == OPTTSP) {
                Location loc = Location{x, y, false};
                locations.push_back(loc);
            }
        }
    }
}

/**
 * Run the program as MST
 *
 */
void run_as_mst() {
    // build Prim MST
    build_prim_mst(pokemons);
    
}

/**
 * Compute the weight of two pokemons as the Eucledean distance.
 *
 */
double compute_edge_weight(const size_t &src, const size_t &parent) {
    double dx = static_cast<double>(locations[src].x - locations[parent].x);
    double dy = static_cast<double>(locations[src].y - locations[parent].y);
    
    return sqrt(dx*dx + dy*dy);
}

/**
 *  return the total weight of a path.
 *
 */
double compute_path_weight(const vector<size_t> &path) {
    double retval = 0.0;
    
    for (size_t i = 0; i < path.size()-1; i++) {
        // retval += dist[path[i]][path[i+1]];
        retval += compute_edge_weight(path[i], path[i+1]);
    }
    
    // retval += dist[path[0]][path[path.size()-1]];
    
    retval += compute_edge_weight(path[0], path[path.size()-1]);
    
    return retval;
}

/**
 * print the fast tsp path
 *
 */
void print_fast_tsp() {
    cout << compute_path_weight(fastTSP) << "\n";
    
    for (size_t i = 0; i < fastTSP.size(); i++) {
        cout << fastTSP[i] << " ";
    }
    cout << "\n";
}

/**
 * Find the nearest pokemon to the current node
 *
 */
size_t find_nearest_pokemon(const size_t &current) {
    double temp = INFINITY;
    double temp1 = INFINITY;
    size_t retval = SIZE_MAX;
    
    for (size_t i = 1; i < total; i++) {
        if (!locations[i].visited) {
            temp1 = compute_edge_weight(current, i);
            if (temp1 < temp) {
                // temp = dist[current][i];
                temp = temp1;
                retval = i;
            }
        }
    }
    
    return retval;
}

/**
 * build a path by finding the nearest node
 *
 */
void run_nearest_insertion() {
    size_t visitedCount = 1;
    size_t next = 0;
    
    // deal with some edge cases first
    if (total == 0) return;
    
    // if there are less than 4 (at most 3) pokemons, then any path is optimal so
    // just pick the order as they are read in from input
    if (total < 4) {
        for (size_t i = 0; i < total; i++) {
            fastTSP.push_back(i);
        }
        return;
    }
    
    // Pick a starting pokemon and push to the path visitedCount was set to 1
    locations[0].visited = true;
    fastTSP.push_back(0);
    
    // Find the nearest node
    next = find_nearest_pokemon(0);
    locations[next].visited = true;
    fastTSP.push_back(next);
    
    visitedCount = 2;
    
    // Start with 0, 1, 2
    // fastTSP.push_back(0);
    // locations[0].visited = true;
    //
    // fastTSP.push_back(1);
    // locations[1].visited = true;
    //
    // fastTSP.push_back(2);
    // locations[1].visited = true;
    //
    // fastTSP.push_back(0);
    //
    // visitedCount = 3;
    
    while (visitedCount < total) {
        
        // go through the unvisited nodes, and decide where to insert in between
        // an edge in the partial tour
        for (size_t k = 0; k < total; k++) {
            
            // if the node is visited, go to check the next node
            if (locations[k].visited) {
                // cout << "pokemons[" << k << "] has been visited!" << endl;
                continue;
            }
            
            // for each unvisited node, find an edge to insert with minimal
            // triangle difference
            double temp = INFINITY;
            size_t toInsert = 0;
            double temp1 = INFINITY;
            
            for (size_t i = 0; i < fastTSP.size()-1; i++) {
                temp1 = compute_edge_weight(fastTSP[i], k)
                +compute_edge_weight(fastTSP[i+1], k)
                -compute_edge_weight(fastTSP[i], fastTSP[i+1]);
                
                if (temp1 < temp) {
                    temp = temp1;
                    toInsert = i;
                }
            }
            // this shouldn't happen!
            if (temp == INFINITY) {
                cerr << "Not finding the minimal, something went wrong!" << endl;
                exit(1);
            }
            
            // insert pokemons[k] in between fastTSP[i] and fastTSP[i+1]
            auto iPos = fastTSP.begin() + static_cast<long>(toInsert+1);
            fastTSP.insert(iPos, k);
            
            // mark the inserted pokemon as visited
            locations[k].visited = true;
            
            // increase the visited count
            visitedCount++;
        }
    }
}

/**
 * calculate distances among all nodes.
 *
 */
//  void calculate_distances() {
//     // allocate the space for the matrix
//     dist = new double*[total];
//
//     // fill in the distance between each node
//     for (size_t i = 0; i < total; i++) {
//         dist[i] = new double[total];
//
//         for (size_t j = i+1; j < total; j++) {
//             dist[i][j] = compute_edge_weight(i, j);
//         }
//     }
//
//     // fill the other half of the matrix
//     for (size_t i = 0; i < total; i++) {
//         for (size_t j = i+1; j < total; j++) {
//             dist[j][i] = dist[i][j];
//         }
//     }
//  }
//
// void delete_dist() {
//
//   // fill in the distance between each node
//   for (size_t i = 0; i < total; i++) {
//       delete [] dist[i];
//   }
//
//   delete [] dist;
// }

/**
 * Run the program as FASTTSP.
 *
 */
void run_as_fast_tsp() {
    
    // calculate the distance matrics to keep the distance among pokemons
    // calculate_distances();
    
    // run nearest insertion
    run_nearest_insertion();
}

/**
 * Run the programs as OPTTSP.
 *
 */
void run_as_opt_tsp() {
    
    // calculate distances among any two nodes
    // calculate_distances();
    
    // generate the fastTSP as upper bound
    run_as_fast_tsp();
    
    // call the OptTSP class to execute on the fastTSP
    OptTSP *optTsp = new OptTSP(pokemons);
    
    optTsp->execute(fastTSP);
    
    delete optTsp;
}

/**
 *  Main function.
 *
 */
int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cout << std::setprecision(2); //Always show 2 decimal places
    cout << std::fixed; //Disable scientific notation for large numbers
    
    // get the options
    get_options(argc, argv);
    
    // read data from input file
    read_map_data();
    
    switch (mode) {
        case MST:
            run_as_mst();
            break;
        case FASTTSP:
            run_as_fast_tsp();
            
            // print the path
            print_fast_tsp();
            
            // delete_dist();
            
            break;
        case OPTTSP:
            run_as_opt_tsp();
            
            // delete_dist();
            
            break;
        default:
            break;
    }
    
    return 0;
}
