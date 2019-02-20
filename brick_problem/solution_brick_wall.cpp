/***************************************************************************************************
Brick By Brick Problem

Author Name: Lin Jia
Date: 2019-2-13

Description:
Consider the problem of building a wall out of 2 X 1 and 3 X 1 bricks (horizontal vertical dimensions)
such that, for extra strength, the gaps between horizontally-adjacent bricks never line up in consecutive
layers, i.e. never form a "running crack".
There are eight ways of forming a crack-free 9 3 wall, written W(9,3) = 8.
Calculate W(32,10).

Answer: 806844323190414

Execution time : 189.502 s

Notes:
My program could break into 3 steps:
1:  Use function all_possible_layers(w) generate all possible ways of making the wall. The function returns
    A 2-D vector, a vector of vector of integers of 2s or 3s.
2:  Use function get_compatible_layers to get a vector of vectors of indexes of all compatible layers. Here
    I wrote 2 helper functions: position_sub_total() and no_conflict().
    position_sub_total() returns a set A, in which every element A(i) is a sum of all the elements before a
    brick index i in one layer.
    no_conflict() helps to compare if 2 layers in position sub-total has common element(s) or not.
    Using memoization to optimize calculation.
3:  Use dynamic programming recursively calculate every layer's possible ways for height of h walls. Finally
    add all possible ways together.

****************************************************************************************************/


#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <unordered_map>

using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::set;
using std::set_intersection;
using std::inserter;
using std::prev;
using std::unordered_map;
using std::pair;
using std::make_pair;

typedef vector<unsigned int> Layer;
typedef vector<unsigned long long int> LongLayer;
typedef set<unsigned int> Distinct;

vector<Layer> all_possible_layers( const unsigned int );  // create all possible layers
vector<Layer> get_compatible_layers( const vector<Layer>&, const unsigned long int, const unsigned int ); // get all compatible layers
Distinct position_sub_total( const Layer&, const unsigned int ); // helper function to get all subtotal after adding up each brick length
bool no_conflict( const Distinct &, const Distinct &); // compare if two sets have common element


int main()
{
    unsigned int w = 32;
    unsigned int h = 10;
    unsigned int layerNum;

    vector<Layer> layers;

    layers = all_possible_layers( w );
    layerNum = layers.size();

    vector<Layer> compatibleList; // hold compatible layer index for each layer index

    compatibleList = get_compatible_layers( layers, layerNum, w );

    // declare a vector holding all elements with initial value of 1, with size of total possible ways of one layer of bricks.
    LongLayer counter( layerNum, 1 );

    for ( unsigned int i = 1; i < h; ++i )
    {
        LongLayer tempCounter (layerNum, 0);

        for (unsigned int j = 0; j < layerNum; ++j)
        {
            for (auto h : compatibleList[j])
            {
                tempCounter[j] += counter[h];
            }
        }
        counter = tempCounter;
    }

    unsigned long long int total = 0;

    for(auto &i : counter)
    {
        total += i;
    }

    cout << total << "\n";
    return 0;

}


vector<Layer> all_possible_layers( const unsigned int w )
{
    map<unsigned char, Layer> base; // base case for recursive function
    vector<Layer> addTwo; // to hold all layers from all_possible_layers(w-2)
    vector<Layer> addThree; // to hold all layers from all_possible_layers(w-3)
    vector<Layer> result; // to hold all the possible brick combination


    base[2] = {2};
    base[3] = {3};

    if(w == 0 || w == 1)
        return {};
    else if (w == 2 || w == 3)
        return {base[w]};

    addTwo = all_possible_layers( w - 2 );
    addThree = all_possible_layers( w - 3);

    if( w >= 2)
    {
        for(auto &i : addTwo)
        {
            i.push_back(2);
        }

        for(auto &j : addThree)
        {
            j.push_back(3);
        }
    }

    result.reserve( addTwo.size() + addThree.size() ); // preallocate memory
    result.insert( result.end(), addTwo.begin(), addTwo.end() );
    result.insert( result.end(), addThree.begin(), addThree.end() );

    return result;
}
Distinct position_sub_total(const Layer &row, const unsigned int w)
{
    Distinct subTotal {};
    int temp = 0;
    for(auto i : row)
    {
        temp += i;
        subTotal.insert(temp);
    }
    //remove the last element from set, since this is the width of the wall which are same for all the layers
    subTotal.erase(prev(subTotal.end()));
    return subTotal;
}
bool no_conflict(const Distinct &one, const Distinct &two)
{
    set<int> intersect;

    set_intersection(one.begin(),one.end(),two.begin(),two.end(),std::inserter(intersect,intersect.begin()));

    return intersect.empty();
}

/***
store each layer's compatible layer(s) index
*/
vector<Layer> get_compatible_layers(const vector<Layer> &allLayers, const unsigned long int numPossibleLayers, const unsigned int w)
{

    vector<Distinct> totalInPositionList;
    vector<Layer> compatibleLayerIndexList(numPossibleLayers,Layer());
    compatibleLayerIndexList.reserve(numPossibleLayers);

    //use an map to cache calculated results
    map<pair<int, int>, bool> cache;
    map<int, Distinct> cacheList;

    for (unsigned int i = 0; i < numPossibleLayers; ++i)
    {
        if ( i == 0 )
        {
            cacheList[i] = position_sub_total(allLayers[i], w);
        }
        for (unsigned int j = 0; j < numPossibleLayers; ++j)
        {
            if (cacheList.find(j)== cacheList.end())
            {
                cacheList[j] = position_sub_total(allLayers[j], w);
            }

            else if(i == j)
                continue;

            else if(cache.find(make_pair(i,j))!= cache.end())
            {
               if (cache[make_pair(i,j)])
                    {
                        compatibleLayerIndexList[i].push_back(j);
                        continue;
                    }
            }

            cache[make_pair(i,j)] = no_conflict(cacheList[i], cacheList[j]);
            cache[make_pair(j,i)] = cache[make_pair(i,j)];

            if (cache[make_pair(i,j)])
            {
                compatibleLayerIndexList[i].push_back(j); // any compatible j for i will put in a temp Layer
            }
        }
    }

    return compatibleLayerIndexList;
}
