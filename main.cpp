#include <iostream>
#include <fstream>
#include "SFML/Graphics.hpp"
#include "Helpful.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "SFML/Audio.hpp"

sf::RenderWindow Window;
sf::Event Event;
sf::Vector2f translated_pos;
sf::Font MyFont;

namespace Audio {
	sf::Sound EffectsSounds, BackMusicSounds;
	sf::SoundBuffer pickUpSound, wallBreak, backMusic;
	void Init() {
		EffectsSounds.setVolume(15);
		BackMusicSounds.setVolume(4);
		wallBreak.loadFromFile("res/wall_break.wav");
		pickUpSound.loadFromFile("res/pick_up_sound.wav");
		backMusic.loadFromFile("res/backMusic.wav");
	}
	void PlaySound(sf::Sound *sursa, const sf::SoundBuffer& soundeffect,const bool& loop) {
		sursa->setBuffer(soundeffect);
		sursa->play();
		sursa->setLoop(loop);
	}
}

namespace MainMenu {
	bool playClicked = false;
	sf::RectangleShape menuback, playbutton;
	sf::Texture menutexture,playbuttontexture;
	void Init() {
		Audio::Init();
		//
		Window.create(sf::VideoMode(800, 500), "Robot Puzzle Game", sf::Style::Close);
		Window.setFramerateLimit(60);
		//
		menutexture.loadFromFile("res/main_menu_back.png");
		InitRect(&menuback, sf::Vector2f(0, 0), sf::Vector2f(800, 500), sf::Color::White);
		menuback.setTexture(&menutexture);
		//
		playbuttontexture.loadFromFile("res/play_button.png");
		InitRect(&playbutton, sf::Vector2f(Window.getSize().x / 2, Window.getSize().y / 2 + 150), sf::Vector2f(134, 52), sf::Color::White);
		playbutton.setTexture(&playbuttontexture);
		CenterObject(&playbutton);
	}
	void CheckEvents(){
		while (Window.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed) {
				Window.close();
			}
			if (Event.type == sf::Event::MouseMoved) {
				translated_pos = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
			}
			if (Event.type == sf::Event::MouseButtonPressed) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					std::cout << "Click." << "\n";
					if (IsMouseOver(&playbutton)) {
						playClicked = true;
						Audio::PlaySound(&Audio::EffectsSounds, Audio::pickUpSound, false);
						Window.close();
					}
				}
			}
		}
	}
	void Update() {

	}
	void Draw() {
		Window.clear(sf::Color::Black);
		Window.draw(menuback);
		Window.draw(playbutton);
		Window.display();
	}
}

namespace GameOverScreen {
	sf::RectangleShape BlackRect;
	sf::Text Text, BelowText, BelowText2;
	bool isGameOver = false;
	void Init() {
		InitRect(&BlackRect, sf::Vector2f(0, 0), sf::Vector2f(50 * 20, 50 * 17), sf::Color(0, 0, 0, 180));
		InitText(&Text, 70, MyFont, "GAME OVER", sf::Vector2f(BlackRect.getSize().x / 2, (BlackRect.getSize().y / 2) - 90), MyColors::TextRed);
		CenterObject(&Text);
		InitText(&BelowText2, 30, MyFont, "Out of Fuel", sf::Vector2f(BlackRect.getSize().x / 2, (BlackRect.getSize().y / 2)-20), sf::Color(255,255,255,80));
		CenterObject(&BelowText2);
		InitText(&BelowText, 40, MyFont, "[ Press ENTER to restart ]", sf::Vector2f(BlackRect.getSize().x / 2, (BlackRect.getSize().y / 2) + 30), sf::Color::White);
		CenterObject(&BelowText);
	}
	void Draw() {
		if (isGameOver) {
			Window.draw(BlackRect);
			Window.draw(Text);
			Window.draw(BelowText);
			Window.draw(BelowText2);
		}
	}
	void CheckKeyPress();
}

namespace HUD {
	namespace Avatar {
		enum TextureCode {
			DEAD,
			HAPPY,
			NEUTRU,
			SAD,
			SMILE
		};
		sf::RectangleShape Rect;
		sf::Texture AvatarTextureArray[5];
	}
	namespace FuelBar {
		sf::RectangleShape BarBack, FuelLevelBar;
		sf::Texture BarBackTexture;
		float FuelLevelBarHeight = 100, CurFuelLevel = 100, MaxFuelLevel = 100, CurFuelRatio = 1, FuelConsumption = 2;
		void CheckFuelConsumption();
		void SetFuelLevel(const float &procentage) {
			CurFuelLevel = procentage;
			CurFuelRatio = CurFuelLevel / MaxFuelLevel;
			FuelLevelBar.setScale(1, CurFuelRatio);
		}
	}
	namespace HealthBar {
		sf::RectangleShape BarBack, HealthLevelBar;
		sf::Texture BarBackTexture;
		float HealthLevelBarHeight = 100, CurHealthLevel = 100, MaxHealthLevel = 100, CurHealthRatio = 1;
		void Damage(const float& damage);
		void SetHealthLevel(const float& procentage) {
			CurHealthLevel = procentage;
			CurHealthRatio = CurHealthLevel / MaxHealthLevel;
			HealthLevelBar.setScale(1, CurHealthRatio);
		}
	}
	void ResetHUD() {
		HUD::FuelBar::SetFuelLevel(100);
		HUD::HealthBar::SetHealthLevel(100);
		HUD::Avatar::AvatarTextureArray[HUD::Avatar::SMILE].loadFromFile("res/avatar_smile.png");
		HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::SMILE]);
	}
	void Init() {
		// Textures
		FuelBar::BarBackTexture.loadFromFile("res/fuel_indicator.png");
		HealthBar::BarBackTexture.loadFromFile("res/health_indicator.png");
		//
		Avatar::AvatarTextureArray[Avatar::DEAD].loadFromFile("res/avatar_dead.png");
		Avatar::AvatarTextureArray[Avatar::HAPPY].loadFromFile("res/avatar_happy.png");
		Avatar::AvatarTextureArray[Avatar::NEUTRU].loadFromFile("res/avatar_neutru.png");
		Avatar::AvatarTextureArray[Avatar::SAD].loadFromFile("res/avatar_sad.png");
		Avatar::AvatarTextureArray[Avatar::SMILE].loadFromFile("res/avatar_smile.png");
		Avatar::Rect.setTexture(&Avatar::AvatarTextureArray[Avatar::SMILE]);
		// Avatar
		InitRect(&Avatar::Rect, sf::Vector2f(1010, 10), sf::Vector2f(230,  230), sf::Color::White);
		// Fuel Bar
		sf::Vector2f barbackpos = sf::Vector2f(Avatar::Rect.getPosition().x+(Avatar::Rect.getSize().x/2)-70, Avatar::Rect.getPosition().y + Avatar::Rect.getSize().y + 20);
		InitRect(&FuelBar::BarBack, sf::Vector2f(barbackpos), sf::Vector2f(60, 120), sf::Color::White);
		FuelBar::BarBack.setTexture(&FuelBar::BarBackTexture);
		sf::Vector2f fuelbarpos = sf::Vector2f(FuelBar::BarBack.getPosition().x+20, FuelBar::BarBack.getPosition().y+10+ FuelBar::FuelLevelBarHeight);
		InitRect(&FuelBar::FuelLevelBar, sf::Vector2f(fuelbarpos), sf::Vector2f(30, FuelBar::FuelLevelBarHeight), sf::Color(sf::Color::Yellow));
		FuelBar::FuelLevelBar.setOrigin(0, FuelBar::FuelLevelBar.getSize().y);
		// Health Bar
		barbackpos = sf::Vector2f(Avatar::Rect.getPosition().x + (Avatar::Rect.getSize().x / 2) + 10, Avatar::Rect.getPosition().y + Avatar::Rect.getSize().y + 20);
		InitRect(&HealthBar::BarBack, sf::Vector2f(barbackpos), sf::Vector2f(60, 120), sf::Color::White);
		HealthBar::BarBack.setTexture(&HealthBar::BarBackTexture);
		fuelbarpos = sf::Vector2f(HealthBar::BarBack.getPosition().x + 10, HealthBar::BarBack.getPosition().y + 10 + HealthBar::HealthLevelBarHeight);
		InitRect(&HealthBar::HealthLevelBar, sf::Vector2f(fuelbarpos), sf::Vector2f(30, HealthBar::HealthLevelBarHeight), sf::Color(60, 170, 6));
		HealthBar::HealthLevelBar.setOrigin(0, FuelBar::FuelLevelBar.getSize().y);
	}
	void Draw() {
		Window.draw(Avatar::Rect);
		Window.draw(FuelBar::BarBack);
		Window.draw(FuelBar::FuelLevelBar);
		Window.draw(HealthBar::BarBack);
		Window.draw(HealthBar::HealthLevelBar);
	}
}

namespace HoldingItem {
	sf::RectangleShape* HoldingSubject;
}

namespace Levels {
	std::vector <sf::RectangleShape*> LevelTilesVector;
	sf::Texture arrow, cornerWall, floor, mazeWall, tepi, groapa, pereteDestructibil, pereteRamasite, tarnacop_jos, canistra_jos, repairkit_jos, scandura_jos;
}

namespace Inventory {
	const sf::Vector2i invSize = sf::Vector2i(3, 2);
	enum ItemCodes {
		NONE,
		TARNACOP,
		BATERIE,
		CANISTRA,
		REPAIR_KIT,
		SCANDURI
	};
	sf::Texture invslot, tarnacop, baterie, canistra, repair_kit, scanduri, select;
	sf::RectangleShape SelectRect;
	bool SelectVisible = false;
	std::vector<sf::RectangleShape*> PlacedScanduriV;
	struct InventorySlot {
		sf::RectangleShape BackRect, ItemRect;
		ItemCodes whatItem = NONE;
		uint8_t quantity = 0;
		sf::Text quantityText;
	};
	std::vector<InventorySlot*> InventoryVector;
	void DeletePlacedScanduri() {
		if (PlacedScanduriV.empty() == false) {
			for (auto scandura : PlacedScanduriV) {
				delete scandura;
			}
			PlacedScanduriV.clear();
		}
	}
	void Init() {
		tarnacop.loadFromFile("res/tarnacop.png");
		baterie.loadFromFile("res/baterie.png");
		canistra.loadFromFile("res/canistra.png");
		repair_kit.loadFromFile("res/repair_kit.png");
		invslot.loadFromFile("res/inv_slot.png");
		scanduri.loadFromFile("res/scanduri.png");
		select.loadFromFile("res/scanduri_select.png");
		//
		InitRect(&SelectRect, sf::Vector2f(0, 0), sf::Vector2f(50, 50), sf::Color::White);
		SelectRect.setTexture(&select);
		//
		sf::Vector2f startpos = sf::Vector2f(1050, HUD::FuelBar::BarBack.getPosition().y+HUD::FuelBar::BarBack.getSize().y + 50);
		for (int i = 0; i < invSize.y; i++) {
			for (int j = 0; j < invSize.x; j++) {
				InventorySlot* newInvSlot = new InventorySlot;
				InitRect(&newInvSlot->BackRect, sf::Vector2f(startpos.x + j * 75, startpos.y + i * 75), sf::Vector2f(60, 60), sf::Color::White);
				newInvSlot->BackRect.setTexture(&invslot);
				CenterObject(&newInvSlot->BackRect);
				// Quantity Text
				InitText(&newInvSlot->quantityText, 20, MyFont, "0", sf::Vector2f(newInvSlot->BackRect.getPosition().x + 27, newInvSlot->BackRect.getPosition().y + 20), sf::Color::White);
				CenterObject(&newInvSlot->quantityText);
				// Item Rect
				InitRect(&newInvSlot->ItemRect, sf::Vector2f(newInvSlot->BackRect.getPosition()), sf::Vector2f(50, 50), sf::Color::White);
				CenterObject(&newInvSlot->ItemRect);
				//
				InventoryVector.push_back(newInvSlot);
			}
		}
	}
	void Draw() {
		for (auto slot : InventoryVector) {
			Window.draw(slot->BackRect);
		}
		for (auto slot : InventoryVector) {
			if (slot->quantity > 0) {
				Window.draw(slot->quantityText);
				Window.draw(slot->ItemRect);
			}
		}
		if (HoldingItem::HoldingSubject != nullptr && SelectVisible) {
			Window.draw(SelectRect);
		}
	}
	void CheckSelect() {
		if (HoldingItem::HoldingSubject != nullptr) {
			if (HoldingItem::HoldingSubject->getTexture() == &Inventory::scanduri) {
				for (auto tile : Levels::LevelTilesVector) {
					if (tile->getTexture() == &Levels::groapa && IsMouseOver(tile)) {
						SelectRect.setPosition(tile->getPosition());
						SelectVisible = true;
						return;
					}
				}
				SelectVisible = false;
			}
			else if (HoldingItem::HoldingSubject->getTexture() == &Inventory::tarnacop) {
				for (auto tile : Levels::LevelTilesVector) {
					if (tile->getTexture() == &Levels::pereteDestructibil && IsMouseOver(tile)) {
						SelectRect.setPosition(tile->getPosition());
						SelectVisible = true;
						return;
					}
				}
				SelectVisible = false;
			}
		}
	}
	bool SearchIfAlreadyInInv(const ItemCodes& itemcode) {
		for (auto slot : InventoryVector) {
			if (slot->whatItem == itemcode){
				return true;
			}
		}
		return false;
	}
	void AddItem(const ItemCodes& itemcode, const int &amount) {
		if (SearchIfAlreadyInInv(itemcode) == false) {
			for (auto slot : InventoryVector) {
				if (slot->quantity == 0) {
					switch (itemcode)
					{
					case TARNACOP: slot->ItemRect.setTexture(&tarnacop); break;
					case BATERIE: slot->ItemRect.setTexture(&baterie); break;
					case CANISTRA: slot->ItemRect.setTexture(&canistra); break;
					case REPAIR_KIT: slot->ItemRect.setTexture(&repair_kit); break;
					case SCANDURI: slot->ItemRect.setTexture(&scanduri); break;
					default: break;
					}
					slot->whatItem = itemcode;
					slot->quantity += amount;
					ChangeText(&slot->quantityText, std::to_string(slot->quantity), true);
					return;
				}
			}
		}
		else {
			for (auto slot : InventoryVector) {
				if (slot->whatItem == itemcode) {
					slot->quantity += amount;
					ChangeText(&slot->quantityText, std::to_string(slot->quantity), true);
					return;
				}
			}
		}
	}
	void RemoveItem(const ItemCodes& itemcode, const int& amount) {
		if (SearchIfAlreadyInInv(itemcode) == true) {
			for (auto slot : InventoryVector) {
				if (slot->whatItem == itemcode && slot->quantity-amount>=0) {
					slot->quantity -= amount;
					ChangeText(&slot->quantityText, std::to_string(slot->quantity), true);
					if (slot->quantity == 0) {
						slot->whatItem = NONE;
					}
					return;
				}
			}
		}
		else {
			std::cout << "Nu am gasit astfel de item in inventar." << "\n";
		}
	}
	void PlaceScandura(const sf::Vector2f& pos) {
		sf::RectangleShape* newScandura = new sf::RectangleShape;
		InitRect(newScandura, sf::Vector2f(pos), sf::Vector2f(50, 50), sf::Color::White);
		newScandura->setTexture(&scanduri);
		PlacedScanduriV.push_back(newScandura);
		RemoveItem(SCANDURI, 1);
	}
	void ClearInventory() {
		if (InventoryVector.empty() == false) {
			for (auto slot : InventoryVector) {
				slot->quantity = 0;
				slot->whatItem = NONE;
			}
		}
	}
}

namespace LevelEndScreen {
	bool levelEnded = false;
}

namespace HoldingItem {
	sf::Vector2f itemsavedpos, textsavedpos;
	sf::Text* HoldingText;
	uint8_t* SubjectQuanitity;
	bool isHolding = false;
	Inventory::ItemCodes whatIsHolding = Inventory::NONE;
	void UpdatePos() {
		if (isHolding == true && HoldingSubject != nullptr){
			HoldingSubject->setPosition(translated_pos);
			HoldingText->setPosition(sf::Vector2f(HoldingSubject->getPosition().x+27, HoldingSubject->getPosition().y+20));
		}
	}
	void CheckItemUse(const Inventory::ItemCodes& itemcode);
	void CheckClick() {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && GameOverScreen::isGameOver == false && LevelEndScreen::levelEnded == false) {
			if (isHolding == false) {
				for (auto slot : Inventory::InventoryVector) {
					if (slot->BackRect.getGlobalBounds().contains(translated_pos) && slot->quantity > 0) {
						Inventory::SelectVisible = false;
						isHolding = true;
						HoldingSubject = &slot->ItemRect;
						HoldingText = &slot->quantityText;
						whatIsHolding = slot->whatItem;
						itemsavedpos = HoldingSubject->getPosition();
						textsavedpos = HoldingText->getPosition();
						SubjectQuanitity = &slot->quantity;
					}
				}
			}
			else {
				CheckItemUse(whatIsHolding);
			}
		}
	}
	void CheckKeyPress() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && isHolding == true) {
			HoldingSubject->setPosition(itemsavedpos);
			HoldingText->setPosition(textsavedpos);
			isHolding = false;
			whatIsHolding = Inventory::NONE;
			HoldingSubject = nullptr;
		}
	}
}

namespace LevelEndScreen {
	sf::RectangleShape BlackRect;
	sf::Text Text, BelowText;
	void Init() {
		InitRect(&BlackRect, sf::Vector2f(0, 0), sf::Vector2f(50 * 20, 50 * 17), sf::Color(0,0,0,180));
		InitText(&Text, 70, MyFont, "LEVEL FINISHED", sf::Vector2f(BlackRect.getSize().x / 2, (BlackRect.getSize().y / 2)-80), MyColors::TextGreen);
		CenterObject(&Text);
		InitText(&BelowText, 40, MyFont, "[ Press ENTER to continue ]", sf::Vector2f(BlackRect.getSize().x / 2, (BlackRect.getSize().y / 2)+20), sf::Color::White);
		CenterObject(&BelowText);
	}
	void Draw() {
		if (levelEnded) {
			Window.draw(BlackRect);
			Window.draw(Text);
			Window.draw(BelowText);
		}
	}
	void CheckKeyPress();
}

namespace Levels {
	bool walkableZones[20][17];
	const uint8_t levelSizeX = 20, levelSizeY = 17, totalLevelCount = 5;
	sf::RectangleShape finishRectangle;
	sf::RectangleShape startArrow, finishArrow;
	uint8_t CurLevel = 1;
}

namespace Robotel {
	sf::Vector2f startPos;
	sf::Vector2i curPos;
	sf::RectangleShape Robo;
	uint8_t moveDirection, moveSpeed = 2;
	bool isMoving = false, canMove = true, isVisible = true;
	float pasulMiscarii = 0, totalMiscare = 0;
	namespace Frames {
		sf::Texture idle;
	}
	void Init() {
		Frames::idle.loadFromFile("res/robot_idle.png");
		InitRect(&Robo,sf::Vector2f(0,0), sf::Vector2f(50, 50), sf::Color::White);
		CenterObject(&Robo);
		Robo.setTexture(&Frames::idle);
	}
	void GoToStart() {
		Robo.setPosition(sf::Vector2f(startPos.x * 50 + 25, startPos.y * 50 + 25));
		curPos = sf::Vector2i(int(startPos.x), int(startPos.y));
		isVisible = true;
	}
	void Draw() {
		if (isVisible)
			Window.draw(Robo);
	}
	enum DirectionCodes {
		UP,
		LEFT,
		DOWN,
		RIGHT,
		STOP
	};
	void Move(const DirectionCodes& directionCode) {
		moveDirection = directionCode;
		isMoving = true;
		pasulMiscarii = (float)25 / (float)moveSpeed;
		//std::cout << pasulMiscarii << "\n";
		totalMiscare = 0;
	}
	// Aceasta functie verifica ce tasta este apasata si daca este posibil ca robotelul sa se miste in acea directie.
	void CheckMovement() {
		if (isMoving == false && canMove) {
			bool validMove = false;
			switch (Event.key.code)
			{
			case sf::Keyboard::W: if ( curPos.y != 0 && Levels::walkableZones[curPos.x][curPos.y-1] == true) {
				Move(UP);
				validMove = true;
			}break;
			case sf::Keyboard::A: if (curPos.x != 0 && Levels::walkableZones[curPos.x-1][curPos.y] == true) {
				Move(LEFT);
				validMove = true;
			}break;
			case sf::Keyboard::S: if ((curPos.y != Levels::levelSizeY && Levels::walkableZones[curPos.x][curPos.y + 1] == true) || (Levels::finishArrow.getGlobalBounds().contains(Robo.getPosition()))) {
				Move(DOWN);
				validMove = true;
			}break;
			case sf::Keyboard::D: if (curPos.x != Levels::levelSizeX && Levels::walkableZones[curPos.x + 1][curPos.y] == true) {
				Move(RIGHT);
				validMove = true;
			}break;
			default: break;
			}
			if (!validMove) std::cout << "Nu poti sa te misti in aceasta directie." << "\n";
			//std::cout << curPos.x << " " << curPos.y << "\n";
		}
	}
	void CheckIfTouchingSpikes();
	void CheckIfTouchingGroapa();
	void CheckIfTouchingFinish() {
		if (Levels::finishRectangle.getGlobalBounds().contains(Robo.getPosition())) {
			canMove = false;
			LevelEndScreen::levelEnded = true;
			if (Levels::CurLevel != Levels::totalLevelCount) {
				ChangeText(&LevelEndScreen::Text, ("LEVEL " + std::to_string(Levels::CurLevel) + " COMPLETED"), true);
			}
			else if (Levels::CurLevel == Levels::totalLevelCount) {
				ChangeText(&LevelEndScreen::Text, ("GAME COMPLETED"), true);
				ChangeText(&LevelEndScreen::BelowText, ("[ Congratulations! ]"), true);
			}
			HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::HAPPY]);
		}
	}
	void CheckIfTouchingItem();
	bool CheckIfTouchingScandura() {
		for (auto scandura : Inventory::PlacedScanduriV) {
			if (scandura->getGlobalBounds().contains(Robo.getPosition())) {
				return true;
			}
		}
		return false;
	}
	// Aceasta functie este apelata atunci cand o miscare valida a fost indentificata, se itereaza pana robotelul ajunge in pozitia noua.
	void UpdatePos() {
		if (isMoving) {
			totalMiscare += pasulMiscarii;
			switch (moveDirection)
			{
			case UP: Robo.move(0, -pasulMiscarii); break;
			case LEFT: Robo.move(-pasulMiscarii, 0); break;
			case DOWN: Robo.move(0, pasulMiscarii); break;
			case RIGHT: Robo.move(pasulMiscarii, 0); break;
			default: break;
			}
			if (totalMiscare >= 50) {
				isMoving = false;
				switch (moveDirection)
				{
				case UP: curPos.y -= 1; std::cout << "Te-ai miscat in sus." << "\n"; break;
				case LEFT: curPos.x -= 1; std::cout << "Te-ai miscat la stanga." << "\n"; break;
				case DOWN: curPos.y += 1; std::cout << "Te-ai miscat in jos." << "\n"; break;
				case RIGHT: curPos.x += 1; std::cout << "Te-ai miscat la dreapta." << "\n"; break;
				default: break;
				}
				moveDirection = STOP;
				CheckIfTouchingFinish();
				if (Levels::finishRectangle.getGlobalBounds().contains(Robo.getPosition()) == false) {
					HUD::FuelBar::CheckFuelConsumption();
				}
				CheckIfTouchingSpikes();
				CheckIfTouchingGroapa();
				CheckIfTouchingItem();
			}
		}
	}
}

namespace Levels {
	std::unordered_map<char, sf::Texture*> textureMap;
	void Init() {
		textureMap['#'] = &cornerWall;
		textureMap['_'] = &floor;
		textureMap['%'] = &mazeWall;
		textureMap['^'] = &tepi;
		textureMap['o'] = &groapa;
		textureMap['P'] = &pereteDestructibil;
		textureMap['T'] = &tarnacop_jos;
		textureMap['C'] = &canistra_jos;
		textureMap['R'] = &repairkit_jos;
		textureMap['L'] = &scandura_jos;
		//
		cornerWall.loadFromFile("res/corner_wall.png");
		floor.loadFromFile("res/floor.png");
		mazeWall.loadFromFile("res/maze_wall.png");
		arrow.loadFromFile("res/arrow.png");
		tepi.loadFromFile("res/tepi.png");
		groapa.loadFromFile("res/groapa.png");
		pereteDestructibil.loadFromFile("res/stone_wall.png");
		pereteRamasite.loadFromFile("res/perete_ramasite.png");
		tarnacop_jos.loadFromFile("res/tarnacop_jos.png");
		canistra_jos.loadFromFile("res/canistra_jos.png");
		repairkit_jos.loadFromFile("res/repair_kit_jos.png");
		scandura_jos.loadFromFile("res/scanduri_jos.png");
		InitRect(&startArrow, sf::Vector2f(0, 0), sf::Vector2f(50, 50), sf::Color::White);
		startArrow.setTexture(&arrow);
		InitRect(&finishArrow, sf::Vector2f(0, 0), sf::Vector2f(50, 50), sf::Color::White);
		finishArrow.setTexture(&arrow);
	}
	void AddTile(const char &type,const sf::Vector2f &cords) {
		sf::Texture* textureAdress;
		if (strchr("#_%^oPTCRL", type) != NULL) {
			textureAdress = textureMap[type]; &cornerWall;
		} else {
		textureAdress = &floor;
		}
		if (type == 'S') {
			Robotel::startPos = cords;
			startArrow.setPosition(sf::Vector2f(cords.x * 50, cords.y * 50));
		} else if (type == 'F') {
			InitRect(&finishRectangle, sf::Vector2f(cords.x * 50, (cords.y+1) * 50), sf::Vector2f(50, 50), sf::Color::White);
			finishArrow.setPosition(sf::Vector2f(cords.x * 50, cords.y * 50));
		}
		sf::RectangleShape* newRect = new sf::RectangleShape;
		InitRect(newRect, sf::Vector2f(cords.x * 50, cords.y * 50), sf::Vector2f(50, 50), sf::Color::White);
		newRect->setTexture(textureAdress);
		LevelTilesVector.push_back(newRect);
	}
	void PrintWalkableArray() {
		for (int i = 0; i < levelSizeY; i++) {
			for (int j = 0; j < levelSizeX; j++) {
				std::cout << walkableZones[j][i];
			}
			std::cout << "\n";
		}
	}
	void CheckIfWalkable(const char& type, const sf::Vector2i& cords) {
		if (strchr("_SF^oTCRL",type) == NULL) {
			walkableZones[cords.x][cords.y] = false;
		}
		else {
			walkableZones[cords.x][cords.y] = true;
		}
	}
	// Functia citeste fisierul 'levelPath' si din informatiile citite creaza harta.
	void LoadMap(const uint8_t& level) {
		std::ifstream input;
		std::string levelPath = "level" + std::to_string(level) + ".txt";
		input.open("levels\\" + levelPath);
		for (int i = 0; i < levelSizeY; i++) {
			std::string inputString;
			std::getline(input, inputString);
			std::cout << inputString << "\n"; // DEBUG
			for (int j = 0; j < levelSizeX; j++) {
				AddTile(inputString[j],sf::Vector2f(float(j),float(i)));
				CheckIfWalkable(inputString[j],sf::Vector2i(j,i));
			}
		}
	}
	void DeleteTiles() {
		if (LevelTilesVector.empty() == false) {
			for (auto tile : LevelTilesVector) {
				delete tile;
			}
			LevelTilesVector.clear();
		}
		std::cout << "LevelTilesVector cleared succesfully!" << "\n";
	}
	void LoadLevel(const uint8_t &level) {
		DeleteTiles();
		LoadMap(level);
		Robotel::GoToStart();
		Levels::PrintWalkableArray();
		HUD::ResetHUD();
		Inventory::DeletePlacedScanduri();
		Inventory::ClearInventory();
		std::cout << "Level loaded successfully!" << "\n";
	}
	void Draw() {
		for (auto tile : LevelTilesVector) {
			Window.draw(*tile);
		}
		Window.draw(finishRectangle); // DEBUG
		Window.draw(startArrow);
		Window.draw(finishArrow);
		for (auto scandura : Inventory::PlacedScanduriV) {
			Window.draw(*scandura);
		}
	}
}

void Robotel::CheckIfTouchingSpikes() {
	for (auto tile : Levels::LevelTilesVector) {
		if (tile->getTexture() == &Levels::tepi && tile->getGlobalBounds().contains(Robotel::Robo.getPosition())) {
			HUD::HealthBar::Damage(10);
			if (HUD::HealthBar::CurHealthRatio <= 0.40) {
				HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::SAD]);
			}
			return;
		}
	}
}

void Robotel::CheckIfTouchingGroapa() {
	for (auto tile : Levels::LevelTilesVector) {
		if (tile->getTexture() == &Levels::groapa && tile->getGlobalBounds().contains(Robotel::Robo.getPosition()) && CheckIfTouchingScandura() == false) {
			GameOverScreen::isGameOver = true;
			Robotel::isVisible = false;
			HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::DEAD]);
			ChangeText(&GameOverScreen::BelowText2, "The robot fell into a hole", true);
			return;
		}
	}
}

void Robotel::CheckIfTouchingItem() {
	for (auto tile : Levels::LevelTilesVector) {
		if (tile->getGlobalBounds().contains(Robo.getPosition())) {
			if (tile->getTexture() == &Levels::tarnacop_jos) {
				tile->setTexture(&Levels::floor);
				Inventory::AddItem(Inventory::TARNACOP, 1);
				Audio::PlaySound(&Audio::EffectsSounds,Audio::pickUpSound,false);
			}
			else if (tile->getTexture() == &Levels::canistra_jos) {
				tile->setTexture(&Levels::floor);
				Inventory::AddItem(Inventory::CANISTRA, 1);
				Audio::PlaySound(&Audio::EffectsSounds, Audio::pickUpSound, false);
			}
			else if (tile->getTexture() == &Levels::repairkit_jos) {
				tile->setTexture(&Levels::floor);
				Inventory::AddItem(Inventory::REPAIR_KIT, 1);
				Audio::PlaySound(&Audio::EffectsSounds, Audio::pickUpSound, false);
			}
			else if (tile->getTexture() == &Levels::scandura_jos) {
				tile->setTexture(&Levels::floor);
				Inventory::AddItem(Inventory::SCANDURI, 1);
				Audio::PlaySound(&Audio::EffectsSounds, Audio::pickUpSound, false);
			}
		} 
	}
}

void LevelEndScreen::CheckKeyPress() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && levelEnded) {
		if (Levels::CurLevel != Levels::totalLevelCount) {
			Levels::LoadLevel(Levels::CurLevel + 1);
			levelEnded = false;
			Robotel::canMove = true;
			Levels::CurLevel++;
		}
	}
}

void GameOverScreen::CheckKeyPress() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && isGameOver) {
		Levels::LoadLevel(Levels::CurLevel);
		Robotel::canMove = true;
		isGameOver = false;
	}
}

void HUD::FuelBar::CheckFuelConsumption() {
	if ((CurFuelLevel - FuelConsumption) >= 0) {
		CurFuelLevel -= FuelConsumption;
		CurFuelRatio = CurFuelLevel / MaxFuelLevel;
		FuelLevelBar.setScale(1,CurFuelRatio);
		if (HUD::FuelBar::CurFuelRatio <= 0.40) {
			HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::SAD]);
		}
	}
	if (CurFuelLevel == 0) {
		Robotel::canMove = false;
		std::cout << "Out of fuel!" << "\n";
		GameOverScreen::isGameOver = true;
		ChangeText(&GameOverScreen::BelowText2, "Out of fuel", true);
		HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::DEAD]);
	}
}

void HUD::HealthBar::Damage(const float& damage) {
	if ((CurHealthLevel - damage)>=0) {
		CurHealthLevel -= damage;
		CurHealthRatio = CurHealthLevel / MaxHealthLevel;
		HealthLevelBar.setScale(1, CurHealthRatio);
	}
	if (CurHealthLevel == 0) {
		Robotel::canMove = false;
		std::cout << "Robot died!" << "\n";
		GameOverScreen::isGameOver = true;
		ChangeText(&GameOverScreen::BelowText2, "The robot got destroyed", true);
		HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::DEAD]);
	}
}

void HoldingItem::CheckItemUse(const Inventory::ItemCodes& itemcode) {
	switch (itemcode)
	{
	case Inventory::CANISTRA: {
		if (IsMouseOver(&HUD::FuelBar::BarBack) && HUD::FuelBar::CurFuelRatio != 1) {
			HUD::FuelBar::SetFuelLevel(100);
			Inventory::RemoveItem(Inventory::CANISTRA, 1);
			HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::SMILE]);
		}
	}break;
	case Inventory::REPAIR_KIT: {
		if (IsMouseOver(&HUD::HealthBar::BarBack) && HUD::HealthBar::CurHealthRatio != 1) {
			HUD::HealthBar::SetHealthLevel(100);
			Inventory::RemoveItem(Inventory::REPAIR_KIT, 1);
			HUD::Avatar::Rect.setTexture(&HUD::Avatar::AvatarTextureArray[HUD::Avatar::SMILE]);
		}
	}
	case Inventory::SCANDURI: {
		sf::RectangleShape Zone;
		if (HoldingItem::HoldingSubject != nullptr && HoldingItem::HoldingSubject->getTexture() == &Inventory::scanduri) {
			for (auto tile : Levels::LevelTilesVector) {
				if (tile->getTexture() == &Levels::groapa && IsMouseOver(tile)) {
					Zone.setPosition(tile->getPosition().x - 50, tile->getPosition().y - 50);
					Zone.setSize(sf::Vector2f(150, 150));
					if (Zone.getGlobalBounds().contains(Robotel::Robo.getPosition())) {
						Inventory::PlaceScandura(tile->getPosition());
					}
					else {
						std::cout << "Esti prea departe!" << "\n";
					}
				}
			}
		}
	}
	case Inventory::TARNACOP: {
		sf::RectangleShape Zone;
		if (HoldingItem::HoldingSubject != nullptr && HoldingItem::HoldingSubject->getTexture() == &Inventory::tarnacop) {
			for (auto tile : Levels::LevelTilesVector) {
				if (tile->getTexture() == &Levels::pereteDestructibil && IsMouseOver(tile)) {
					Zone.setPosition(tile->getPosition().x - 50, tile->getPosition().y - 50);
					Zone.setSize(sf::Vector2f(150, 150));
					if (Zone.getGlobalBounds().contains(Robotel::Robo.getPosition())) {
						Inventory::RemoveItem(Inventory::TARNACOP, 1);
						tile->setTexture(&Levels::pereteRamasite);
						Levels::walkableZones[int(tile->getPosition().x / 50)][int(tile->getPosition().y / 50)] = true;
						Audio::PlaySound(&Audio::EffectsSounds, Audio::wallBreak,false);
					}
					else {
						std::cout << "Esti prea departe!" << "\n";
					}
				}
			}
		}
	}
	default: break;
	}
	if (*SubjectQuanitity == 0) {
		HoldingSubject->setPosition(itemsavedpos);
		HoldingText->setPosition(textsavedpos);
		isHolding = false;
		HoldingSubject = nullptr;
		whatIsHolding = Inventory::NONE;
	}
}

void InitWindow() {
	Window.create(sf::VideoMode(1250, 850), "Robot Puzzle Game", sf::Style::Close);
	Window.setFramerateLimit(60);
	MyFont.loadFromFile("res/BAHNSCHRIFT.TTF");
}

// Aici se initializeaza majoritatea lucrurilor
void InitAll() {
	Audio::PlaySound(&Audio::BackMusicSounds, Audio::backMusic, true);
	InitWindow();
	Levels::Init();
	Robotel::Init();
	Levels::LoadLevel(Levels::CurLevel);
	LevelEndScreen::Init();
	GameOverScreen::Init();
	HUD::Init();
	Inventory::Init();
}

// Aceasta functie actualizeaza coordonatele cursorului (doar atunci cand mouse-ul este miscat).
void UpdateMouseCords() {
	translated_pos = Window.mapPixelToCoords(sf::Mouse::getPosition(Window));
	//std::cout << translated_pos.x << "," << translated_pos.y << "\n"; // DEBUG
}

void CheckOtherEvents() {
	if (Event.type == sf::Event::MouseMoved) {
		UpdateMouseCords();
		Inventory::CheckSelect();
	}
	if (Event.type == sf::Event::KeyPressed) {
		Robotel::CheckMovement();
		LevelEndScreen::CheckKeyPress();
		GameOverScreen::CheckKeyPress();
		HoldingItem::CheckKeyPress();
	}
	if (Event.type == sf::Event::MouseButtonPressed) {
		HoldingItem::CheckClick();
	}
}

// Aici se verifica daca a fost apasat butonul close al ferestrei apoi alte event-uri.
void CheckEvents() {
	while (Window.pollEvent(Event)) {
		if (Event.type == sf::Event::Closed) {
			Window.close();
		}
		CheckOtherEvents();
	}
}

void Update() {
	Robotel::UpdatePos();
	HoldingItem::UpdatePos();
}

void DrawAll() {
	Levels::Draw();
	Robotel::Draw();
	LevelEndScreen::Draw();
	GameOverScreen::Draw();
	HUD::Draw();
	Inventory::Draw();
}

void Draw() {
	Window.clear(sf::Color::Black);
	DrawAll();
	Window.display();
}

int main()
{
	MainMenu::Init();
	while (Window.isOpen()) {
		MainMenu::CheckEvents();
		MainMenu::Update();
		MainMenu::Draw();
	}
	if (MainMenu::playClicked == false)
		return 0;
	InitAll();
	while (Window.isOpen()) {
		CheckEvents();
		Update();
		Draw();
	}
}