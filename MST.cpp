// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <vector>
#include <iostream>

#include "MST.h"

using namespace std;

double totalWeight = 0.0;

vector<size_t> mst;
bool useTerrain = true;

/**
 * Calculate the sqaure distance of two pokemons.
 *
 */
double square_sum(const Pokemon &p1, const Pokemon &p2) {
    // need to cast them to double to avoid overflow with int
    double dx = static_cast<double>(p1.x - p2.x);
    double dy = static_cast<double>(p1.y - p2.y);
    
    return (dx*dx + dy*dy);
}

/**
 * Print the MST
 *
 */
void print_prim_mst(vector<Pokemon> &pokemons) {
    cout << totalWeight << "\n";
    
    for (size_t i = 0; i < mst.size(); i++) {
        if (mst[i] < pokemons[mst[i]].parent) {
            cout << mst[i] << " " << pokemons[mst[i]].parent << "\n";
        }
        else {
            cout << pokemons[mst[i]].parent << " " << mst[i] << "\n";
        }
    }
}

/**
 * Find the square value of two pokemons
 *
 */
double find_mst_square_sum(vector<Pokemon> &pokemons, const size_t &from, const size_t &to) {
    double retval = INFINITY;
    
    switch (pokemons[from].terrain) {
        case 's':
            if (pokemons[to].terrain == 'c' || pokemons[to].terrain == 's') {
                retval = square_sum(pokemons[from], pokemons[to]);
            }
            break;
            
        case 'c':
            retval = square_sum(pokemons[from], pokemons[to]);
            break;
        case 'l':
            if (pokemons[to].terrain == 'l' || pokemons[to].terrain == 'c') {
                retval = square_sum(pokemons[from], pokemons[to]);
            }
            break;
    }
    
    return retval;
}

/**
 * Find the index of the pokemon whose weight is smallest, among unvisited pokemons.
 *
 */
size_t find_smallest_pokemon(vector<Pokemon> &pokemons) {
    double temp = INFINITY;
    size_t retval = SIZE_MAX;
    
    for (size_t i = 0; i < pokemons.size(); i++) {
        // cout << "Is pokemons[" << i << "] been visited? = " << pokemons[i].visited << ", dist = " << pokemons[i].distance << ", temp = " << temp << endl;
        if (!pokemons[i].visited && pokemons[i].distance < temp) {
            // cout << " pokenmon[" << i << "] unvisited, adding its distance with: " << pokemons[i].distance << endl;
            temp = pokemons[i].distance;
            retval = i;
        }
    }
    if (temp != INFINITY) {
        totalWeight += sqrt(temp);
    }
    else {
        cerr << "Cannot construct MST\n";
        exit(1);
    }
    
    return retval;
}

/**
 * Use the square value as the distance/weight for all ajacent nodes.
 *
 */
void set_ajacent_distance(vector<Pokemon> &pokemons, const size_t &v) {
    
    for (size_t w = 0; w < pokemons.size(); w++) {
        // skip the visited pokemon
        if (pokemons[w].visited) {
            continue;
        }
        
        // test whether dw is greater then distance (v, w)
        double squareSum = 0.0;
        
        if (useTerrain) {
            squareSum = find_mst_square_sum(pokemons, v, w);
        }
        else {
            squareSum = square_sum(pokemons[v], pokemons[w]);
        }
        
        // set dw = distance(v, w), and pw to v
        if (pokemons[w].distance > squareSum) {
            pokemons[w].distance = squareSum;
            pokemons[w].parent = v;
        }
    }
}

/**
 * Build MST using Prim's algorithm.
 *
 */
void build_prim_mst(vector<Pokemon> &pokemons) {
    size_t count = 0;
    // size_t current = 0;
    size_t next = 0;
    
    // Pick a starting pokemon
    pokemons[0].distance = 0;
    
    while (count < pokemons.size()) {
        
        // select a pokemon with the smallest distance
        next = find_smallest_pokemon(pokemons);
        
        // set the select pokemon as visited
        // cout << "set pokemons[" << next << "] to be visited." << endl;
        pokemons[next].visited = true;
        
        // keep track the MST. Skip the starting pokemon since it has no parent
        if (count > 0) {
            mst.push_back(next);
        }
        // update distance for all ajacent pokemons
        set_ajacent_distance(pokemons, next);
        
        // move the counter forward
        count++;
        // current = next;
    }
    
    if (useTerrain) {
        print_prim_mst(pokemons);
    }
}

double get_mst_weight(vector<Pokemon> &pokemons, vector<size_t> &vertices) {
    // reset the global variables
    if (!mst.empty()) {
        mst.clear();
    }
    totalWeight = 0.0;
    useTerrain = false;
    
    vector<Pokemon> unvisitedPokemons;
    
    // Need to create copies of the unvisited pokemons here because pokemons is passed
    // in a parameter, and it will go out of scope after the function call
    for (size_t i = 0; i < vertices.size(); i++) {
        Pokemon pokemon = Pokemon{INFINITY, pokemons[vertices[i]].x, pokemons[vertices[i]].y, SIZE_MAX, false, 'l'};
        unvisitedPokemons.push_back(pokemon);
    }
    
    // build the Pim MST using just the unvisited pokemons
    build_prim_mst(unvisitedPokemons);
    
    return totalWeight;
}
