#pragma once
#include <string>
#include <functional>
#include <sgg/scancodes.h>
#include <vector>




namespace graphics
{

	typedef enum {
		CANVAS_SCALE_WINDOW = 0,
		CANVAS_SCALE_STRETCH,
		CANVAS_SCALE_FIT
	}
	scale_mode_t;


	struct Brush
	{
		float fill_color[3] = {1.f,1.f,1.f};


		float fill_secondary_color[3] = { 1.f,1.f,1.f };


		float fill_opacity = 1.0f;



		float fill_secondary_opacity = 1.f;


		float outline_color[3] = { 1.f,1.f,1.f };



		float outline_opacity = 1.f;


		float outline_width = 1.0f;


		std::string texture = "";




































		bool gradient = false;


		float gradient_dir_u = 0.0f;







		float gradient_dir_v = 1.0f;
	};


	struct MouseState
	{
		bool button_left_pressed;
		bool button_middle_pressed;
		bool button_right_pressed;
		bool button_left_released;
		bool button_middle_released;
		bool button_right_released;
		bool button_left_down;
		bool button_middle_down;
		bool button_right_down;
		bool dragging;
		int cur_pos_x;
		int cur_pos_y;
		int prev_pos_x;
		int prev_pos_y;
	};





	void createWindow(int width, int height, std::string title);


	void setWindowBackground(Brush style);


	void destroyWindow();


	void startMessageLoop();


	void stopMessageLoop();


	void setCanvasSize(float w, float h);


	void setCanvasScaleMode(scale_mode_t sm);


	void setFullScreen(bool fs);


	float windowToCanvasX(float x, bool clamped = true);


	float windowToCanvasY(float y, bool clamped = true);


	void setUserData(const void* user_data);


	void* getUserData();






	void setDrawFunction(std::function<void()> draw);


	void setPreDrawFunction(std::function<void()> predraw);


	void setPostDrawFunction(std::function<void()> postdraw);


	void setUpdateFunction(std::function<void(float)> update );


	void setResizeFunction(std::function<void(int, int)> resize);




	void getMouseState(MouseState & ms);



	bool getKeyState(scancode_t key);





	float getDeltaTime();


	float getGlobalTime();





	void drawRect(float center_x, float center_y, float width, float height, const Brush & brush);


	void drawBezier(float *ep1, float *cp1, float *cp2, float *ep2, const Brush& brush);



	void drawLine(float x1, float y1, float x2, float y2, const Brush & brush);


	void drawDisk(float cx, float cy, float radius, const Brush & brush);


	void drawSector(float cx, float cy, float radius1, float radius2, float start_angle, float end_angle, const Brush & brush);


	bool setFont(std::string fontname);


	void drawText(float pos_x, float pos_y, float size, const std::string & text, const Brush & brush);


	void setOrientation(float angle);


	void setScale(float sx, float sy);


	void resetPose();






	std::vector<std::string> preloadBitmaps(const std::string & dir);


	bool getBitmapData(const std::string & bitmap_name, unsigned char ** buffer, unsigned int * width, unsigned int * height);


	bool updateBitmapData(const std::string& bitmap_name, const unsigned char* buffer);








	void playSound(std::string soundfile, float volume, bool looping = false);


	void playMusic(std::string soundfile, float volume, bool looping = true, int fade_time = 0);


	void stopMusic(int fade_time = 0);


}
