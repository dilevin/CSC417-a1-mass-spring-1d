# Physics-Based Animation – Time Integration of Mass-Spring Systems in One Dimension
[![](https://github.com/dilevin/CSC2549-a1-mass-spring-1d/workflows/Build-CSC2549-Assignment-One/badge.svg)](https://github.com/dilevin/CSC2549-a1-mass-spring-1d/actions)

> **To get started:** Clone this repository and all its [submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) dependencies using:
> 
>     git clone --recursive https://github.com/dilevin/CSC2549-a1-mass-spring-1d.git

**Do not fork:** Clicking "Fork" will create a _public_ repository. If you'd like to use GitHub while you work on your assignment, then mirror this repo as a new _private_ repository: https://stackoverflow.com/questions/10065526/github-how-to-make-a-fork-of-public-repository-private

## Introduction

Welcome to Physics-Based Animation. This assignment has two purposes, (1) to familiarize you with the development tools used for assignments in the course and (2) to introduce you to some basic physics simulation concepts in one-dimension (1D).  

### Prerequisite installation

On all platforms, we will assume you have installed cmake and a modern c++
compiler on Mac OS X[¹](#¹macusers), Linux[²](#²linuxusers), or
Windows[³](#³windowsusers).

We also assume that you have cloned this repository using the `--recursive`
flag (if not then issue `git submodule update --init --recursive`). 

**Note:** We only officially support these assignments on Ubuntu Linux 18.04 (the OS the teaching labs are running) and OSX 10.13 (the OS I use on my personal laptop). While they *should* work on other operating systems, we make no guarantees. 

**All grading of assignments is done on Linux 18.04**

### Layout

All assignments will have a similar directory and file layout:

    README.md
    CMakeLists.txt
    main.cpp
    include/
      function1.h
      function2.h
      ...
    src/
      function1.cpp
      function2.cpp
      ...
    data/
      ...
    ...

The `README.md` file will describe the background, contents and tasks of the
assignment.

The `CMakeLists.txt` file setups up the cmake build routine for this
assignment.

The `main.cpp` file will include the headers in the `include/` directory and
link to the functions compiled in the `src/` directory. This file contains the
`main` function that is executed when the program is run from the command line.

The `include/` directory contains one file for each function that you will
implement as part of the assignment.

The `src/` directory contains _empty implementations_ of the functions
specified in the `include/` directory. This is where you will implement the
parts of the assignment.

The `data/` directory contains _sample_ input data for your program. Keep in
mind you should create your own test data to verify your program as you write
it. It is not necessarily sufficient that your program _only_ works on the given
sample data.

## Compilation

This and all following assignments will follow a typical cmake/make build
routine. Starting in this directory, issue:

    mkdir build
    cd build
    cmake ..

If you are using Mac or Linux, then issue:

    make

If you are using Windows, make sure to use `x64`. Running `cmake ..` should have created a Visual Studio solution file
called `a1-mass-spring-1d.sln` that you can open and build from there. Building the project will generate an .exe file. Move the .exe file to the `build` folder so that links to the input mesh aren't broken.

Why don't you try this right now?

## Execution

Once built, you can execute the assignment from inside the `build/` using 

    ./a1-mass-spring-1d

## Background

Physics-based animation leverages techniques from classical mechanics, numerical solutions of partial and ordinary equations (and more !!). As this course progresses you will learn how to use such methods to produce compelling animations of a wide variety of real-world phenomena. This assignment will lay common mathematical and technical foundation on which to build some seriously cool stuff. 

**Github does not render the math in this Markdown. Please look at README.html to see the equations in their proper form**

<!-- mention where things are implemented inline -->
### Newton's Second Law of Motion

Isaac Newton famously described the near-earth motion of objects using three laws:  

1. *every object will remain at rest or in uniform motion in a straight line unless compelled to change its state by the action of an external force*  
2. *the force acting on an object is equal to the time rate-of-change of the momentum*  
3. *for every action there is an equal and opposite reaction*  
   
The first law simply states an object cannot change its velocity unless a force acts on it. The third law is used to imply conservation of momentum. It is the second law that implies a mathematical model we can use to create moving pictures. That mathematical model can be expressed in 1D by the very famous formula

<p align="center"><img src="/tex/4b492a3e227366caaacb829793f79549.svg?invert_in_darkmode&sanitize=true" align=middle width=54.857300849999994pt height=14.611878599999999pt/></p>.

Here <img src="/tex/0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode&sanitize=true" align=middle width=14.433101099999991pt height=14.15524440000002pt/> is the scalar mass of the object (typically measured in kg), <img src="/tex/44bc9d542a92714cac84e01cbbb7fd61.svg?invert_in_darkmode&sanitize=true" align=middle width=8.68915409999999pt height=14.15524440000002pt/> is the acceleration of the object (typically measured in meters per second squared) and <img src="/tex/190083ef7a1625fbc75f243cffb9c96d.svg?invert_in_darkmode&sanitize=true" align=middle width=9.81741584999999pt height=22.831056599999986pt/> are the forces acting on the object (measured in Newtons, what an ego!).

Because acceleration is the rate-of-change of velocity over time, and velocity is the rate-of-change of position over time, we can rewrite this in all its differential equation glory as

<p align="center"><img src="/tex/1b7d193f3a1e0602ae4c4123f94c197b.svg?invert_in_darkmode&sanitize=true" align=middle width=55.563113099999995pt height=14.611878599999999pt/></p>,

where we use <img src="/tex/a9b948e0b565b6b895d2ed62dcb6df42.svg?invert_in_darkmode&sanitize=true" align=middle width=9.39498779999999pt height=21.95701200000001pt/> to mean <img src="/tex/2514e89e149c045bcc82b875829a7b7e.svg?invert_in_darkmode&sanitize=true" align=middle width=20.713063799999997pt height=33.45973289999998pt/>.

Before we get busy solving this differential equation numerically, we are going to introduce **one of the the most important concepts on this course** -- the variational perspective on classical mechanics. 

### The Variational Perspective  

We can thank [Leibniz](https://en.wikipedia.org/wiki/Gottfried_Wilhelm_Leibniz) for introducing the quantities of kinetic and potential energy. As the course progresses you will see how understanding dynamic motion in terms of these quantities makes it easy to extend Newton's Second Law to a wide variety of applications. Here we will get some intution in 1D by breifly reviewing how it arises from a variational principle.

It was Hamilton who identified the approriate variational principle for describing mechanics, called [**the Principle of Least Action**](https://en.wikipedia.org/wiki/Principle_of_least_action). The Principle of Least Action asserts that the trajectory of an object over time, is one that minimizes the integral, over time, of the difference between the kinetic and potential energy, or 

<p align="center"><img src="/tex/b75fe2ce1525f772749d0869aba8c972.svg?invert_in_darkmode&sanitize=true" align=middle width=288.88054965pt height=66.5117112pt/></p>,

In this course we use <img src="/tex/2f118ee06d05f3c2d98361d9c30e38ce.svg?invert_in_darkmode&sanitize=true" align=middle width=11.889314249999991pt height=22.465723500000017pt/> to represent the kinetic energy of an object and <img src="/tex/a9a3a4a202d80326bda413b5562d5cd1.svg?invert_in_darkmode&sanitize=true" align=middle width=13.242037049999992pt height=22.465723500000017pt/> to represent its potential energy. The mysterious quantity <img src="/tex/e73485aa867794d51ccd8725055d03a3.svg?invert_in_darkmode&sanitize=true" align=middle width=9.97711604999999pt height=14.611878600000017pt/> are the "generalized coordinates" of the system and, briefly all I'll say is that they describe the "configuration" of the mechanical system (don't panic, more soon!).

Note: The quantity <img src="/tex/cc9c184357ea9e61d2bd7897251a858a.svg?invert_in_darkmode&sanitize=true" align=middle width=45.222538349999994pt height=22.465723500000017pt/> is typically referred to as the Lagrangian, named after [Lagrange](https://en.wikipedia.org/wiki/Joseph-Louis_Lagrange).  

Finding the conditions under which the Principle of Least Action is stationary can be done using the Calculus of Variations (Top 3 in the most useful things to know if you are a graphics researcher!). It turns out that if <img src="/tex/818d3aa9c930b6852ba42f48ced5f0df.svg?invert_in_darkmode&sanitize=true" align=middle width=31.43830304999999pt height=24.65753399999998pt/> satisfies the differential equations (the Euler-Lagrange Equations)

<p align="center"><img src="/tex/9da65789c696f021e3bb50569810a35c.svg?invert_in_darkmode&sanitize=true" align=middle width=125.68145204999999pt height=39.452455349999994pt/></p>,

then <img src="/tex/ec1a89b0af2d725dd4f7862f3349d6bf.svg?invert_in_darkmode&sanitize=true" align=middle width=32.85376544999999pt height=24.65753399999998pt/> is a physically valid trajectory. In this way we can see that as long as we can define appropriate generalized coordinates along with kinetic and potential energies we can describe the motion of a physical system. This 

As physics simulation connoiseurs, this makes our job conceptually easy, we just need to solve this differential equation. Let's look at how we do that for our 1D mass spring system (and thereby get a good grade on this assignment). 

### The 1D Mass-Spring System

You will soon see that the variational machinery above gives us a cookbook method for describing physics-based motion. To do this we are going to need to define three quantities

1. Our nebulous <img src="/tex/e73485aa867794d51ccd8725055d03a3.svg?invert_in_darkmode&sanitize=true" align=middle width=9.97711604999999pt height=14.611878600000017pt/>'s
2. The kinetic energy of  mass spring, <img src="/tex/2f118ee06d05f3c2d98361d9c30e38ce.svg?invert_in_darkmode&sanitize=true" align=middle width=11.889314249999991pt height=22.465723500000017pt/>
3. The potential energy of mass spring, <img src="/tex/a9a3a4a202d80326bda413b5562d5cd1.svg?invert_in_darkmode&sanitize=true" align=middle width=13.242037049999992pt height=22.465723500000017pt/>

![A particle attached to a zero rest length spring](images/massspring.gif)

The first task is to choose <img src="/tex/e73485aa867794d51ccd8725055d03a3.svg?invert_in_darkmode&sanitize=true" align=middle width=9.97711604999999pt height=14.611878600000017pt/> so that all permissable positions (the configurations) of our mass-spring system can be written as <img src="/tex/74ac4e94ff3d34db6632e5982090c43c.svg?invert_in_darkmode&sanitize=true" align=middle width=107.88770849999999pt height=24.65753399999998pt/>. Here <img src="/tex/b75fcbf8855977c1ee1ffc3e8a82a336.svg?invert_in_darkmode&sanitize=true" align=middle width=32.85376544999999pt height=24.65753399999998pt/> is just the position of the center of our particle in 1D Euclidean space and so an easy choice for <img src="/tex/0fa61bd281e3d78d9a73be4c4ddddf00.svg?invert_in_darkmode&sanitize=true" align=middle width=41.87186189999999pt height=14.611878600000017pt/>.


Having picked some reasonable generalized coordinates we can now define <img src="/tex/2f118ee06d05f3c2d98361d9c30e38ce.svg?invert_in_darkmode&sanitize=true" align=middle width=11.889314249999991pt height=22.465723500000017pt/> and <img src="/tex/a9a3a4a202d80326bda413b5562d5cd1.svg?invert_in_darkmode&sanitize=true" align=middle width=13.242037049999992pt height=22.465723500000017pt/>. In 1D, the definition of kinetic energy is likely very familiar 

<p align="center"><img src="/tex/3558eeeb44a06017135adca7ce77f537.svg?invert_in_darkmode&sanitize=true" align=middle width=76.93410284999999pt height=32.990165999999995pt/></p>,

where <img src="/tex/0e51a2dede42189d77627c4d742822c3.svg?invert_in_darkmode&sanitize=true" align=middle width=14.433101099999991pt height=14.15524440000002pt/> is the mass of the particle. 

The potential energy in this system comes from the titular spring. Here we can use a simple quadratic energy defined as

<p align="center"><img src="/tex/acd191d2fcf5e206372019cb99a961c0.svg?invert_in_darkmode&sanitize=true" align=middle width=72.92908424999999pt height=32.990165999999995pt/></p>,

where <img src="/tex/63bb9849783d01d91403bc9a5fea12a2.svg?invert_in_darkmode&sanitize=true" align=middle width=9.075367949999992pt height=22.831056599999986pt/> is the "stiffness of the spring", the proportionality constant that converts a measure of stretch or compression to an energy.

From the Euler-Lagrange equations we can show that the motion of this single mass and spring satisfies

<p align="center"><img src="/tex/444a96131009683c07bf20aa67cdaf0a.svg?invert_in_darkmode&sanitize=true" align=middle width=82.7319834pt height=14.611878599999999pt/></p>

in other words, it satisfies Newton's Second Law in its standard form, with the forces given by [Hooke's Law](https://en.wikipedia.org/wiki/Hooke%27s_law). While this example might seem trivial, later in the course we'll see examples where applying the Principle of Least Action will be crucial for deriving equations of motion for more complicated objects.

### Numerical Time Integration 

A standard approach to solving the *second-order* differential equation above is to transform it into the coupled first-order system

<p align="center"><img src="/tex/03a95759422764d7a5c86bc8be15d490.svg?invert_in_darkmode&sanitize=true" align=middle width=112.14995219999999pt height=31.1415555pt/></p>

where we introduce the velocity of the mass-spring system as a seperate variable and the second equation enforces the approriate relationship between position and velocity. Now, given some initial position and velocity (together called the intial state) for the mass-spring system, our goal is to compute subsequent states which represent the trajectory of the particle over time. This process is called time integration because we are *integrating* the first-order differential equation, across time, to create the solution. Because almost all the systems we deal with will not admit analytical solutions, we are going to do this numerically. Let's take a look at four of the most common integration schemes. 

### Numerical Solution Attempt #1: Forward Euler Integration

Forward Euler is the simplest algorithm for numerical integration of an ordinary differential equation. It's so popular that it even got a shout out in the blockbuster movie [Hidden Figures](https://www.insidescience.org/news/exploring-math-hidden-figures). Sadly, we will quickly see that, for elastic objects, like springs, it has some pretty serious downfalls.

Forward Euler discretizes our differential equation by replacing all derivatives with first-order, finite difference approximations of the form 

<p align="center"><img src="/tex/5dcaba2794ebfc85932f57a803dd8058.svg?invert_in_darkmode&sanitize=true" align=middle width=110.47480949999999pt height=35.77743345pt/></p>

where we use superscripts to indicate whether we are accessing a variable at the current (<img src="/tex/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.936097749999991pt height=20.221802699999984pt/>) or next (<img src="/tex/628783099380408a32610228991619a8.svg?invert_in_darkmode&sanitize=true" align=middle width=34.24649744999999pt height=21.18721440000001pt/>) timestep. By rearranging we see that

<p align="center"><img src="/tex/01b6e466b98a1aa1e55bc7efad2ed586.svg?invert_in_darkmode&sanitize=true" align=middle width=120.77628914999998pt height=17.399144399999997pt/></p>

Following the same basic approach for our coupled, first-order system yields a scheme to compute an updated state for our mass spring system.

### Attempt #2: Runge-Kutta Integration

Explicit Runge-Kutta methods (which is what you will implement here) rely on multiple evalations of the right hand side of the ODE to approximate its time integral. In particular, <img src="/tex/58b6837ece4522fac45f635902ae08f5.svg?invert_in_darkmode&sanitize=true" align=middle width=20.88102719999999pt height=27.91243950000002pt/> order [Runge-Kutta](https://en.wikipedia.org/wiki/Runge–Kutta_methods) requires four such evaluations and combines them to compute an update of the object state which is much better behaved than that of Forward Euler.  

### Attempt #3: Implicit (Backward) Euler 

The key difference between Implicit, or Backward, Euler and Forward Euler time integration is that Implicit Euler no longer just relies on the configuration and velocity of the mass-spring at the current time step. Instead it tries to look into the future to make the integration more robust. Consider the following differential equation

<p align="center"><img src="/tex/665deada7cca78d2772000e398e9b7f6.svg?invert_in_darkmode&sanitize=true" align=middle width=87.9333939pt height=16.438356pt/></p>

Backward Euler discretizes the time derivitive using the same first-order finite difference as Forward Euler. However, while Forward Euler would choose to evaluate the function <img src="/tex/47b0192f8f0819d64bce3612c46d15ea.svg?invert_in_darkmode&sanitize=true" align=middle width=7.56844769999999pt height=22.831056599999986pt/> at time <img src="/tex/4f4f4e395762a3af4575de74c019ebb5.svg?invert_in_darkmode&sanitize=true" align=middle width=5.936097749999991pt height=20.221802699999984pt/>, Backward Euler chooses to evaluate it at time <img src="/tex/628783099380408a32610228991619a8.svg?invert_in_darkmode&sanitize=true" align=middle width=34.24649744999999pt height=21.18721440000001pt/> yielding the following update scheme

<p align="center"><img src="/tex/e8803e0e10d8ca77dc05138ed5d00f75.svg?invert_in_darkmode&sanitize=true" align=middle width=204.38330219999997pt height=19.9563243pt/></p>

If <img src="/tex/47b0192f8f0819d64bce3612c46d15ea.svg?invert_in_darkmode&sanitize=true" align=middle width=7.56844769999999pt height=22.831056599999986pt/> is a linear function in <img src="/tex/1da18d2de6d16a18e780cd6c435a2936.svg?invert_in_darkmode&sanitize=true" align=middle width=10.239687149999991pt height=14.611878600000017pt/>, this update can be efficiently evaluated (you'll do it in this assignment!)
.
### Attempt #4: Symplectic Euler

[Symplectic Euler](https://en.wikipedia.org/wiki/Semi-implicit_Euler_method), so named because it preserves the area carved out by an objects phase-space trajectory, is well suited to the types of coupled, first-order ordinary differential equations we will be solving. Given the system

<p align="center"><img src="/tex/3c3438a106714194faf5a08309225000.svg?invert_in_darkmode&sanitize=true" align=middle width=116.12233379999999pt height=104.29224135pt/></p>

Symplectic Euler applies the updates

<p align="center"><img src="/tex/88f618b8b0bbdaeb5fc7074f22fe0bde.svg?invert_in_darkmode&sanitize=true" align=middle width=180.34991369999997pt height=19.9563243pt/></p>

and then

<p align="center"><img src="/tex/5eb73f76a6a340eff88e68969be00750.svg?invert_in_darkmode&sanitize=true" align=middle width=147.05461649999998pt height=15.572667pt/></p>

## Tasks

In this assignment we'll primarily be interested in the energetic behaviour of our mass-spring system when integrated with the four time integrators described above. The user interface for this assignment includes a window which displays the trajectory of the mass-spring system in the [*phase plane*](https://en.wikipedia.org/wiki/Phase_plane) of the mass spring system over time. Images of the correct phase plane trajectories are included below.  

### Groundrules

Implementations of nearly any task you're asked to implemented in this course can be found online. Do not copy these and avoid googling for code; instead, search the internet for explanations. Many topics have relevant wikipedia articles. Use these as references. Always remember to cite any references in your comments.

### Implementation Notes

For this course most functions will be implemented in **.cpp** files. In this assignment the only exception is that time integrators are implemented in **.h** files. This is due to the use of lambda functions to pass force data to the time integration algorithms.

### src/dV_spring_particle_particle_dq.cpp

Compute the derivative of the potential energy with respect to the generalized coordinates. 

### src/d2V_spring_particle_particle_dq2.cpp

Compute the second derivative of the potential energy with respect to the generalized coordinates.

### include/forward_euler.h 

Advance the mass spring system one step forward in time using the Forward Euler algorithm.

![Phase space trajectory for Forward Euler integration](images/forward_euler.gif)

Forward Euler is the default integrator used by the assignment code. 

### include/runge_kutta.h

Advance the mass spring system one step forward in time using the <img src="/tex/b95ed19ec960536c272178c2cbff5f14.svg?invert_in_darkmode&sanitize=true" align=middle width=61.62698354999999pt height=27.91243950000002pt/> Runge-Kutta algorithm.

![Phase space trajectory for Runge-Kutta integration](images/rk4.gif)

To run the assignment code with the Runge-Kutta algorithm use

    ./a1-mass-spring-1d 'rk'

### include/backward_euler.h

Advance the mass spring system one step forward in time using the Backward Euler algorithm

![Phase space trajectory for Backward Euler integration](images/backward_euler.gif)

To run the assignment code with the Backward Euler algorithm use

    ./a1-mass-spring-1d 'be'

### include/symplectic_euler.h 

Advance the mass spring system one step forward in time using the Symplectic Euler algorithm

![Phase space trajectory for Symplectic Euler integration](images/symplectic_euler.gif)

To run the assignment code with the Symplectic Euler algorithm use

    ./a1-mass-spring-1d 'se'




