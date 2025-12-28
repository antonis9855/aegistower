#pragma once
#include "widget.h"
#include <string>
#include "globalstate.h"

class Button : public Widget
{
public:
	enum Button_type
	{
		Tower_button,
		HELTH,
		PAUSE,
		INVENTORY,
		PLAY,
		QUIT,
		SETTINGS,

	};;
	Button(Button_type m_type, float pos_x, float pos_y, std::string m_button_texture, std::string m_text = "", int m_gold = 0,
		float m_dimension_x = 120.0f, float m_dimension_y = 120.0f
		, Text_alignment h = CENTER, Text_alignment v = BOTTOM,
		float m_font_size = 23.0f, float r = 194.0f, float g = 151.0f, float b = 112.0f, bool m_interactiv = true)
		: Widget()
	{
		this->setColor(r, g, b);
		this->setDimensions(m_dimension_x, m_dimension_y);
		this->setPosition(pos_x, pos_y);
		this->set_font_text(m_text);
		this->set_text_alignment_h(h, v);
		this->set_font_size(m_font_size);
		this->setInteractive(m_interactive);
		m_gold_worth = m_gold;
		m_type_of_button = m_type;
		m_button_texture = m_button_texture;
		m_widget_brush.fill_color[0] = { m_color_r / 255.0f };
		m_widget_brush.fill_color[1] = { m_color_g / 255.0f };
		m_widget_brush.fill_color[2] = { m_color_b / 255.0f };
		m_widget_brush.texture = Globalstate().getImagesDir() + m_button_texture;
	}
	Button_type getButtonType() const
	{
		return m_type_of_button;
	}

private:
	Button_type m_type_of_button;
	std::string m_button_texture;
	int m_gold_worth;
};