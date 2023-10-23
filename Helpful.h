#pragma once
#include "SFML/Graphics.hpp"
#include <string>

extern sf::RenderWindow Window;
extern sf::Vector2f translated_pos;

namespace MyColors {
	const sf::Color
		Blue = sf::Color(52, 101, 164),
		DarkBlue = sf::Color(53, 82, 105),
		Gray = sf::Color(153, 153, 153),
		DarkGray = sf::Color(56, 61, 60),
		LightGreen = sf::Color(235, 255, 228),
		LightRed = sf::Color(255, 229, 229),
		LightBlue = sf::Color(237, 235, 255),
		TextGreen = sf::Color(129, 212, 26),
		TextRed = sf::Color(255, 109, 109),
		TextBlue = sf::Color(114, 159, 222);
}

void InitRect(sf::RectangleShape* subject, const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color) {
	subject->setPosition(pos);
	subject->setSize(size);
	subject->setFillColor(color);
}

void InitCircle(sf::CircleShape* subject, const sf::Vector2f& pos, const float& radius, const sf::Color& color) {
	subject->setPosition(pos);
	subject->setRadius(radius);
	subject->setFillColor(color);
}

void InitText(sf::Text* subiect, const short int& charsize, const sf::Font& font, const std::string& string, const sf::Vector2f& pos, const sf::Color charcolor) {
	subiect->setCharacterSize(charsize);
	subiect->setFont(font);
	subiect->setString(string);
	subiect->setPosition(pos);
	subiect->setFillColor(charcolor);
}

void CenterObject(sf::RectangleShape* subject);
void CenterObject(sf::CircleShape* subject);
void CenterObject(sf::Text* subject);

void CenterObject(sf::RectangleShape* subject) { subject->setOrigin(subject->getSize().x / 2, subject->getSize().y / 2); }
void CenterObject(sf::CircleShape* subject) { subject->setOrigin(subject->getRadius() / 2, subject->getRadius() / 2); }
void CenterObject(sf::Text* subject) { subject->setOrigin(subject->getLocalBounds().width / 2, subject->getLocalBounds().height / 2); }

void ChangeText(sf::Text* subiect, const std::string& string, const bool& centered) {
	subiect->setString(string);
	if (centered) CenterObject(subiect);
}

bool IsMouseOver(const sf::RectangleShape* x) {
	if (x->getGlobalBounds().contains(translated_pos.x, translated_pos.y)) {
		return true;
	}
	else {
		return false;
	}
}