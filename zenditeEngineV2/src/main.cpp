
#include "utils.h"
#include <GLFW/glfw3.h>
#include <chrono>

#include "vendor/stb_image/stb_image.h"
#include "assimp/Importer.hpp"

#include "Shader.h"
//#include "Texture2D.h"
#include "geometrySetup.h"
#include "menu.h"
#include "Camera.h"

#include <filesystem>

#include "Coordinator.h"
#include "ECS/Components.h"

#include "helper/DayCycleCoordinator.h"


//Test comment

namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void generateFBMOctave(float* data, int width, int height, float scale, float amplitude) 
{
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			float fx = x * scale;
			float fy = y * scale;
			data[y * width + x] = stb_perlin_noise3(fx, fy, 0, 0, 0, 0) * amplitude;
		}
	}
}

//void addDataToLightRenderable(c_LightRenderable& rc, float* vertCubePosData, unsigned int* indices, size_t sizeofVertCubePosData, size_t sizeofIndices);

void setUpBasicModelMatrix(glm::mat4& MM, glm::vec3 pos, glm::vec3 scale)
{	
	MM = glm::mat4(1.0f);
	MM = glm::translate(MM, pos);
	MM = glm::scale(MM, scale);
}

// camera
std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
std::shared_ptr<Camera> Reflectioncamera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
std::shared_ptr<Camera> Refractioncamera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool trackMouseMovement = true;
double savedXpos, savedYpos;
double currentX, currentY;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//GUI Menu Toggles
bool toggle = true;
bool wireframe = false;
bool rotation = false;
bool seedMovement = false;
bool pauseSun = false;

unsigned int SEED = 0;
unsigned int frequency = 4;
int octaves = 4;
float lacunarity = 2.0f;
float persistence = 0.5f;
float amplitude = 20.0f;
float maxAmplitude = 0.0f;
bool reload = false;

//cloud dbm parameters:
float cloud_noiseFrequency = 0.01f;
float cloud_persistence = 0.5f;
float cloud_amplitude = 1.0f;

float discardThreshold = 0.06;

float offsetVal = 0.0f;

float dawnColor[3] = { 0.25f, 0.35f, 1.0f };
float middayColor[3] = { 0.25f, 0.35f, 1.0f };
float eveningColor[3] = {0.25f, 0.35f, 1.0f};
float sunsetColor[3] = {0.7f, 0.3f, 0.2f};
float nightColor[3] = {0.0f, 0.0f, 0.1f};

bool castShadows = true;
float shadowIntensity = 1.0f;
float waterShadowIntensity = 1.0f;

float haltedTime = 0.0f;
bool haltClouds = false;

int main(void)
{
	do 
	{
	
	GLFWwindow* window;
	if (!glfwInit()) {
		return -1;
	}
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Zen", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	GLFWcursorposfun previousCursorPosCallback = nullptr;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	previousCursorPosCallback = glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glfwSetCursorPosCallback(window, nullptr);
	//glfwSetCursorPosCallback(window, mouse_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "I'm am not GLEW_OK, I'm GLEW_SAD :(\n";
	}

	imGuiSetup(window);

	stbi_set_flip_vertically_on_load(true); //#### THIS NEEDS TO BE ACTIVE ### or else image texture will be upside down.
	//std::cout << "Current working directory: " << fs::current_path() << std::endl;
	glEnable(GL_DEPTH_TEST);

	std::cout << glGetString(GL_VERSION) << "\n";


	float Quad[] = {
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f
	};

	float QuadTexCoord[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int QuadIndices[] = {
		0, 1, 2,
		2, 3, 0
	};


	//Check the number of texture units we can have on the GPU
	GLint maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	std::cout << "Maximum texture units available: " << maxTextureUnits << std::endl;

	//Testing ECS: Start ECS - $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	
	Coordinator COORD("opengl", "opengl", camera); //std::string API_Type, std::string Render_Type, std::shared_ptr<Camera> camera
	COORD.RegisterComponents();
	COORD.RegisterSystems();
	COORD.SetUpSystemBitsets();
	

	std::cout << "\nRenderableSystem bitset: " << COORD.GetSystemBitset<RenderableSystem>() << std::endl;
	//std::cout << "\Rigid_CollisionDetectionSystem bitset: " << COORD.GetSystemBitset<Rigid_CollisionDetectionSystem>() << std::endl;

	std::shared_ptr<Shader> sh_Skydome = std::make_shared<Shader>("res/shaders/PerlinNoise/vs_Skydome.glsl",
		"res/shaders/PerlinNoise/fs_Skydome.glsl"); 

	std::shared_ptr<Shader> sh_Clouds = std::make_shared<Shader>("res/shaders/PerlinNoise/vs_Clouds.glsl",
		"res/shaders/PerlinNoise/fs_Clouds.glsl");

	std::shared_ptr<Shader> sh_basicWithTex = std::make_shared<Shader>("res/shaders/BasicShaders/vs_cubeWnormANDtex.glsl",
		"res/shaders/BasicShaders/fs_cubeWnormANDtex.glsl"); //#Shaders have not yet been abstracted into the API_Manger

	std::shared_ptr<Shader> sh_ShellTexturing = std::make_shared<Shader>("res/shaders/shellTexturing/vs_shellTexturingV1.glsl",
		"res/shaders/shellTexturing/fs_shellTexturingV1.glsl"); //#Shaders have not yet been abstracted into the API_Manger

	std::shared_ptr<Shader> sh_DirLight = std::make_shared<Shader>("res/shaders/worldLighting/vs_dirLight.glsl",
		"res/shaders/worldLighting/fs_dirLight.glsl");

	std::shared_ptr<Shader> sh_LightEmitter = std::make_shared<Shader>("res/shaders/worldLighting/vs_lightEmitter.glsl",
		"res/shaders/worldLighting/fs_lightEmitter.glsl");

	std::shared_ptr<Shader> sh_emReflection = std::make_shared<Shader>("res/shaders/cubeMaps/vs_emReflection.glsl",
		"res/shaders/cubeMaps/fs_emReflection.glsl");


	std::shared_ptr<Shader> sh_waterReflection = std::make_shared<Shader>("res/shaders/water/vs_Reflection.glsl",
		"res/shaders/water/fs_Reflection.glsl");

	std::shared_ptr<Shader> sh_waterRefraction = std::make_shared<Shader>("res/shaders/water/vs_Refraction.glsl",
		"res/shaders/water/fs_Refraction.glsl");

	std::shared_ptr<Shader> sh_Water = std::make_shared<Shader>("res/shaders/water/vs_water.glsl",
		"res/shaders/water/fs_water.glsl");

	std::shared_ptr<I_SceneFactory> sceneFactory = std::make_unique<MinimalSceneFactory>(COORD);

	std::vector<std::shared_ptr<Shader>> shaders;
	shaders.push_back(sh_basicWithTex);		// 0
	sh_basicWithTex->setShaderArrayIndex(shaders.size() - 1);
	shaders.push_back(sh_Skydome);			// 1
	sh_Skydome->setShaderArrayIndex(shaders.size() - 1);
	shaders.push_back(sh_Clouds);			// 2
	sh_Clouds->setShaderArrayIndex(shaders.size() - 1);
	shaders.push_back(sh_ShellTexturing);	// 3
	sh_ShellTexturing->setShaderArrayIndex(shaders.size() - 1);

	shaders.push_back(sh_DirLight);			// 4
	sh_DirLight->setShaderArrayIndex(shaders.size() - 1);

	shaders.push_back(sh_LightEmitter);		// 5
	sh_LightEmitter->setShaderArrayIndex(shaders.size() - 1);

	shaders.push_back(sh_emReflection);		// 6
	sh_emReflection->setShaderArrayIndex(shaders.size() - 1);

	shaders.push_back(sh_waterReflection);		// 7
	sh_waterReflection->setShaderArrayIndex(shaders.size() - 1);

	shaders.push_back(sh_waterRefraction);		// 8
	sh_waterRefraction->setShaderArrayIndex(shaders.size() - 1);
	
	shaders.push_back(sh_Water);		// 9
	sh_Water->setShaderArrayIndex(shaders.size() - 1);

	std::vector<Entity> entities;
	std::vector<Entity> allEntites;
	std::vector<unsigned short int> allTexUnits;
	std::unordered_map<std::string, std::shared_ptr<EntityScene>> map_SceneNameToEntitiyScene;
	std::unordered_map<std::string, std::vector<Entity>> map_SceneEntites;

	unsigned int hfWidth = 100;
	unsigned int hfHeight = 100;
	unsigned int heightFieldTex;

	Skydome skydome;

	util::setupSceneECS(
		COORD,
		shaders,
		entities,
		allEntites,
		allTexUnits,
		map_SceneNameToEntitiyScene,
		map_SceneEntites,
		sceneFactory,
		hfWidth,
		hfHeight,
		heightFieldTex,
		frequency,
		SEED,
		octaves,
		lacunarity,
		persistence,
		amplitude,
		maxAmplitude,
		skydome
	);
		
	//std::cout << "\nc_AABB bitset position: " << static_cast<unsigned int>(COORD.GetComponentBitsetPos<c_AABB>());
	//std::cout << "\nentities[2] bitset: " << COORD.GetEntitySignature(entities[2]) << std::endl;

	std::cout << "\nImGui Version: " << IMGUI_VERSION << std::endl;
	std::cout << "\n Number of active entities: " << COORD.GetActiveEntities() << std::endl;

	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);

	std::cout << "GLFW Version: " << major << "." << minor << "." << revision << std::endl;

	//Fill AllEntities Vector:
	for(const auto& pair :  map_SceneEntites)
	{
		for(int i = 0; i < pair.second.size(); ++i)
		{
			allEntites.push_back(pair.second[i]);
		}
	}

	unsigned seedCounter = 0;

	//Water FBOs --- START
	//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	unsigned int FBO_reflection;
	unsigned int FBO_refraction;

	glGenFramebuffers(1, &FBO_reflection);
	glGenFramebuffers(1, &FBO_refraction);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_reflection);

	unsigned int reflectionTexture;

	unsigned int short reflectionTexUnit = 8;
	glActiveTexture(GL_TEXTURE0 + reflectionTexUnit);

	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
	
	unsigned int reflection_rbo;
	glGenRenderbuffers(1, &reflection_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, reflection_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single render buffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflection_rbo); // now actually attach it
	
	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: reflection Framebuffer is not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_refraction);

	unsigned int refractionTexture;

	unsigned int short refractionTexUnit = 9;
	glActiveTexture(GL_TEXTURE0 + refractionTexUnit);

	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);

	unsigned int refractionDepthTex;
	unsigned int short refractionDepthTU = 13;
	glActiveTexture(GL_TEXTURE0 + refractionDepthTU);
	
	glGenTextures(1, &refractionDepthTex);
	glBindTexture(GL_TEXTURE_2D, refractionDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTex, 0);
	

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: refraction Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Water FBOs --- END


	//FBO - START:

	glViewport(0, 0, 512, 512);

	unsigned int quadVAO;
	unsigned int quadPosVBO, quadTexVBO;
	unsigned int quadEBO;

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &quadTexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadTexCoord), QuadTexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &quadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);


	unsigned int cloudFBO;
	glGenFramebuffers(1, &cloudFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, cloudFBO);

	unsigned int cloudTexture;
	unsigned int short cloudTexUnit = 6;

	glActiveTexture(GL_TEXTURE0 + cloudTexUnit);

	glGenTextures(1, &cloudTexture);
	glBindTexture(GL_TEXTURE_2D, cloudTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Attach the cloudTexture to the cloudFBO:
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cloudTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		std::cerr << "Cloud Framebuffer is not complete! :(" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Cloud Framebuffer is complete" << std::endl;
	}

	// FrameBuffer Used to be drawn here //#HERE_WAS_FRAMEBUFFER

	glActiveTexture(GL_TEXTURE0);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


	//Generate Light Dept FBO
	unsigned int FBO_lightViewDepth;

	glGenFramebuffers(1, &FBO_lightViewDepth);
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_lightViewDepth);

	unsigned int lightDepthTexture;

	unsigned int short lightDepthTextureUnit = 16;
	glActiveTexture(GL_TEXTURE0 + lightDepthTextureUnit);

	glGenTextures(1, &lightDepthTexture);
	glBindTexture(GL_TEXTURE_2D, lightDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lightDepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: refraction Framebuffer is not complete!" << std::endl;
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//Set Up Cloud Noise Octave Textures
	float noiseData_1[512 * 512];
	float noiseData_2[512 * 512];
	float noiseData_3[512 * 512];
	float noiseData_4[512 * 512];

	// Parameters for fbm

	generateFBMOctave(noiseData_1, 512, 512, cloud_noiseFrequency, cloud_amplitude);

	cloud_noiseFrequency = cloud_noiseFrequency * 2;
	cloud_amplitude = cloud_amplitude * cloud_persistence;
	generateFBMOctave(noiseData_2, 512, 512, cloud_noiseFrequency, cloud_amplitude);

	cloud_noiseFrequency = cloud_noiseFrequency * 2;
	cloud_amplitude = cloud_amplitude * cloud_persistence;
	generateFBMOctave(noiseData_3, 512, 512, cloud_noiseFrequency, cloud_amplitude);

	cloud_noiseFrequency = cloud_noiseFrequency * 2;
	cloud_amplitude = cloud_amplitude * cloud_persistence;
	generateFBMOctave(noiseData_4, 512, 512, cloud_noiseFrequency, cloud_amplitude);

	float* combinedNoiseData = new float[512 * 512 * 4];

	for (int i = 0; i < 512 * 512; ++i) {
		combinedNoiseData[4 * i + 0] = noiseData_1[i]; // Red channel
		combinedNoiseData[4 * i + 1] = noiseData_2[i]; // Green channel
		combinedNoiseData[4 * i + 2] = noiseData_3[i]; // Blue channel
		combinedNoiseData[4 * i + 3] = noiseData_4[i]; // Alpha channel
	}

	GLuint cloudNoiseTexture;
	glGenTextures(1, &cloudNoiseTexture);
	glBindTexture(GL_TEXTURE_2D, cloudNoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, combinedNoiseData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set the speed for each octave
	glm::vec2 speeds[4] = {
		glm::vec2(0.0f, 0.025f),
		glm::vec2(0.013f, 0.00f),
		glm::vec2(0.00f, 0.006f),
		glm::vec2(0.003f, 0.00f)
	};

	//Setup DayCycle Coordinator:
	float sunRadius = 26.0f;
	float singleCycleDuration = 20.0f;
	DayCycleCoordinator DCC(COORD, entities[1], sunRadius, singleCycleDuration);

	c_Renderable& waterFloor = COORD.GetComponentDataFromEntity<c_Renderable>(0);

	glEnable(GL_CLIP_DISTANCE0);

	int clippingPlane = 0;

	// Enable alpha blending

	dawnColor[0] = DCC.m_dawn.r;
	dawnColor[1] = DCC.m_dawn.g;
	dawnColor[2] = DCC.m_dawn.b;

	middayColor[0] = DCC.m_midday.r;
	middayColor[1] = DCC.m_midday.g;
	middayColor[2] = DCC.m_midday.b;

	eveningColor[0] = DCC.m_evening.r;
	eveningColor[1] = DCC.m_evening.g;
	eveningColor[2] = DCC.m_evening.b;

	sunsetColor[0] = DCC.m_sunset.r;
	sunsetColor[1] = DCC.m_sunset.g;
	sunsetColor[2] = DCC.m_sunset.b;

	nightColor[0] = DCC.m_night.r;
	nightColor[1] = DCC.m_night.g;
	nightColor[2] = DCC.m_night.b;
	
	int lightDepthPass = 0;
	bool checkHalt = true;

	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// START => Cloud Noise Octaves 
		sh_Clouds->bindProgram();

		if(haltClouds)
		{
			if (checkHalt)
			{
				haltedTime = currentFrame;
				checkHalt = false;
			}
			sh_Clouds->setUniformFloat("time", haltedTime);
		}
		else
		{
			sh_Clouds->setUniformFloat("time", currentFrame);
			checkHalt = true;
		}

		for (int i = 0; i < 4; ++i) {
			std::string uniformName = "speed[" + std::to_string(i) + "]";
			GLint location = glGetUniformLocation(sh_Clouds->getShaderHandle(), uniformName.c_str());
			glUniform2f(location, speeds[i].x, speeds[i].y);
		}

		// Bind the noise texture
		glActiveTexture(GL_TEXTURE14);
		glBindTexture(GL_TEXTURE_2D, cloudNoiseTexture);
		sh_Clouds->setUniformTextureUnit("noiseTexture", 14);
		sh_Clouds->setUniformFloat("discardThreshold", discardThreshold);

		glm::vec3 skyColorValue = DCC.getSkyColor();
		sh_Clouds->setUniform3fv("skyColor", skyColorValue);


		glActiveTexture(GL_TEXTURE0);

		// END => Cloud Noise Octaves 

		glViewport(0, 0, 512, 512);
		//sh_Clouds->bindProgram();
		//sh_Clouds->setUniformFloat("deltaTime", currentFrame);

		glBindFramebuffer(GL_FRAMEBUFFER, cloudFBO);
		glBindVertexArray(quadVAO);

		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		float waterHeight = COORD.GetComponentDataFromEntity<c_Transform>(0).modelMat[0][3].y;
		float cameraHeight = camera->getPosition().y;
		float camera_water_diff = fabs((cameraHeight - waterHeight));

		//std::cout << camera_water_diff << std::endl;

		for(int ii = 0; ii < 2; ++ii)
		{
			if(ii == 0)
			{
				clippingPlane = 2;
				glDisable(GL_CLIP_DISTANCE0);
				//glEnable(GL_CLIP_DISTANCE0);
				glBindFramebuffer(GL_FRAMEBUFFER, FBO_lightViewDepth); //rebind default framebuffer
				
				waterFloor.isActive = false;

				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

				//glBindFramebuffer(GL_FRAMEBUFFER, fbo);

				glClearColor(skyColorValue.x, skyColorValue.y, skyColorValue.z, 1.0f);
				glClear(GL_DEPTH_BUFFER_BIT); // <== #HERE
				glEnable(GL_DEPTH_TEST);

				//glDepthFunc(GL_ALWAYS);
				glm::vec3 currentSkyColor = DCC.getSkyColor();
				COORD.runAllSystems(deltaTime, currentFrame, allEntites, clippingPlane, offsetVal, currentSkyColor.r, currentSkyColor.g, currentSkyColor.b, 0, castShadows, shadowIntensity, waterShadowIntensity);
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					if (i == 0)
					{
						clippingPlane = 0;
						glEnable(GL_CLIP_DISTANCE0);
						glBindFramebuffer(GL_FRAMEBUFFER, FBO_reflection);
						glm::vec3 offsetVec = glm::vec3(0.0f, -5.0f, 0.0f);
						//COORD.offsetCamera(offsetVec, 0.0f, 0.0f, 0.0f);
						camera->ShiftDown(camera_water_diff * 2.0f);
						//camera->RotateUp(40.0f);
						camera->InvertPitch();

						waterFloor.isActive = false;
					}
					else if (i == 1)
					{
						clippingPlane = 1;
						glEnable(GL_CLIP_DISTANCE0);
						glBindFramebuffer(GL_FRAMEBUFFER, FBO_refraction);
						//camera->ShiftDown(-10.0f);
						//camera->RotateUp(-40.0f);
						camera->ShiftDown(-camera_water_diff * 2.0f);
						camera->InvertPitch();
					}
					else
					{
						clippingPlane = 2;
						glDisable(GL_CLIP_DISTANCE0);
						//glEnable(GL_CLIP_DISTANCE0);
						glBindFramebuffer(GL_FRAMEBUFFER, 0); //rebind default framebuffer
						glm::vec3 offsetVec = glm::vec3(0.0f, 0.0f, 0.0f);

						//camera->RotateUp(-40.0f);

						waterFloor.isActive = true;
						//COORD.offsetCamera(offsetVec, 0.0f, 0.0f, 0.0f);
					}


					//FBO - END

					glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

					//glBindFramebuffer(GL_FRAMEBUFFER, fbo);

					glClearColor(skyColorValue.x, skyColorValue.y, skyColorValue.z, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <== #HERE
					glEnable(GL_DEPTH_TEST);

					//glDepthFunc(GL_ALWAYS);
					glm::vec3 currentSkyColor = DCC.getSkyColor();
					COORD.runAllSystems(deltaTime, currentFrame, allEntites, clippingPlane, offsetVal, currentSkyColor.r, currentSkyColor.g, currentSkyColor.b, 1, castShadows, shadowIntensity, waterShadowIntensity); //#ECS_RENDERING
				}
			}
			
		}
		
		
		if(wireframe == true)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		DCC.Update(deltaTime, currentFrame);
		if(pauseSun == true)
		{
			DCC.Pause();
		}
		else
		{
			DCC.Resume();
		}

		genMenu_1(
			deltaTime,
			allEntites,
			entities,
			map_SceneEntites,
			map_SceneNameToEntitiyScene,
			COORD,
			allTexUnits[0],
			allTexUnits[1],
			allTexUnits[2],
			allTexUnits[3],
			allTexUnits[4],
			allTexUnits[5],
			allTexUnits[6],
			allTexUnits[7],
			10,
			SEED,
			frequency,
			reload,
			octaves,
			lacunarity,
			persistence,
			amplitude,
			maxAmplitude,
			cloud_noiseFrequency,
			cloud_persistence,
			cloud_amplitude,
			discardThreshold,
			pauseSun,
			&DCC,
			dawnColor,
			middayColor,
			eveningColor,
			sunsetColor,
			nightColor,
			castShadows,
			shadowIntensity,
			waterShadowIntensity,
			haltClouds
			);

		if (seedMovement == true)
		{
			SEED = SEED + 1;
		}

		util::resetHF
		(
			allTexUnits[0],
			COORD,
			COORD.GetComponentDataFromEntity<c_Renderable>(allEntites[6]),
			allEntites[6],
			heightFieldTex,
			hfWidth,
			hfHeight,
			frequency,
			SEED,
			octaves,
			lacunarity,
			persistence,
			amplitude,
			maxAmplitude
		);

		glfwPollEvents();

		processInput(window);

		glfwSwapBuffers(window);
		/*
		seedCounter++;
		if (seedCounter == 2)
		{
			seedCounter = 0;
		}
		*/

	}

	glDeleteFramebuffers(1, &cloudFBO);
	glDeleteFramebuffers(1, &FBO_reflection);
	glDeleteFramebuffers(1, &FBO_refraction);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	//std::cin.get();
	} while (reload);

	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera->ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera->ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		seedMovement = true;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		seedMovement = false;

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		pauseSun = false;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		pauseSun = true;

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{

		//glfwGetCursorPos(window, &savedXpos, &savedYpos);
		savedXpos = currentX;
		savedYpos = currentY;
		trackMouseMovement = false;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		//glfwSetCursorPos(window, savedXpos, savedYpos);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		lastX = savedXpos;
		lastY = savedYpos;

		currentX = lastX;
		currentY = lastY;

		glfwSetCursorPos(window, currentX, currentY);

		trackMouseMovement = true;

	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		wireframe = true;
		
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		wireframe = false;

	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

	if (trackMouseMovement)
	{

		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		currentX = xpos;
		currentY = ypos;

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera->ProcessMouseMovement(xoffset, yoffset);
	}

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(static_cast<float>(yoffset));
}