#include <boost/timer/timer.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/noncopyable.hpp>

#include <Windows.h>

#include "DebugTrace.h"

#include "FrameRateController.h"
	
using namespace boost;

namespace
{
	static inline double NanoSecondsToSeconds(timer::nanosecond_type ns)
	{
		return ns * 0.001 * 0.001 * 0.001;
	}
	
	static inline timer::nanosecond_type SecondsToNanoSeconds(double s)
	{
		return static_cast<timer::nanosecond_type>(s * 1000.0 * 1000.0 * 1000.0);
	}

	static const timer::nanosecond_type ONE_SECOND = SecondsToNanoSeconds(1);
	static const unsigned int FPS_DEFAULT = 60;
	static const size_t NUM_FRAME_TIME_SAMPLES_DEFAULT = 8;
	static const size_t NUM_SLEEP_TIME_SAMPLES_DEFAULT = 10;
}

// ----------------------------------------------------------------------------
// FrameRateController クラスの実装部。

struct FrameRateController::Impl : private boost::noncopyable
{
	timer::cpu_timer _timer;

	timer::nanosecond_type _frameTime;
	timer::nanosecond_type _elapsedTime;
	timer::nanosecond_type _sleepTime;

	double _currentFPS;
	
	typedef circular_buffer<timer::nanosecond_type> FrameTimeSamples;
	FrameTimeSamples _frameTimeSamples;
	timer::nanosecond_type _frameTimeSum;
	
	Impl()
		: _elapsedTime(0)
		, _currentFPS(0.f)
	{
		SetFrameRate(FPS_DEFAULT);
		SetNumFrameTimeSamples(NUM_FRAME_TIME_SAMPLES_DEFAULT);
		
		// Sleep(1) の精度計測。
		timer::cpu_times tmp;
		_sleepTime = 0;
		for (int i = 0; i < NUM_SLEEP_TIME_SAMPLES_DEFAULT; ++i)
		{
			_timer.start();
			Sleep(1);
			tmp = _timer.elapsed();
			_sleepTime += tmp.wall;
		}
		_sleepTime /= NUM_SLEEP_TIME_SAMPLES_DEFAULT;

//		DebugTrace(_T("%f"), NanoSecondsToSeconds(_sleepTime));

		// 開始。
		_timer.start();
	}
	
	~Impl()
	{
	}
	
	void SetFrameRate(unsigned int fps)
	{
		_frameTime = ONE_SECOND / fps;
	}
	
	void SetNumFrameTimeSamples(size_t num)
	{
		_frameTimeSamples.resize(num);
		std::fill(_frameTimeSamples.begin(), _frameTimeSamples.end(), _frameTime);
		_frameTimeSum = _frameTime * num;
	}
	
	double GetFrameTime()
	{
		return NanoSecondsToSeconds(_frameTime);
	}
	
	double GetCurrentFPS()
	{
		return _currentFPS;
	}
	
	double GetElapsedTime()
	{
		return NanoSecondsToSeconds(_elapsedTime);
	}
	
	void ChangeFrame()
	{
		// FPS 計算。
		_frameTimeSum -= _frameTimeSamples.front();
		_frameTimeSamples.pop_front();
		_frameTimeSum += _elapsedTime;
		_frameTimeSamples.push_back(_elapsedTime);
		timer::nanosecond_type frameTimeAverage = _frameTimeSum / _frameTimeSamples.size();
		assert(frameTimeAverage);
		_currentFPS = static_cast<double>(ONE_SECOND) / static_cast<double>(frameTimeAverage);

//		DebugTrace(_T("%f"), NanoSecondsToSeconds(frameTimeAverage));

		// フレームレート制御。
		timer::cpu_times current;
		for (;;)
		{
			current = _timer.elapsed();
			if (_frameTime >= current.wall)
			{
				if (_frameTime - current.wall > _sleepTime)
				{
					Sleep(1);
					continue;
				}
				continue;
			}
			break;
		}
		
		// 経過時間保存。
		_elapsedTime = current.wall;
		_timer.start();	// 経過時間リセット。
	}

};	// end struct FrameRateController::Impl

// ----------------------------------------------------------------------------
// FrameRateController クラスの転送部。

FrameRateController::FrameRateController()
{
	_pimpl.reset(new Impl());
}

FrameRateController::~FrameRateController()
{
}

void FrameRateController::SetFrameRate(unsigned int fps)
{
	_pimpl->SetFrameRate(fps);
}

void FrameRateController::SetNumFrameTimeSamples(size_t num)
{
	_pimpl->SetNumFrameTimeSamples(num);
}

double FrameRateController::GetFrameTime() const
{
	return _pimpl->GetFrameTime();
}

double FrameRateController::GetCurrentFPS() const
{
	return _pimpl->GetCurrentFPS();
}

double FrameRateController::GetElapsedTime() const
{
	return _pimpl->GetElapsedTime();
}

void FrameRateController::ChangeFrame()
{
	_pimpl->ChangeFrame();
}
