#include <Eigen/Dense>

//Input:
//  q - the current generalized coordinates for the mass-spring system
//  stiffness - the stiffness parameter (spring constant) for the mass-spring system
//Output:
// dV - the gradient of the potential energy with respect to the generalised coordinates.
void dV_spring_particle_particle_dq(Eigen::VectorXd &dV, const Eigen::VectorXd &q, double stiffness);