#include <Eigen/Dense>

//force - function which computes the gradient of the potential energy function as a function of the state (q, qdot).
//stiffness - function that computes the stiffness (- hessian ) of the potential energy
template<typename FORCE, typename STIFFNESS> 
inline void backward_euler(Eigen::VectorXd &q, Eigen::VectorXd &qdot, double dt, double mass,  FORCE &force, STIFFNESS &stiffness) {


}