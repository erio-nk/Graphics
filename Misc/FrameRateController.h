#pragma once
	
/*!
	@brief	フレームレート制御クラス。
	@author	E.Nakayama
*/
class FrameRateController
{
public:
	FrameRateController();

	~FrameRateController();
	
	/*!
		@brief	フレームレートを設定する。
		@param	fps	FPS。
	*/
	void SetFrameRate(unsigned int fps);
	
	/*!
		@brief	FPS算出の為のサンプルフレーム数を設定する。
		@param	num	サンプル数。デフォルトでは10。
	*/
	void SetNumFrameTimeSamples(size_t num);
	
	/*!
		@brief	１フレームの目標時間を取得する。
				GetElapsedTime の戻り値がこの関数より大きい場合、フレームスキップを検討する必要がある。
	*/
	double GetFrameTime() const;
	
	/*!
		@brief	現在FPSを取得する。
	*/
	double GetCurrentFPS() const;
	
	/*!
		@brief	前回フレームからの経過時間を取得する。
	*/
	double GetElapsedTime() const;
	
	/*!
		@brief	フレームの切り替わりを通知し、フレームレートを制御する為に必要であれば待機する。
	*/
	void ChangeFrame();

private:
	struct Impl;
	std::auto_ptr<Impl> _pimpl;
	
};	// end class FrameRateController
