#include <boost/test/unit_test.hpp>
#include <neural_network/KohonenNN.hpp>
#include <weight_vector/WeightVectorContEuclidean.hpp>

using namespace boost::unit_test;
using namespace nn;

namespace 
{
    const uint32_t TestNumDimensions = 3;
    const uint32_t TestNumClusters = 5;
}

class TestKohonenNN: public KohonenNN
{
    void initializeNN()
    {
        cont::StaticArray<double> coords[TestNumClusters];
        for (uint32_t i = 0; i < TestNumClusters; i++)
            coords[i] = cont::StaticArray<double>(TestNumDimensions);
            
        //Fill mas by concrete values
        //five points: (2, 3, 4) (4, 3, 3) (1, 2, -1) (-2, 4, 0) (5, 0, 0) 
        coords[0][0] = 2;
        coords[0][1] = 3;
        coords[0][2] = 4;
        coords[1][0] = 4;
        coords[1][1] = 3;
        coords[1][2] = 3;
        coords[2][0] = 1;
        coords[2][1] = 2;
        coords[2][2] = -1;
        coords[3][0] = -2;
        coords[3][1] = 4;
        coords[3][2] = 0;
        coords[4][0] = 5;
        coords[4][1] = 0;
        coords[4][2] = 0;
        
        cont::StaticArray<neuron::KohonenNeuron> neurons(TestNumClusters);
        
        for (uint32_t i = 0; i < neurons.size(); i++)
        {
            switch(neuronType())
            {
                case neuron::NeuronType::EUCLIDEAN:
                {
                    wv::WeightVectorContEuclidean* sWeightVector = new wv::WeightVectorContEuclidean(coords[i]);  
                    //delete[] neurons;
                    neurons[i] = neuron::KohonenNeuron(sWeightVector); //create neurons 
                    break;
                }
                default:
                {
                    throw std::runtime_error("Can't construct neural network. Incorrect neuron type.");
                    break;
                }
            }
        }
        m_Neurons = neurons;
    }

public:
    TestKohonenNN(NetworkInitType nnit = NetworkInitType::RANDOM, neuron::NeuronType nt = neuron::NeuronType::EUCLIDEAN)
    :KohonenNN(false, TestNumClusters, TestNumDimensions, nnit, nt)
    {
        initializeNN();        
    }
    
    void testFindWinner(const wv::Point* p)
    {
        findWinner(p);
    }

    ~TestKohonenNN()
    {
        for (uint32_t i = 0; i < numClusters(); i++)
            if (m_Neurons[i].getWv() != NULL)
            {
                delete m_Neurons[i].getWv();
                m_Neurons[i].setZeroPointer();
            }
    }    
};

BOOST_AUTO_TEST_SUITE(KohonenNeuralNetwork)
BOOST_AUTO_TEST_CASE(test_InitializeNN)
{
    KohonenNN knn1(5, 3);
    for (uint32_t j = 0; j < 5; j++)
    {
        wv::AbstractWeightVector* awv = knn1.getNeuron(j).getWv();
        wv::WeightVectorContEuclidean* wv = dynamic_cast<wv::WeightVectorContEuclidean*>(awv);
        BOOST_REQUIRE(wv != NULL);

        BOOST_CHECK(awv->getNumDimensions() == 3);
        for (uint32_t i = 0; i < awv->getNumDimensions(); i++)
            BOOST_CHECK(awv->getConcreteCoord(i) >= 0 and awv->getConcreteCoord(i) <= 1);
    }
}    
BOOST_AUTO_TEST_SUITE_END()
    
BOOST_AUTO_TEST_SUITE(TestKohonenNeuralNetwork)
BOOST_AUTO_TEST_CASE(test_testInitialization)
{
    TestKohonenNN tknn;
    for (uint32_t j = 0; j < tknn.numClusters(); j++)
    {
        wv::AbstractWeightVector* awv = tknn.getNeuron(j).getWv();
        
        //check if awv has correct type
        wv::WeightVectorContEuclidean* wv = dynamic_cast<wv::WeightVectorContEuclidean*>(awv);
        BOOST_REQUIRE(wv != NULL);

        BOOST_CHECK(wv->getNumDimensions() == 3);
    }
    
    //check concrete values
    wv::AbstractWeightVector* wv = tknn.getNeuron(0).getWv();
    BOOST_CHECK(wv->getConcreteCoord(0) == 2);
    BOOST_CHECK(wv->getConcreteCoord(1) == 3);
    BOOST_CHECK(wv->getConcreteCoord(2) == 4);
    
    wv = tknn.getNeuron(1).getWv();
    BOOST_CHECK(wv->getConcreteCoord(0) == 4);
    BOOST_CHECK(wv->getConcreteCoord(1) == 3);
    BOOST_CHECK(wv->getConcreteCoord(2) == 3);

    wv = tknn.getNeuron(2).getWv();
    BOOST_CHECK(wv->getConcreteCoord(0) == 1);
    BOOST_CHECK(wv->getConcreteCoord(1) == 2);
    BOOST_CHECK(wv->getConcreteCoord(2) == -1);
    
    wv = tknn.getNeuron(3).getWv();
    BOOST_CHECK(wv->getConcreteCoord(0) == -2);
    BOOST_CHECK(wv->getConcreteCoord(1) == 4);
    BOOST_CHECK(wv->getConcreteCoord(2) == 0);

    wv = tknn.getNeuron(4).getWv();
    BOOST_CHECK(wv->getConcreteCoord(0) == 5);
    BOOST_CHECK(wv->getConcreteCoord(1) == 0);
    BOOST_CHECK(wv->getConcreteCoord(2) == 0);    
}

BOOST_AUTO_TEST_CASE(test_findWinner)
{
    TestKohonenNN tknn;
    
    cont::StaticArray<double> arr1(3);
    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;
    
    wv::WeightVectorContEuclidean wv1(arr1);
    
    //right data
    BOOST_REQUIRE_NO_THROW(tknn.testFindWinner(&wv1));
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(0).curPointDist() * 100), 173);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(1).curPointDist() * 100), 316);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(2).curPointDist() * 100), 400);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(3).curPointDist() * 100), 469);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(4).curPointDist() * 100), 538);    
    BOOST_CHECK_EQUAL(tknn.numNeuronWinner(), 0);

    //wrong type
    wv::AbstractWeightVector*awv = NULL;
    BOOST_CHECK_THROW(tknn.testFindWinner(awv), std::bad_typeid);

    cont::StaticArray<double> arr2(3);
    arr2[0] = 4;
    arr2[1] = 3;
    arr2[2] = 3;
    
    wv::WeightVectorContEuclidean wv2(arr2);
    //another right variant
    BOOST_REQUIRE_NO_THROW(tknn.testFindWinner(&wv2));
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(0).curPointDist() * 100), 223);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(1).curPointDist() * 100), 0);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(2).curPointDist() * 100), 509);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(3).curPointDist() * 100), 678);
    BOOST_CHECK_EQUAL(int(tknn.getNeuron(4).curPointDist() * 100), 435);    
    BOOST_CHECK_EQUAL(tknn.numNeuronWinner(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
