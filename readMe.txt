This is a basic implementation of Alexander Sannikov's "Novel Approach to Calculating Global
Illumination" knows as Radiance Cascades.
Link to his github repo (which also links the relevant paper):
https://github.com/Raikiri/RadianceCascadesPaper

To run the project, do the following:

1.	Be using the Windows 10 operating system (Windows 11 might work, but I have not tested it)
2.	Ensure you have CMAKE installed and added to the system path on your computer.
3.	Ensure you have Visual Studio 2022 installed with the C++ workload.


If the above are all as stated, perform the following steps:

1.	When you pulled this repo from git, ensure that you used git clone --recurse-submodules <repository-url>  (otherwise, the assimp submodule will not be included)
2.	In the Solution Directory, navigate to dep/assimp
3.	In the “assimp” directory, open a bash terminal and type “cmake CMakeLists.txt” an push enter.
4.	Then type “cmake --build .” and push enter again. This will set up the assimp submodule.
5.	Now double-click the visual studio solution file in the solution directory “zenditeEngineV2.sln” to open it in Visual Studio 2022.
6.	Press F5 to build and run.



The following 3rd Party Libs were used in this project:

- GLFW => For window context creation
	Link: https://www.glfw.org/

- GLEW => For retrieving OpenGL functions
	Link: https://glew.sourceforge.net/

- Assimp => for model loading
	Link: https://github.com/assimp/assimp

- Stb image => for texture image loading
	Link: https://github.com/nothings/stb

- glm => for maths operations (mainly for matrix and vector calculations)
	Link: https://github.com/g-truc/glm

- imgui => for the GUI implementation (implemented in menu.h)
	Link: https://github.com/ocornut/imgui



Other 3rd Party Contributions:

- LearnOpengl.com's camera class implementation. (Implemented as Camera.h)
	Links: https://learnopengl.com/Getting-started/Camera and https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

- The Churno's Basic OpenGl error checking macro from his OpenGl error checking YouTube video. (Implemented on lines 21 - 24 of utils.h)
	Link: https://www.youtube.com/watch?v=FBbPWSOQ0-w

- Austin Morlan's C++ entity component system blog. I used this design for the overall ECS design. With some modifications, such as having a higher level Coordinator on top of ECSCoordinator.h. (Implemented: All .h files within the src/ECS directory of this repo)
	Link: https://austinmorlan.com/posts/entity_component_system/

- Learnopengl textures => The following textures in the res/textures dir were taken from learnopengl.com: (awesomeface.png, container2.png, container2_specular.png, wall.png)

- res/textures/lava.jpg texture => taken from Maiomi on Pintrest: https://www.pinterest.com/pin/306737424617953515/

- res/textures/water.jpg texture => taken from pintrest PublicDomainPictures.net: https://www.pinterest.com/pin/306737424617953515/

- res/textures/rockySurface.png texture => taken from pintrest Medialoot: https://www.pinterest.com/pin/22-beautiful-and-seamless-rock-textures-medialoot--570127634082430201/

- Heightmap textures taken from: https://tangrams.github.io/heightmapper/