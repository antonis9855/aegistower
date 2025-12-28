#pragma once
#include "sgg/graphics.h"
#include <string>
#include <vector>	
#include "widget.h"	

class Globalstate
{
public:
	enum class GameMode
	{
		MENU,
		PLAYINGMODE,
		EDITORMODE,

	};;

	Globalstate();
	~Globalstate();
	std::string getImagesDir() const { return m_images_path; }
	std::string getFontsDir() const { return m_fonts_path; }
	static Globalstate* getInstance();
	float getMouse_pos_x() const { return (float)(m_mouse.cur_pos_x); }
	float getMouse_pos_y() const { return (float)(m_mouse.cur_pos_y); }
	bool ismouse_left_pressed() const { return m_mouse.button_left_pressed; }

	void clearWidgets();

	void update_menu();
	void update_playingmode();
	void update_editormoder();

	void draw_menu();
	void draw_playingmode();
	void draw_editormode();
	//Initialization, drawing and updating methods
	void init();
	void draw();
	void update(float dt);
	//Main game loop
	void Run();
private:
	//Assets path
	std::string m_images_path = "assets/";
	std::string m_fonts_path = m_images_path + "Kreon Regular.ttf";
	//Canvas dimensions
	float m_canvas_width;
	float m_canvas_height;
	//Mouse state
	graphics::MouseState m_mouse;
	//Singleton instance
	static Globalstate* m_instance;


	std::vector<Widget*> m_Allwidget;

	bool m_mode_change_pending;
	float m_transition_timer;
	float m_transition_duration;

	GameMode m_current_mode;
};


