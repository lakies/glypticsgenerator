#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>

#include "OgreApp.h"

using namespace std;
using namespace Ogre;
using namespace OgreBites;

#ifndef QUICK_STARTUP
using eos::cpp17::optional;
using eos::cpp17::nullopt;
#endif

void OgreApp::setup(void)
{

	ApplicationContext::setup();

	// register for input events
	addInputListener(this);
	mRoot = getRoot();
	mRoot->restoreConfig();

	mRoot->addFrameListener(this);

	mScene = mRoot->createSceneManager();

	// Set the lights
	mScene->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	// Start the video stream
#ifndef QUICK_STARTUP
	frameCap.startVideoStream();
#endif


	mScene->getRootSceneNode()->translate(-5.0f, 40.0f, -130.0f);

	SceneNode* camNode = mScene->getRootSceneNode()->createChildSceneNode();
	camNode->setPosition(650, 0, 0);
	camNode->lookAt(mScene->getRootSceneNode()->getPosition(), Node::TS_PARENT);
	//camNode->lookAt(Vector3(0, 0, 0), Node::TS_PARENT);

	// Set-up camera and camera controls
	mainCamera = mScene->createCamera("MainCam");
	mainCamera->setAutoAspectRatio(true);
	mainCamera->setNearClipDistance(0.1);
	mainCamera->setFarClipDistance(1000);
	float neardist = mainCamera->getNearClipDistance();
	float fardist = mainCamera->getFarClipDistance();
	camNode->attachObject(mainCamera);

	getRenderWindow()->addViewport(mainCamera);
	Light* spotLight = mScene->createLight("SpotLight");
	spotLight->setDiffuseColour(1, 1, 1.0);
	spotLight->setSpecularColour(1, 1, 1.0);
	spotLight->setType(Light::LT_DIRECTIONAL);
	SceneNode* spotLightNode = mScene->getRootSceneNode()->createChildSceneNode();
	spotLightNode->attachObject(spotLight);
	spotLightNode->setDirection(-1, -1, 0);
	spotLightNode->setPosition(Vector3(200, 200, 0));
	spotLight->setSpotlightRange(Degree(35), Degree(50));


	// parse available materials from the config
	// config format:
	// MaterialSetName:HeadMaterial,BackgroundMaterial,EdgeMaterial
	parseMaterialsConfig();

	setupUI();

	// For HLMS shading
	//hlmsManager = new HlmsManager(mScene, "General");

	loadSS();


	camController = new CameraMan(camNode);
	camController->setStyle(CameraStyle::CS_ORBIT);
	camController->setYawPitchDist(Radian(Degree(90)), Radian(0), 650);
	addInputListener(camController);

	// Setup threading

#ifdef QUICK_STARTUP
	meshNeedsUpdating = true;
#else
	meshNeedsUpdating = false;
#endif


	assert(!mThread);
	mThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&OgreApp::runFrame, this)));
}

void OgreApp::loadSS() {
	CompositorManager::getSingleton().addCompositor(getRenderWindow()->getViewport(0), "sss");
	CompositorManager::getSingleton().setCompositorEnabled(getRenderWindow()->getViewport(0), "sss", false);


	
	//float* kernelArray = &kernel[0];


	//MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName("Material.001");
	//GpuProgramParametersSharedPtr params = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	////params->setNamedConstant("normalOverride", 1.0f);
}

void OgreApp::setupUI() {
	loadResources();
	trayMgr = new TrayManager("UI", getRenderWindow(), this);
	mScene->addRenderQueueListener(getOverlaySystem());
	trayMgr->hideCursor();

	addInputListener(trayMgr);

	/*fps = trayMgr->createLabel(TL_TOPLEFT, "FPS", "FPS", 100.0f);
	Label* pauseBtn = trayMgr->createLabel(TL_TOPLEFT, "Pause", "Pause", 100.0f);*/
	Label* materialBtn = trayMgr->createLabel(TL_BOTTOMRIGHT, "Material", curMat.second, 100.0f);
	Label* ssBtn = trayMgr->createLabel(TL_BOTTOMLEFT, "SS", "SS Off", 100.0f);
	Label* adjustBtn = trayMgr->createLabel(TL_TOPLEFT, "adjust", "Adjust SS", 150.0f);
	Label* resetBtn = trayMgr->createLabel(TL_TOPLEFT, "reset", "Reset SS", 150.0f);
	sswidth = trayMgr->createLongSlider(TL_TOPRIGHT, "sswidth", "SS Strength", 200, 100, 0, 100, 100);
	sswidth->hide();
}

void OgreApp::loadResources() {
	Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
}

void OgreApp::parseMaterialsConfig() {
	ifstream matsConfigFile;

	matsConfigFile.open("materials.cfg");
	string line;

	if (matsConfigFile.is_open()) {
		while (getline(matsConfigFile, line)) {
			if (line.rfind("#", 0) == 0) continue; // ignore comments
			std::vector<string> result;
			boost::split(result, line, boost::is_any_of(":"));

			std::vector<string> matsUsed;
			boost::split(matsUsed, result[1], boost::is_any_of(","));

			materials[result[0]] = matsUsed;

			cout << "Added GPG material set " << result[0] << ", consitsting of " << matsUsed[0] << ", " << matsUsed[1] << " and " << matsUsed[2] << ".\n";
		}
		matsConfigFile.close();
	}

	// set the initial material
	std::vector<string> keys;
	keys.reserve(materials.size());
	for (auto const& mat : materials) {
		keys.push_back(mat.first);
	}

	curMat.second = keys[curMat.first];
}

Eigen::Vector3f* OgreApp::calculateNormals(eos::core::Mesh mesh) {
	Eigen::Vector3f normals[20208]; // SFM
	int timesUsed[20208]; // amount of times the vertex was part of the polygon

	for (auto t : mesh.tvi) {
		// Normals calculation
		Eigen::Vector3f dir0 = mesh.vertices[t[2]] - mesh.vertices[t[0]];
		Eigen::Vector3f dir1 = mesh.vertices[t[0]] - mesh.vertices[t[1]];
		Eigen::Vector3f n = dir0.cross(dir1).normalized();

		// Check normals for all three used vertices, average if already exists
		for (int vi = 0; vi < 3; vi++) {
			if (t[vi] < 20208) { // checking for bounds
				int times = timesUsed[t[vi]];
				normals[t[vi]] = n;// (normals[t[vi]] * times + n) / (times + 1); // additive average
			}
		}

		// increase "used" counter for all three vertices
		for (int vi = 0; vi < 3; vi++) {
			if (t[vi] < 20208) { // checking for bounds
				timesUsed[t[vi]] += 1;
			}
		}
	}

	return normals;
}

void OgreApp::addMesh(eos::core::Mesh mesh) {
	SceneNode* profileNode = mScene->getRootSceneNode()->createChildSceneNode();

#ifndef QUICK_STARTUP
	int vertexCount = mesh.vertices.size();
	auto normals = calculateNormals(mesh);

	ManualObject* man = mScene->createManualObject("profile");
	man->setDynamic(true);
	man->estimateVertexCount(vertexCount);
	man->begin(materials[curMat.second][0]);

	for (int i = 0; i < vertexCount; i++) {
		auto v = mesh.vertices[i];
		man->position(v[0], v[1] + 5.0f, v[2] - 5.0f);
		auto n = normals[i]; // previously calculated normal for the vertex
		man->normal(n[0], n[1], n[2]);
	}

	for (auto t : mesh.tvi) {
		man->triangle(t[0], t[1], t[2]);
	}

	man->end();
	man->setMaterialName(0, materials[curMat.second][0]);
	profileNode->attachObject(man);
#endif

	profileNode->scale(0.07, 1, 1);
	profileNode->translate(1.7, 0, 0);

	// Adding the rest of the head
	Entity* headEntity = mScene->createEntity("head", "head.mesh");
	SceneNode* headNode = mScene->getRootSceneNode()->createChildSceneNode();
	headEntity->setMaterialName(materials[curMat.second][0]);

	headEntity->setCastShadows(false);
	//createHLMSMaterial(headEntity->getSubEntity(0), 1, amethystColor, 0.9f);
	headNode->attachObject(headEntity);
	headNode->scale(0.1, 1, 1);

	//// And the gem's base
	Entity* baseCenterEntity = mScene->createEntity("baseCenter", "baseCenter.mesh");
	SceneNode* baseCenterNode = mScene->getRootSceneNode()->createChildSceneNode();
	baseCenterEntity->setMaterialName(materials[curMat.second][1]);
	//createHLMSMaterial(baseCenterEntity->getSubEntity(0), 2, amethystColor, 0.9f);
	baseCenterNode->attachObject(baseCenterEntity);

	SubMesh* submesh = baseCenterEntity->getMesh()->getSubMesh(0);
	//baseCenterNode->scale(230, 230, 230);
	baseCenterNode->translate(-5.0f, 40.0f, -130.0f);

	Entity* baseEdgeEntity = mScene->createEntity("baseEdge", "baseEdge.mesh");
	SceneNode* baseEdgeNode = mScene->getRootSceneNode()->createChildSceneNode();
	baseEdgeEntity->setMaterialName(materials[curMat.second][2]);
	//(baseEdgeEntity->getSubEntity(0), 3, goldColor, 0.9f);//0.255f);
	baseEdgeNode->attachObject(baseEdgeEntity);
	//baseEdgeNode->scale(230, 230, 230);
	baseEdgeNode->translate(-5.0f, 40.0f, -130.0f);

	// Set the overall gem shape as a target for the camera

	getRenderWindow()->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
}

void OgreApp::updateMesh(eos::core::Mesh mesh) {
	int vertexCount = mesh.vertices.size();
	auto normals = calculateNormals(mesh);

	ManualObject* man = mScene->getManualObject("profile");
	man->beginUpdate(0);

	for (int i = 0; i < vertexCount; i++) {
		auto v = mesh.vertices[i];
		man->position(v[0], v[1] + 5.0f, v[2] - 5.0f);
		auto n = normals[i]; // previously calculated normal for the vertex
		man->normal(n[0], n[1], n[2]);
	}
	
	for (auto t : mesh.tvi) {
		man->triangle(t[0], t[1], t[2]);
	}

	man->end();
}

void OgreApp::addOrUpdateMesh(eos::core::Mesh mesh) {
	if (!mScene->hasManualObject("profile")) {
		addMesh(mesh);
	}
	else {
		updateMesh(mesh);
	}
	meshNeedsUpdating = false;
}

void OgreApp::createHLMSMaterial(SubEntity* subEntity, unsigned int id, ColourValue color, float roughness)
{
	PbsMaterial* pbsMaterial = new PbsMaterial();

	MaterialPtr materialPtr = subEntity->getMaterial();
	String newMaterialName = "Pbs_" + subEntity->getMaterialName() + "_" + StringConverter::toString(id);

	// Some test properties set
	pbsMaterial->setAlbedo(color);

	pbsMaterial->setRoughness(roughness);
	pbsMaterial->setLightRoughnessOffset(0.2);

	float f0 = 0.9f;
	pbsMaterial->setF0(color);

	MaterialPtr newMaterialPtr = subEntity->getMaterial()->clone(newMaterialName);
	newMaterialPtr->removeAllTechniques();
	Pass* pass = newMaterialPtr->createTechnique()->createPass();
	pass->setName("pbs");

	subEntity->setMaterial(newMaterialPtr);

	hlmsManager->bind(subEntity, pbsMaterial, "pbs");
}

bool OgreApp::keyPressed(const KeyboardEvent& evt)
{


	if (evt.keysym.sym == SDLK_ESCAPE)
	{
		mExiting = true;
		exit(EXIT_SUCCESS);
	}
	if (evt.keysym.sym == SDLK_F1) {
		//detector.toggleWindow();
	}

	return true;
}

void OgreApp::sliderMoved(Slider* slider) {

	if (!CompositorManager::getSingleton().getCompositorChain(getRenderWindow()->getViewport(0))->getCompositor("sss")->getEnabled())
	{
		return;
	}

	CompositorManager::getSingleton().setCompositorEnabled(getRenderWindow()->getViewport(0), "sss", false);
	
	int val = slider->getValue();
	float trueVal = 1500 + (1.0 * (100 - 1500) / (100.0)) * (val);
	std::cout << trueVal << std::endl;

	MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName("Horizontal_pass");
	GpuProgramParametersSharedPtr params = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	params->setNamedConstant("sssWidth", trueVal);
	mat = (MaterialPtr)MaterialManager::getSingleton().getByName("Vertical_pass");
	params = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	params->setNamedConstant("sssWidth", trueVal);

	CompositorManager::getSingleton().setCompositorEnabled(getRenderWindow()->getViewport(0), "sss", true);
}

void OgreApp::labelHit(Label* label) {
	if (label->getName().compare("Pause") == 0) {
		if (isPaused)
			label->setCaption("Pause");
		else
			label->setCaption("Unpause");
		isPaused = !isPaused;

		if (isPaused)
		{
			camController->setStyle(CS_MANUAL);
		}
		else
		{
			camController->setStyle(CS_ORBIT);
		}
	}
	if (label->getName().compare("Material") == 0) {
		changeNextMaterial();
		label->setCaption(curMat.second);
	}

	if (label->getName().compare("SS") == 0) {
		isSS = !isSS;
		CompositorManager::getSingleton().setCompositorEnabled(getRenderWindow()->getViewport(0), "sss", isSS);
		label->setCaption(isSS ? "SS On" : "SS Off");
	}

	if (label->getName().compare("reset") == 0) {
		sswidth->setValue(75);
		sliderMoved(sswidth);
	}

	if (label->getName().compare("adjust") == 0) {
		showAdjust = !showAdjust;

		if (showAdjust)
		{
			camController->setStyle(CS_MANUAL);
			sswidth->show();
		}
		else {
			camController->setStyle(CS_ORBIT);
			sswidth->hide();
		}

		trayMgr->adjustTrays();
		label->setCaption(!showAdjust ? "Adjust SS" : "Finish Adjust");
	}
}

void OgreApp::changeNextMaterial() {
	int matsCount = materials.size();

	if (curMat.first >= matsCount - 1) { // if last - cycle back
		curMat.first = 0;
	}
	else {
		curMat.first++;
	}

	std::vector<string> keys;
	keys.reserve(matsCount);
	for (auto const& mat : materials) {
		keys.push_back(mat.first);
	}

	curMat.second = keys[curMat.first];

	// Now update all the models
	if (mScene->hasManualObject("profile")) {
		auto profile = mScene->getManualObject("profile");
		profile->getSection(0)->setMaterialName(materials[curMat.second][0]);
	}
	if (mScene->hasEntity("head")) {
		auto mesh = mScene->getEntity("head");
		mesh->setMaterialName(materials[curMat.second][0]);
	}
	if (mScene->hasEntity("baseCenter")) {
		auto mesh = mScene->getEntity("baseCenter");
		mesh->setMaterialName(materials[curMat.second][1]);
	}
	if (mScene->hasEntity("baseEdge")) {
		auto mesh = mScene->getEntity("baseEdge");
		mesh->setMaterialName(materials[curMat.second][2]);
	}
}

void OgreApp::runFrame() {
	
	while (!mExiting) {
#ifndef QUICK_STARTUP
		if (!isPaused && !meshNeedsUpdating) {

			// Do calculations
			auto data = frameCap.capture();
			auto face = detector.detect(data);
			if (face.has_value()) {
				auto tempMesh = morpher.morph(face.value(), data.first);

			//	// Lock mutex
				boost::unique_lock<boost::mutex>* lock = new boost::unique_lock<boost::mutex>(mMutex);

				mesh = tempMesh;
				meshNeedsUpdating = true;

			//	// Unlock mutex
				delete lock;
			}
		}
#endif
	}
}

bool OgreApp::frameRenderingQueued(const FrameEvent& evt) {
	if (mExiting) {
		cout << "Exiting the application." << endl;
		return false;
	}

	// Lock mutex
	boost::unique_lock<boost::mutex>* lock = new boost::unique_lock<boost::mutex>(mMutex);

	if (meshNeedsUpdating) {
		addOrUpdateMesh(mesh);
	}

	// Unlock mutex
	delete lock;

	RenderTarget::FrameStats stats = getRenderWindow()->getStatistics();
	//std::cout << stats.avgFPS << " FPS" << std::endl;

	//fps->setCaption(std::to_string((int) stats.avgFPS));

	return true;
}