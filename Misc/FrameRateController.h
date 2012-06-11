#pragma once
	
/*!
	@brief	�t���[�����[�g����N���X�B
	@author	E.Nakayama
*/
class FrameRateController
{
public:
	FrameRateController();

	~FrameRateController();
	
	/*!
		@brief	�t���[�����[�g��ݒ肷��B
		@param	fps	FPS�B
	*/
	void SetFrameRate(unsigned int fps);
	
	/*!
		@brief	FPS�Z�o�ׂ̈̃T���v���t���[������ݒ肷��B
		@param	num	�T���v�����B�f�t�H���g�ł�10�B
	*/
	void SetNumFrameTimeSamples(size_t num);
	
	/*!
		@brief	�P�t���[���̖ڕW���Ԃ��擾����B
				GetElapsedTime �̖߂�l�����̊֐����傫���ꍇ�A�t���[���X�L�b�v����������K�v������B
	*/
	double GetFrameTime() const;
	
	/*!
		@brief	����FPS���擾����B
	*/
	double GetCurrentFPS() const;
	
	/*!
		@brief	�O��t���[������̌o�ߎ��Ԃ��擾����B
	*/
	double GetElapsedTime() const;
	
	/*!
		@brief	�t���[���̐؂�ւ���ʒm���A�t���[�����[�g�𐧌䂷��ׂɕK�v�ł���Αҋ@����B
	*/
	void ChangeFrame();

private:
	struct Impl;
	std::auto_ptr<Impl> _pimpl;
	
};	// end class FrameRateController
