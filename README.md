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

for which $\frac{sin(x)}{sin(1)}$ is the exact solution.


# Technical details
The stencils are obtained by solving a particular Vandermonde system, this is somewhat unconventional as finite difference stencils are obtained with [Fornberg algorithm](https://www.colorado.edu/amath/sites/default/files/attached-files/mathcomp_88_fd_formulas.pdf). Nevertheless the calculations are to be made in exact arithmetic as we want the stencils to be accurate as possible and the Vandermonde matrix is ill-conditioned. Since the coefficient are integer, rational arithmetic is exact.


## A good rational type
The stencil calculations are made at runtime, so we will need a library that implements rational arithmetic. This may seem a simple task and one is tempted to roll out a custom rational type library as it's very common on github, but we need a critical feature.

Rational arithmetic is built on integer arithmetic, which could overflow. It's possible that some calculation could generate overflows even if the result is representable. Some techniques are employed to remove this risk like in the [boost rational implementation](https://www.boost.org/doc/libs/latest/libs/rational/rational.html), but in general it is recommended to:
  * provide an integer type with unlimited precision, or
  * provide an integer type that can signal overflow

those are not easy features to support and will require pretty heavy dependencies. Since we need a rigorous comparison between solver orders, I'll accept the boost dependency.
