#ifndef __NEURAL_NETWORK_ESOINN_HPP__
#define __NEURAL_NETWORK_ESOINN_HPP__

#include <map>
#include <memory>
#include <neural_network/Common.hpp>
#include <neuron/ESoinnNeuron.hpp>
#include <set>
#include <weight_vector/WeightVectorContEuclidean.hpp>

namespace nn
{
    const std::string mcl_path = "/home/yura/local/bin/mcl ";
    const std::string output_src_mcl = "esoinn_output_mcl.tmp";

    class ESoinn
    {
      public:
        ESoinn(uint32_t num_dimensions, double age_max, uint32_t lambda, double C1, double C2, NetworkStopCriterion nnit = NetworkStopCriterion::LOCAL_ERROR, neuron::NeuronType nt = neuron::NeuronType::EUCLIDEAN);
            
        ~ESoinn();

        void trainNetwork(const std::vector<std::shared_ptr<wv::Point>>& points, std::vector<std::vector<uint32_t>>& result,
                          uint32_t num_iteration_first_layer);
        void trainNetworkNoiseReduction(const std::vector<std::shared_ptr<wv::Point>>& points, const std::vector<std::string>& labels,
                                        uint32_t num_iteration_first_layer);
        void exportEdgesFile(const std::string& filename) const;
        uint32_t findPointCluster(const wv::Point* p, const std::unordered_map<uint32_t, uint32_t>& neuron_cluster) const;
        void dumpNetwork() const;
        void printNetworkNodesFile(const std::string& filename) const;
        
        //functions for testing
        double getNeuronCoord(uint32_t num, uint32_t coord) const
        {
            return m_Neurons.at(num).getWv()->getConcreteCoord(coord);
        }

        double getWinner() const
        {
            return m_NumWinner;
        }

        double getSecWinner() const
        {
            return m_NumSecondWinner;
        }

        void setWinner(uint32_t num_winner)
        {
            m_NumWinner = num_winner;
        }

        void setSecWinner(uint32_t num_sec_winner)
        {
            m_NumSecondWinner = num_sec_winner;
        }
        
        neuron::ESoinnNeuron getNeuron(int num) const
        {
            return m_Neurons.at(num);
        }

        neuron::ESoinnNeuron& getNeuron(int num)
        {
            return m_Neurons.at(num);
        }

        void incrementLocalSignals(int num)
        {
            m_Neurons.at(num).incrementLocalSignals();
        }

        uint32_t numEmptyNeurons()
        {
            return m_NumEmptyNeurons;
        }
        
      protected:
        //Initialize network with two points from dataset (src_label - source class (label) of this two points) 
        void initialize(const std::pair<wv::Point*, wv::Point*>& points, const std::pair<std::string, std::string>& src_label = std::make_pair("0", "0"));

        std::pair<double, double> findWinners(const wv::Point* p);
        double evalThreshold(uint32_t num_neuron);
        void processNewPoint(const wv::Point* p, std::string label);
        
        void incrementEdgeAgeFromWinner();
        void updateEdgeWinSecWin();
        double midDistNeighbours(uint32_t num_neuron);
        
        void updateWeights(const wv::Point* p);
        void deleteOldEdges();
        
        bool isNodeAxis(uint32_t num_neuron);
        uint32_t findAxisForNode(uint32_t number);
        double meanDensity(uint32_t class_number) const;
        double getAlpha(double maxDensity, double meanDensity) const;
        void labelClasses(std::map<uint32_t, uint32_t>& node_axis);
        void findMergedClasses(const std::map<uint32_t, uint32_t>& node_axis, std::map<uint32_t, std::set<uint32_t>>& mergeClasses) const;
        void findAxesMapping(const std::map<uint32_t, std::set<uint32_t>>& src, std::map<uint32_t, uint32_t>& mapping) const;
        
        void updateClassLabels();
        void deleteEdgesDiffClasses();
        double calcAvgDensity();
        
        void deleteNodes();
        void deleteNeuron(uint32_t number);
        
        void findClustersMCL(std::vector<std::vector<uint32_t>>& clusters) const;
        
        //for tests
        void InsertConcreteNeuron(const wv::Point* p, const std::string& neur_class = "0"); 
        void InsertConcreteEdge(uint32_t neur1, uint32_t neur2);
      
      private:
        void trainOneEpoch(const std::vector<std::shared_ptr<wv::Point>>& points, const std::vector<std::string>& labels = std::vector<std::string> ());
        void SealNeuronVector();
        
        uint32_t m_NumDimensions;
        
        double m_AgeMax;
        uint32_t m_Lambda;

        double m_C1;
        double m_C2;
        
        NetworkStopCriterion m_NetStop;        //Type of network stop criterion
        neuron::NeuronType m_NeuronType;       //Type of the neuron

        std::vector<neuron::ESoinnNeuron> m_Neurons;

        uint32_t m_NumWinner;
        uint32_t m_NumSecondWinner;
        
        uint32_t m_NumEmptyNeurons;        
    };
}

#endif //__NEURAL_NETWORK_SOINN_HPP__
