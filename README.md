# High order finite differences
The finite difference method is a simple but powerful technique for solving regular PDE problems. It shines when:
  * the domain is an hypercube
  * the mesh is equispaced
  * the solution is regular

its simplicity makes it ideal as the first method presented in numerical analysis courses for PDEs, but there are some niches in the industry where finite differences provide great performance.


## Motivation for the study
My focus is in high performance computing, so I'm no stranger in taking a numerical code and manipulate it to expose parallelism, vectorization or redundant work. Sometimes I feel that engineering effort is spent on optimizing a particular solver and not on searching for better solution methods. This is in part due to limited understanding of the problem domain and the space of numerical methods.

Solving a PDE with finite differences allow me to explore the optimality of multiple solvers in terms of:
  * convergence speed
  * robustness to numerical errors
  * cognitive load of implementation


## Target problem
Solve the PDE
  * $u'' + u = 0$ in $[0,1]$
  * $u(0) = 0$
  * $u(1) = 1$

for which $\frac{sin(x)}{sin(1)} is the exact solution.
