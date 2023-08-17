#include <iostream>
#include "runlength_encoding.h"

runlength_encoding::runlength_encoding(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

std::string runlength_encoding::getTag(char key){
    return (myTagTable->find(key))->second;
}

void runlength_encoding::CompressBlock(int z_start, int x_start, int y_start) {

    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;
    

    
    // Implement the specific compression algorithm for a block here

    //i = depth, j = vertical, k = horizontal

    //TODO: ask if y_start can == y_end
    for (int i = z_start; i < z_end; i++){
        for (int j = x_start; j < x_end; j++){
            char saved_key = (*mySlices)[i][j][y_start];
            int key_count = 1;
            int starting_k = y_start;

            for (int k = y_start+1; k < y_end; k++){
                char current_key = (*mySlices)[i][j][k];
                if(saved_key == current_key)
                    key_count++;
                else{
                    std::cout<<starting_k<<","<<j<<","<<i+current_level<<","<<key_count<<",1,1,"<<getTag(saved_key)<<std::endl;
    
                    saved_key = current_key;
                    starting_k = k;
                    key_count = 1;                    
                }
            }
            std::cout<<starting_k<<","<<j<<","<<i+current_level<<","<<key_count<<",1,1,"<<getTag(saved_key)<<std::endl;
        }
    }

}