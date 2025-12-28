#include "globalstate.h"
#include "sgg/graphics.h"
#include "Display.h"
#include "button.h"	

Globalstate* Globalstate::m_instance = nullptr;

Globalstate::Globalstate()
{
}

Globalstate::~Globalstate()
{
	clearWidgets();
}

Globalstate* Globalstate::getInstance()
{
	if (m_instance == nullptr)

		m_instance = new Globalstate();

	return m_instance;
}


void Globalstate::draw_menu()
{



	for (Widget* widget : m_Allwidget)
	{
		Button* button = dynamic_cast<Button*>(widget);
		if (button != nullptr && button->getButtonType() == Button::Button_type::PLAY)
		{
			widget->draw();
		}
		ResourceDisplay* resource_display = dynamic_cast<ResourceDisplay*>(widget);
		if (resource_display != nullptr)
		{
			widget->draw();
		}
	}


}

void Globalstate::draw_playingmode()
{
	for (Widget* widget : m_Allwidget)
	{
		widget->draw();
	}
}

void Globalstate::draw_editormode()
{
}

void Globalstate::clearWidgets()
{
	for (Widget* widget : m_Allwidget)
	{
		delete widget;
	}
	m_Allwidget.clear();
}

void Globalstate::update_menu()
{

	ResourceDisplay* resource_display = new ResourceDisplay("menu.png", false, 0.0f, 0.0f, 1920.0f, 1080.0f);
	m_Allwidget.push_back(resource_display);
	m_Allwidget.push_back(new Button(Button::PLAY, 810.0f, 465.0f,
		"play_button.png", "", 0, 360.0f, 200.0f));



}

void Globalstate::update_playingmode()
{
	//Initialize widgets
	ResourceDisplay* resource_display = new ResourceDisplay();
	m_Allwidget.push_back(resource_display);
	float start_x = 400.0f;
	float spacing = 160.0f;
	float y = 900.0f;

	m_Allwidget.push_back(new Button(Button::Tower_button, start_x, y,
		"BlueTower.png", "100", 100));

	m_Allwidget.push_back(new Button(Button::Tower_button, start_x + spacing, y,
		"Purple_tower.png", "300", 300));

	m_Allwidget.push_back(new Button(Button::Tower_button, start_x + 2 * spacing, y,
		"RedTower.png", "800", 800));

	m_Allwidget.push_back(new Button(Button::Tower_button, start_x + 3 * spacing, y,
		"GoldenTower.png", "2000", 2000));
	m_Allwidget.push_back(new Button(Button::HELTH, start_x + 4 * spacing, y,
		"Castle.png", " = 25", 0, 260.0f, 160.0f, Widget::Text_alignment::RIGHT, Widget::Text_alignment::CENTER, 60.0f));
	m_Allwidget.push_back(new Button(Button::INVENTORY, start_x + 4 * spacing + 260.0f + 20.0f + 150.0f, y,
		"coins.png", "100", 0, 120.0f, 120.0f, Widget::Text_alignment::CENTER, Widget::Text_alignment::BOTTOM, 30.0f));


}

void Globalstate::update_editormoder()
{
}

void Globalstate::draw()
{
	//Draw all widgets
	switch (m_current_mode)
	{
	case GameMode::MENU:
		draw_menu();
		break;

	case GameMode::PLAYINGMODE:
		draw_playingmode();
		break;
	case GameMode::EDITORMODE:
		draw_editormode();
		break;
	}

}

void Globalstate::init()
{
	Globalstate::getInstance()->m_canvas_width = 1920.0f;
	Globalstate::getInstance()->m_canvas_height = 1080.0f;
	Globalstate::getInstance()->m_current_mode = GameMode::MENU;

}


void Globalstate::update(float dt)
{
	graphics::getMouseState(m_mouse);

	static GameMode last_mode = m_current_mode;

	for (Widget* widget : m_Allwidget)
	{
		widget->update((float)(m_mouse.cur_pos_x), (float)(m_mouse.cur_pos_y), m_mouse.button_left_down);
	}


	bool mode_changed = false;
	for (Widget* widget : m_Allwidget)
	{
		Button* button = dynamic_cast<Button*>(widget);
		if (button != nullptr &&
			button->getButtonType() == Button::Button_type::PLAY &&
			button->check_if_clicked())
		{
			m_current_mode = GameMode::PLAYINGMODE;
			mode_changed = true;
			break;
		}
	}

	if (mode_changed || last_mode != m_current_mode)
	{
		clearWidgets();
		last_mode = m_current_mode;
	}


	if (m_Allwidget.empty())
	{
		switch (m_current_mode)
		{
		case GameMode::MENU:
			update_menu();
			break;
		case GameMode::PLAYINGMODE:
			update_playingmode();
			break;
		case GameMode::EDITORMODE:
			update_editormoder();
			break;
		}
	}
}
void Globalstate::Run()
{
	// Initialize the graphics system and create the game window
	init();
	graphics::createWindow(m_canvas_width, m_canvas_height, "Tower Defense Game");
	graphics::setCanvasSize(m_canvas_width, m_canvas_height);
	graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

	// Pass callables with the correct signatures. Can't pass non-static member directly.
	graphics::setDrawFunction([]() { Globalstate::getInstance()->draw(); });
	graphics::setUpdateFunction([](float dt) { Globalstate::getInstance()->update(dt); });


	graphics::startMessageLoop();
	graphics::destroyWindow();
}