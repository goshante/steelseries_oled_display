#include "Canvas.h"
#include "Utils.h"

Canvas::Canvas(int w, int h, const std::string& title, uint32_t brush, uint32_t background)
	: _pw(nullptr)
	, _title(title)
	, _width(w)
	, _height(h)
	, _background(background)
	, _brush(brush)
	, _paused(false)
{
	_frame = std::vector<std::vector<pixel_t>>(_width);
	InitBitmap(_frame, _height, _width);
	_thread = std::make_shared<std::thread>(&Canvas::_draw, this);
}

Canvas::~Canvas()
{
	Show(false);
}

void Canvas::Push(const bitmap_t& picture)
{
	_lock.lock();
	_frame = picture;
	_lock.unlock();
}

void Canvas::Show(bool show)
{
	if (!show)
	{
		_paused = true;
		_thread->detach();
	}
	else
	{
		_paused = false;
		_thread = std::make_shared<std::thread>(&Canvas::_draw, this);
	}
}

void Canvas::_draw()
{
	_pw = std::make_shared<pw::PixelWindow>(_width, _height, _title.c_str());
	while (!_paused)
	{
		_pw->makeCurrent();
		_pw->pollEvents();
		_pw->beginFrame();
		_pw->setBackgroundColor(_background);
		_lock.lock();
		for (int y = 0; y < Min(_height, _frame.size()); y++)
		{
			for (int x = 0; x < Min(_width, _frame[y].size()); x++)
			{
				if (_frame[y][x] > 0)
					_pw->setPixel(x, y, _brush);
			}
		}
		_lock.unlock();
		_pw->endFrame();
	}
	_pw->forceClose();
}