// StandaloneOpenGLRenderer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "../WindowsUtils/Timer.h"

#include "../OpenGLRenderer/Renderer.h"
#include "../OpenGLRenderer/renderer.h"
#include "../OpenGLRenderer/text.h"
#include "../OpenGLRenderer/camera.h"
#include "../OpenGLRenderer/graphic-object.h"
#include "../OpenGLRenderer/graphic-object-2d.h"
#include "../OpenGLRenderer/input-handler.h"
#include "../GeometryLib/quaternion.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/transform3d.h"
#include "../GeometryLib/vector2d.h"
#include "World.h"
#include "RobotMind.h"
#define TIME_STEP 0.6

int main(int argc, char** argv)
{
	CRenderer* pRenderer = 0;
	IInputHandler* pInputHandler = 0;
	World* pWorld;
	RobotMind *pRobotMind;

	pRenderer = new CRenderer();
	pRenderer->init(argc, argv, 800, 600);
	pRenderer->setDataFolder("../scenes/");
	pRenderer->loadScene("robot-mind.scene");
	pInputHandler = new CFreeCameraInputHandler();

	pWorld= new World("map1.csv");
	pRobotMind = new RobotMind(pWorld);

	CTimer drawingTimer;
	CTimer decisionTimer;
	double dt = 0.0;

	drawingTimer.start();
	decisionTimer.start();

	CGraphicObject* pRobot = pRenderer->getObjectByName("Robot");

	double t = 0.0;

	C2DText* pFPSText = new C2DText(string("fps"), Vector2D(0.05, 0.95), 0);
	pRenderer->add2DGraphicObject(pFPSText);
	C2DText* pDirection = new C2DText("", Vector2D(0.05, 0.9), 0);
	pRenderer->add2DGraphicObject(pDirection);

	Vector3D oldPosition= Vector3D(pWorld->getRobotPos().x(), 0
		, pWorld->getRobotPos().y());
	Vector3D targetPosition= Vector3D(pWorld->getRobotPos().x(), 0
		, pWorld->getRobotPos().y());

	while (!pWorld->robotOverTarget())
	{
		//UPDATE////////////////////
		////////////////////////////
		if (decisionTimer.getElapsedTime() > TIME_STEP)
		{
			oldPosition = Vector3D(pWorld->getRobotPos().x(), 0
				, pWorld->getRobotPos().y());

			Position intMovement= pRobotMind->TakeStep();

			pDirection->set(pRobotMind->GetLastDirection());

			targetPosition= oldPosition + Vector3D(intMovement.x(), 0.0, intMovement.y());
			
			decisionTimer.start();
		}
		else
		{
			double u = decisionTimer.getElapsedTime() / TIME_STEP;

			pRobot->setWorldPosition(oldPosition + (targetPosition-oldPosition)*u);
		}

		pFPSText->set(pRobotMind->GetState());

		t += dt;

		//queued events?
		
		pInputHandler->handleInput();

		//RENDER////////////////////
		////////////////////////////
		pRenderer->redraw();


		dt = drawingTimer.getElapsedTime(true);
	}

	delete pWorld;
	delete pRobotMind;
	delete pRenderer;
	delete pInputHandler;
	return 0;
}

