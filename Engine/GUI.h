#pragma once
// Include GUI classes
class Game;
class Settings;
#include "Settings.h"
class SoundEffect;
#include "SoundEffect.h"
#include "MainWindow.h"
class Time;
#include "Time.h"
#include "Animation.h"
#include "SpriteEffect.h"
class Font;
#include "Font.h"
class Label;
#include "Label.h"
class Button;
#include "Button.h"
class TextList;
#include "TextList.h"
class PlayerSpeedBoost;
#include "PlayerSpeedBoost.h"
// Include game objects
class Buff;
class Player;
class Ball;
class Brick;
#include "Player.h"
#include "Ball.h"
#include "Brick.h"
#include "Buff.h"
// Include complex classes
class Server;
#include "Server.h"
class Client;
#include "Client.h"
#include "Util.h"
// Stages
class GameStageList;
#include "GameStageList.h"
class GameStage;
#include "GameStage.h"
namespace Stage {
	class MainMenu;
#include "MainMenu.h"
	class GameProcess; class NewHighScore;
#include "GameProcess.h"
	class TestRoom;
#include "TestRoom.h"
}