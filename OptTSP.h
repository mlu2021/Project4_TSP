// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#ifndef OPTTSP_H
#define OPTTSP_H

#include <vector>

#include "MST.h"

#define MAX_POKEMONS 40

using namespace std;

class OptTSP {
    
private:
    double bestFirstEdgeLen = INFINITY;
    double currentWeight = 0.0;
    double bestWeight = 0.0;
    vector<Pokemon> pokemons;
    vector<size_t> bestPath;
    vector<size_t> currentPath;
    bool hasDistances = false;
    double dist[MAX_POKEMONS][MAX_POKEMONS];
    
    // compute the distance between two pokemons
    double distance(const size_t &src, const size_t &parent) {
        
        if (hasDistances) {
            return dist[src][parent];
        }
        else {
            double dx = static_cast<double>(pokemons[src].x - pokemons[parent].x);
            double dy = static_cast<double>(pokemons[src].y - pokemons[parent].y);
            return sqrt(dx*dx + dy*dy);
        }
    }
    
    void calculate_distances() {
        for (size_t i = 0; i < pokemons.size(); i++) {
            for (size_t j = i+1; j < pokemons.size(); j++) {
                dist[i][j] = distance(i, j);
            }
        }
        
        // fill the other half of the matrix
        for (size_t i = 0; i < pokemons.size(); i++) {
            for (size_t j = i+1; j < pokemons.size(); j++) {
                dist[j][i] = dist[i][j];
            }
        }
        hasDistances = true;
    }
    
    // print the path
    void print_path(const vector<size_t> &path) {
        for (size_t i = 0; i < path.size(); i++) {
            cout << path[i] << " ";
        }
    }
    
    // find the weight of the computed MST
    double find_mst_weight(const size_t &permLength) {
        double retval = 0.0;
        
        vector<size_t> unvisitedNodes;
        if (permLength == currentPath.size() - 1) {
            return 0.0;
        }
        for (size_t i = permLength; i < currentPath.size(); i++) {
            unvisitedNodes.push_back(currentPath[i]);
        }
        
        // when there are only two pokenmons, their distance is mst weight
        if (unvisitedNodes.size() == 2) {
            return distance(unvisitedNodes[0], unvisitedNodes[1]);
        }
        
        
        // call the MST class to compute the mst weight with the unvisited nodes
        retval = get_mst_weight(pokemons, unvisitedNodes);
        
        return retval;
    }
    
    /**
     * find the weight of the path up to the permLength node, i.e., the visited
     * nodes.
     */
    double find_path_weight(const vector<size_t> &path, const size_t &permLength) {
        double retval = 0.0;
        
        if (permLength == 0) {
            return 0.0;
        }
        
        for (size_t i = 0; i < permLength - 1; i++) {
            retval += distance(path[i], path[i+1]);
        }
        
        // If this is a full path, also add the edge connecting the first and last nodes
        if (permLength == path.size()) {
            retval += distance(path[0], path[permLength-1]);
        }
        
        return retval;
    }
    
    /**
     * find the shorted edge from a node to the rest of the unvisited nodes
     *
     */
    double find_min_edge_weight(const size_t &node, const size_t &permLength) {
        double retval = INFINITY;
        
        for (size_t i = permLength; i < currentPath.size(); i++) {
            double temp = distance(currentPath[node], currentPath[i]);
            if (temp < retval) {
                retval = temp;
            }
        }
        
        return retval;
    }
    
    /**
     * find the minimal weight of two ending nodes of the already visited path,
     * connecting to the unvisited MST nodes
     *
     */
    double find_min_connect_weight(const size_t &permLength) {
        double retval = INFINITY;
        double firstEdgeLen = distance(currentPath[0], currentPath[permLength]);
        
        // only need to find the best edge from 0 node if it happens to be the
        // distance to the new node
        if (bestFirstEdgeLen == INFINITY || bestFirstEdgeLen == firstEdgeLen) {
            bestFirstEdgeLen = find_min_edge_weight(0, permLength);
        }
        
        retval = bestFirstEdgeLen;
        
        // double retval = find_min_edge_weight(0, permLength);
        
        if (permLength > 1) {
            retval += find_min_edge_weight(permLength-1, permLength);
        }
        
        return retval;
    }
    
    // size_t counter = 0;
    
    /**
     * check whether it is promising. We do the check incrementally.
     *
     */
    bool promising(const size_t &permLength) {
        double partialWeight = currentWeight;
        
        // If the partial solution is already worse than the best solution, prun it
        if (partialWeight > bestWeight) {
            return false;
        }
        
        partialWeight += find_min_connect_weight(permLength);
        
        if (partialWeight > bestWeight) {
            return false;
        }
        
        partialWeight += find_mst_weight(permLength);
        
        if (partialWeight < bestWeight ) {
            return true;
        }
        
        return false;
    }
    
    /**
     * genPerms to interate through all permutations of the unvisited nodes.
     *
     */
    void genPerms(size_t permLength) {
        
        if (currentPath.size() == permLength) {
            // Do something with the path
            // if this full path is shorter than the current best, replace it.
            double temp = find_path_weight(currentPath, currentPath.size());
            
            if (temp < bestWeight) {
                bestPath = currentPath;
                bestWeight = temp;
            }
            
            return;
        }
        if (!promising(permLength)) {
            return;
        }
        for (size_t i = permLength; i < currentPath.size(); ++i) {
            
            currentWeight += distance(currentPath[permLength-1], currentPath[i]);
            
            swap(currentPath[permLength], currentPath[i]);
            
            genPerms(permLength + 1);
            
            swap(currentPath[permLength], currentPath[i]);
            
            currentWeight -= distance(currentPath[permLength-1], currentPath[i]);
        } // for
        
    } // genPerms()
    
public:
    
    OptTSP(vector<Pokemon> pokes) {
        // dist = graph;
        pokemons = pokes;
        
        // save all the distances
        if (pokes.size() < MAX_POKEMONS) {
            calculate_distances();
        }
    }
    
    ~OptTSP() {}
    
    void execute(vector<size_t> &path) {
        bestPath = path;
        currentPath = path;
        bestWeight = find_path_weight(path, path.size());
        
        // call genPerms starting by fixing the starting node
        genPerms(1);
        
        // print the output
        cout << find_path_weight(bestPath, bestPath.size()) << endl;
        print_path(bestPath);
        cout << endl;
    }
};

#endif // OPTTSP_H
