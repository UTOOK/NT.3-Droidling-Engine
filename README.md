This is the NT.3 Engine for use by UTOOK to develop the Keep Droids Simple (KDS) platform.

Included documents are:
-User manual describing the syntax of the API built into the NT.3 Engine.
-Half finished rough draft of an explanation of how the networks encode, evaluate, and retrieve information.
-The pseudocode for the algorithm used to create the 1D networks that NT.3 currently uses. 

As it is currently it is not very user friendly and needs major refactoring, and it only supports windows. Ideally it will be ported to linux as well.

The NT.3 Engine is currently capable of working with two different 1D constructs.
-One is stored in RAM and suited for small projects.
-The other uses memory mapping for larger projects where RAM is insufficient to store the networks. 

