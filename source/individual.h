#include <vector>       // std::vector
#include <stdint.h>
#include "domain.h"
#include <boost/serialization/access.hpp>

class Individual  {
  private: 
    typedef int gene; 
    std::vector<gene> chromosome;
    double fitness;

    bool is_valid(); 
    int log2(int n); 
    uint32_t fnv_1a(uint8_t *bp, uint8_t *be); //begin point and beyond end
    void swap_segment(); 
    void cut_n_paste_segment(); 
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) { 
      ar & fitness;
      ar & chromosome;
    }
  protected:
  public: 
    Individual(int size, bool shuffle=false); 
    Individual(const Individual& parent, bool mutate=false); 
    Individual(const std::vector<gene>& parent_chromosome); 
    float get_fitness(){ return fitness;}
    int get_size() { return chromosome.size(); }
    void show(); 
    void show(char* s); //Prepend with this optional string
    uint32_t hash(); 
    void mutate();
    float operator - (Individual a); //Computes euclidean distance between the two chromosomes
    bool operator == (Individual a); //Compares if the two chromosomes are equal
};
