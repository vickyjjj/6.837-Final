# 6.837 Final Project: Ocean Animation

General code structure and gloo packages by MIT Fall 2020 6.837 Course Staff. Some code and packages redacted from this repo.

Methods based on Interactive Animation of Ocean Waves (Hinsinger et. al, 2002) ([link](https://hal.inria.fr/inria-00537490/document) ).

## Approach

### Screen Projection 
At initialization, the following projection steps are done to create the mesh. The same steps are called during reprojection. 
1. To project the screen grid, iterate gridlength times through the range [-1, 1] for x and y coordinates, and create a coordinate on the near plane of the camera frustum (z=-1) and on the far plane (z = 1) with w=1.0.
2. Use the inverse of the camera's projection matrix to mutiply the points and divide by the resulting w; these are the points projected into world space.
3. Create a ray with the near point as the origin and the direction as the vector from the near to far point.
4. Transform this by the inverse of the camera's view matrix to clip the ray to the camera's viewing frustum, then calculate the time t for the intersection of the ray with the y=0 ground plane.
5. Using this t and the ray, the (x, z) coordinates for the projected mesh vertex can be found.

One caveat is that if the mesh is projected with the exact screen size, then some wave motions will cause the "underside" of the mesh and empty black space to show near the edges, which is not desired. Therefore, instead of getting the exact view matrix, an additional function GetViewMatrixWithDistance(float distance) was implemented that merely returns the product of the glm::lookat() matrix of the given distance with the view matrix. By passing in a distance >1, a further viewing distance can be spoofed for the grid projection such that parts of the mesh will be rendered outside the actual viewing window; this is good so that the wave motion won't reveal the mesh edges at the window's border.
    
### Procedural Wave Animation
Since the wave procedure doesn't depend on previous state, an integrator and particle state or system aren't needed. New mesh positions can be calculated directly in the Update(delta time) method for SceneNodes.
1. Iterate through all mesh positions.
2. Iterate through a collection of waves. For each wave, a wavenumber vector, angular frequency and amplitude are needed. 
3. Evaluate the wave contribution using the equation, current global time, and initial vertex position, and add it to the new vertex position. 
One caveat is that without a time displacement, multiple waves in a similar direction and with a similar angular frequency may result in an over-accumulation of wave heights and augmented wave shapes. Therefore, each wave also has a time displacement associated with it, which helps space out the waves. In general, parameters need to be chosen carefully to avoid incorrect waves, such as trochoid paths that loop onto themselves. 

### Reprojection
Reprojection is only needed when the camera position or angle changes. Therefore, the WaveNode SceneNode is given access to the camera component and remembers its previous position and angle; if either of them are detected to have changed at the start of the Update(delta time) method, then the projection method is called to reproject vertex positions which are entirely replaced in the mesh before the procedural waves are calculated. 

## Results
The following video demonstrates various camera translations and rotations over the rendered ocean surface. ([link](https://youtu.be/SYxk9cNAIAk}{(https://youtu.be/SYxk9cNAIAk) )
