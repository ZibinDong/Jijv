#include <iostream>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <ctime>
#define infinity MAXFLOAT
using namespace std;
using namespace Eigen;

/* adaptive PID parameters learning machine, powered by PSO 
@param Eigen::Vector3f param_min
@param Eigen::Vector3f param_max
*/
class PSO{
public:
    vector<Eigen::Vector3f> local_best_params;
    Eigen::Vector3f global_best_params;
    vector<float> local_best_t;
    float global_best_t = infinity;

    vector<Eigen::Vector3f> velocity;

    int particle_num;
    // PSO parameters: w c1 c2
    // where w is the weight of staying still, c1 is about local best interfere, c2 is about global best interfere
    float PSO_params[3] = {0.3, 0.252, 0.442};

    vector<Eigen::Vector3f> run(vector<float> run_t, vector<Eigen::Vector3f> run_params, float scale = 1.0);
    
    PSO(Eigen::Vector3f param_min, Eigen::Vector3f param_max, int particle_num = 10);
    ~PSO();
};

// initialize the particles with a given spawning area
PSO::PSO(Eigen::Vector3f param_min, Eigen::Vector3f param_max, int particle_num){
    this->particle_num = particle_num;
    Eigen::Vector3f mean = (param_max+param_min)/2.0;
    Eigen::Vector3f range = (param_max-param_min)/2.0;
    srand((unsigned)time(NULL));
    for(int i = 0; i < particle_num; i++){
        Eigen::Vector3f particle = (Eigen::Vector3f::Random(3).array()*range.array())+mean.array();
        this->local_best_params.push_back(particle);
        this->velocity.push_back(Eigen::Vector3f::Zero(3));
        this->local_best_t.push_back(1000);
    }
    return;
}

PSO::~PSO(){
}

/* update new parameters according to given run time, you can set a scale used to modify the updated parameters' range 
@param vector<float> run_t
@param vector<Eigen::Vector3f> run_params
*/
vector<Eigen::Vector3f> PSO::run(vector<float> run_t, vector<Eigen::Vector3f> run_params, float scale){
    vector<Eigen::Vector3f> new_params;
    // update best run time
    // compute new PID params
    for(int i = 0; i < this->particle_num; i++){
        if(run_t[i] < this->local_best_t[i]){
            this->local_best_t[i] = run_t[i];
            this->local_best_params[i] = run_params[i];
            if(run_t[i] < this->global_best_t){
                this->global_best_t = run_t[i];
                this->global_best_params = run_params[i];
            }
        }
        this->velocity[i] = this->PSO_params[0]*this->velocity[i]+
                            this->PSO_params[1]*(this->local_best_params[i]-run_params[i])+
                            this->PSO_params[2]*(this->global_best_params-run_params[i]);
        new_params.push_back(scale*this->velocity[i]+run_params[i]);
    }
    return new_params;
};

