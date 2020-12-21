// PROJECT IDENTIFIER: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#ifndef MST_H
#define MST_H

#include <cmath>
#include <vector>

using namespace std;

struct Pokemon {
    double distance = INFINITY;
    int x;
    int y;
    size_t parent = SIZE_MAX;
    bool visited = false;
    char terrain = 'l';
};

double square_sum(const Pokemon &p1, const Pokemon &p2);

double get_mst_weight(vector<Pokemon> &pokemons, vector<size_t> &vertices);

void build_prim_mst(vector<Pokemon> &pokemons);

#endif // MST_H
