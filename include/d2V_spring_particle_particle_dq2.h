
#include <Eigen/Dense>

//Input:
//  q - the current generalized coordinates for the mass-spring system
//  stiffness - the stiffness parameter (spring constant) for the mass-spring system
//Output:
// H - the second derivtive of the potential energy
void d2V_spring_particle_particle_dq2(Eigen::MatrixXd &H, const Eigen::VectorXd &q, double stiffness);