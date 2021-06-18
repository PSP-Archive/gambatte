//
//   Copyright (C) 2007 by sinamas <sinamas at users.sourceforge.net>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License version 2 for more details.
//
//   You should have received a copy of the GNU General Public License
//   version 2 along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include <common/adaptivesleep.h>
#include <gambatte_sdl/audiosink.h>
#include <gambatte_sdl/blitterwrapper.h>
#include <common/resample/resampler.h>
#include <common/resample/resamplerinfo.h>
#include <common/skipsched.h>
#include <common/videolink/vfilterinfo.h>
#include <gambatte/gambatte.h>
#include <gambatte/pakinfo.h>
#include <SDL/SDL.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


#include <pspkernel.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <pspdisplay.h>
#include <psppower.h>


#include <unistd.h>
#include "dirent.h"


char rom_filename[256];
uint16_t audio_latency = 133;
extern void memcpy_vfpu( void* dst, const void* src, uint32_t size);

namespace {

using namespace gambatte;


class RateOption {
public:
	RateOption()
	: rate_(48000)
	{
	}

	 void exec(long r) {
		if (r < 4000 || r > 192000)
			return;

		rate_ = r;
	}

	long rate() const { return rate_; }

private:
	long rate_;
};

class LatencyOption{
public:
	LatencyOption()
	: latency_(266)
	{
	}

	void exec(int l) {
		if (l < 16 || l > 5000)
			return;

		latency_ = l;
	}

	int latency() const { return latency_; }

private:
	int latency_;
};

class PeriodsOption {
public:
	PeriodsOption()
	: periods_(4)
	{
	}

	void exec(int p) {
		if (p < 1 || p > 32)
			return;

		periods_ = p;
	}

	int periods() const { return periods_; }

private:
	int periods_;
};

class ScaleOption{
public:
	ScaleOption()
	: scale_(1)
	{
	}

	void exec(int s) {
		if (s < 1 || s > 40)
			return;

		scale_ = s;
	}

	int scale() const { return scale_; }

private:
	int scale_;
};

class VfOption {
public:
	VfOption()
	: filterNo_(0)
	{
	}
	
	void exec(unsigned long fno) {
		if (fno < VfilterInfo::numVfilters())
			filterNo_ = fno;
	}

	VfilterInfo const & filter() const { return VfilterInfo::get(filterNo_); }

private:
	std::size_t filterNo_;
};

class ResamplerOption{
public:
	ResamplerOption()
	: resamplerNo_(1)
	{
	}
	
	void exec(unsigned long n) {
		if (n < ResamplerInfo::num())
			resamplerNo_ = n;
	}

	ResamplerInfo const & resampler() const { return ResamplerInfo::get(resamplerNo_); }

private:
	std::size_t resamplerNo_;
};


class AudioOut {
public:
	struct Status {
		long rate;
		bool low;

		Status(long rate, bool low) : rate(rate), low(low) {}
	};

	AudioOut(long sampleRate, int latency, int periods,
	         ResamplerInfo const &resamplerInfo, std::size_t maxInSamplesPerWrite)
	: resampler_(resamplerInfo.create(2097152, sampleRate, maxInSamplesPerWrite))
	, resampleBuf_(resampler_->maxOut(maxInSamplesPerWrite) * 2)
	, sink_(sampleRate, latency, periods)
	{
	}

	Status write(Uint32 const *data, std::size_t samples) {
		long const outsamples = resampler_->resample(
			resampleBuf_, reinterpret_cast<Sint16 const *>(data), samples);
		AudioSink::Status const &stat = sink_.write(resampleBuf_, outsamples);
		bool low = stat.fromUnderrun + outsamples < (stat.fromOverflow - outsamples) * 2;
		return Status(stat.rate, low);
	}

private:
	scoped_ptr<Resampler> const resampler_;
	Array<Sint16> const resampleBuf_;
	AudioSink sink_;
};

class FrameWait {
public:
	FrameWait() : last_() {}

	void waitForNextFrameTime(usec_t frametime) {
		last_ += asleep_.sleepUntil(last_, frametime);
		last_ += frametime;
	}

private:
	AdaptiveSleep asleep_;
	usec_t last_;
};

class GetInput : public InputGetter {
public:
	unsigned is;

	GetInput() : is(0) {}
	virtual unsigned operator()() { return is; }
};

class SdlIniter : Uncopyable {
public:
	SdlIniter()
	: failed_(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
	{
		if (failed_)
			std::fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	}

	~SdlIniter() {
		SDL_Quit();
	}

	bool isFailed() const { return failed_; }

private:
	bool const failed_;
};

class JsOpen : Uncopyable {
public:
	template<class InputIterator>
	JsOpen(InputIterator begin, InputIterator end) {
		for (InputIterator at = begin; at != end; ++at) {
			if (SDL_Joystick *j = SDL_JoystickOpen(*at))
				opened_.push_back(j);
		}
	}

	~JsOpen() { std::for_each(opened_.begin(), opened_.end(), SDL_JoystickClose); }

private:
	std::vector<SDL_Joystick *> opened_;
};

class GambatteSdl {
public:
	GambatteSdl() { gambatte.setInputGetter(&inputGetter); }
	int exec(int argc, char const *const argv[]);

private:
	GetInput inputGetter;
	GB gambatte;

	bool handleEvents(BlitterWrapper &blitter);
	int run(long sampleRate, int latency, int periods,
	        ResamplerInfo const &resamplerInfo, BlitterWrapper &blitter);
};


int GambatteSdl::exec(int const argc, char const *const argv[]) {
	std::puts("Gambatte PSP"
#ifdef GAMBATTE_SDL_VERSION_STR
	          " (" GAMBATTE_SDL_VERSION_STR ")"
#endif
	);

	LatencyOption latencyOption;
	PeriodsOption periodsOption;
	RateOption rateOption;
	ResamplerOption resamplerOption;
	ScaleOption scaleOption;
	VfOption vfOption;

	//Add a little menu to choose the rom file

	uint8_t gbaCgbOption 			= 0;
	uint8_t forceDmgOption 			= 0;
	uint8_t multicartCompatOption 	= 0;


	if (LoadRes const error =
			gambatte.load(rom_filename,
			                gbaCgbOption          * GB::GBA_CGB
			              + forceDmgOption        * GB::FORCE_DMG
			              + multicartCompatOption * GB::MULTICART_COMPAT)) {
		std::printf("failed to load ROM %s: %s\n", rom_filename , to_string(error).c_str());
		return EXIT_FAILURE;
	}

	SdlIniter sdlIniter;
	if (sdlIniter.isFailed())
		return EXIT_FAILURE;

	
	BlitterWrapper blitter(vfOption.filter(),
	                       scaleOption.scale(), true,
	                       false);
	
	SDL_WM_SetCaption("Gambatte PSP", 0);
	SDL_ShowCursor(SDL_DISABLE);

	latencyOption.exec(audio_latency);

	return run(rateOption.rate(), latencyOption.latency(), periodsOption.periods(),
	           resamplerOption.resampler(), blitter);
}

const u16 pspKEY[8] =
  { PSP_CTRL_CIRCLE,    //A
	PSP_CTRL_CROSS,     //B
	PSP_CTRL_SELECT,	//Select
	PSP_CTRL_START,		//Start
	PSP_CTRL_RIGHT,		//Right
	PSP_CTRL_LEFT,		//Left
	PSP_CTRL_UP,		//Up
	PSP_CTRL_DOWN,		//Down
  };

bool FastForward = false;

bool GambatteSdl::handleEvents(BlitterWrapper &blitter) {
	SceCtrlData pad;
    sceCtrlPeekBufferPositive(&pad, 1); 

	if(pad.Buttons & PSP_CTRL_HOME){
		sceKernelExitGame();
		return true;
	}

	if(pad.Buttons & PSP_CTRL_TRIANGLE)
	{
		FastForward = !FastForward;
		return false;
	}

	for(int i=0;i<8;i++) {
		if (pad.Buttons & pspKEY[i])
			inputGetter.is |= 1<<i;
		else
			inputGetter.is &= ~(1<<i);
    }

	return false;

	/*while (SDL_PollEvent(&e)) switch (e.type) {
	case SDL_JOYAXISMOTION:
		jd.dev_num = e.jaxis.which;
		jd.num = e.jaxis.axis;
		jd.dir = e.jaxis.value < -8192
		       ? JoyData::dir_down
		       : (e.jaxis.value > 8192 ? JoyData::dir_up : JoyData::dir_centered);

		for (std::pair<jmap_t::iterator, jmap_t::iterator> range =
				jaMap.equal_range(jd); range.first != range.second; ++range.first) {
			if (jd.dir == range.first->first.dir)
				inputGetter.is |= range.first->second;
			else
				inputGetter.is &= ~range.first->second;
		}

		break;
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		jd.dev_num = e.jbutton.which;
		jd.num = e.jbutton.button;

		for (std::pair<jmap_t::iterator, jmap_t::iterator> range =
				jbMap.equal_range(jd); range.first != range.second; ++range.first) {
			if (e.jbutton.state)
				inputGetter.is |= range.first->second;
			else
				inputGetter.is &= ~range.first->second;
		}

		break;
	case SDL_JOYHATMOTION:
		jd.dev_num = e.jhat.which;
		jd.num = e.jhat.hat;

		for (std::pair<jmap_t::iterator, jmap_t::iterator> range =
				jhMap.equal_range(jd); range.first != range.second; ++range.first) {
			if (e.jhat.value & range.first->first.dir)
				inputGetter.is |= range.first->second;
			else
				inputGetter.is &= ~range.first->second;
		}

		break;
	case SDL_KEYDOWN:
		if (e.key.keysym.mod & KMOD_CTRL) {
			switch (e.key.keysym.sym) {
			case SDLK_f: blitter.toggleFullScreen(); break;
			case SDLK_r: gambatte.reset(); break;
			default: break;
			}
		} else {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				return true;
			case SDLK_F5:
				gambatte.saveState(blitter.inBuf().pixels, blitter.inBuf().pitch);
				break;
			case SDLK_F6: gambatte.selectState(gambatte.currentState() - 1); break;
			case SDLK_F7: gambatte.selectState(gambatte.currentState() + 1); break;
			case SDLK_F8: gambatte.loadState(); break;
			case SDLK_0: gambatte.selectState(0); break;
			case SDLK_1: gambatte.selectState(1); break;
			case SDLK_2: gambatte.selectState(2); break;
			case SDLK_3: gambatte.selectState(3); break;
			case SDLK_4: gambatte.selectState(4); break;
			case SDLK_5: gambatte.selectState(5); break;
			case SDLK_6: gambatte.selectState(6); break;
			case SDLK_7: gambatte.selectState(7); break;
			case SDLK_8: gambatte.selectState(8); break;
			case SDLK_9: gambatte.selectState(9); break;
			default: break;
			}
		}
		// fallthrough
	case SDL_KEYUP:
		for (std::pair<keymap_t::iterator, keymap_t::iterator> range =
				keyMap.equal_range(e.key.keysym.sym);
				range.first != range.second; ++range.first) {
			if (e.key.state)
				inputGetter.is |= range.first->second;
			else
				inputGetter.is &= ~range.first->second;
		}

		break;
	case SDL_QUIT:
		return true;
	}

	return false;*/
}

static std::size_t const gb_samples_per_frame = 35112;
static std::size_t const gambatte_max_overproduction = 2064;

static bool isFastForward() {
	return FastForward;
}

int GambatteSdl::run(long const sampleRate, int const latency, int const periods,
                     ResamplerInfo const &resamplerInfo, BlitterWrapper &blitter) {
	Array<Uint32> const audioBuf(gb_samples_per_frame + gambatte_max_overproduction);
	AudioOut aout(sampleRate, latency, periods, resamplerInfo, audioBuf.size());
	FrameWait frameWait;
	SkipSched skipSched;
	std::size_t bufsamples = 0;
	bool audioOutBufLow = false;

	SDL_PauseAudio(0);

	uint8_t drawn_frames, fps = 0;

	uint64_t fps_timing = 0;
	uint64_t fps_previous_time = 0;

	uint64_t framelimiter_t = 0;
	uint64_t previous_framelimiter_t = 0;

	bool skipFrame = false;

	for (;;) {
		if (handleEvents(blitter))
			return 0;

		BlitterWrapper::Buf const &vbuf = blitter.inBuf();
		std::size_t runsamples = gb_samples_per_frame - bufsamples;
		std::ptrdiff_t const vidFrameDoneSampleCnt = gambatte.runFor(
			vbuf.pixels, vbuf.pitch, audioBuf + bufsamples, runsamples);
		std::size_t const outsamples = vidFrameDoneSampleCnt >= 0
		                             ? bufsamples + vidFrameDoneSampleCnt
		                             : bufsamples + runsamples;
		bufsamples += runsamples;
		bufsamples -= outsamples;
		
		pspDebugScreenSetXY(0,0);
		

		if (isFastForward()) {
			if (vidFrameDoneSampleCnt >= 0) {
				if (skipFrame){
					blitter.draw();
					blitter.present();
				}
				pspDebugScreenPrintf("FPS: %d  FF",fps);
			}
		} else {
			bool const blit = vidFrameDoneSampleCnt >= 0 && !skipSched.skipNext(audioOutBufLow);
			
			if (blit){
				blitter.draw();
				blitter.present();
			}

			pspDebugScreenPrintf("FPS: %d",fps);

			AudioOut::Status const &astatus = aout.write(audioBuf, outsamples);
			audioOutBufLow = astatus.low;
		}

		skipFrame = !skipFrame;

		
		std::memmove(audioBuf, audioBuf + outsamples, bufsamples * sizeof *audioBuf);

		++drawn_frames;
		sceRtcGetCurrentTick(&fps_timing);
		//Audio freeze the frame rate at 55 - 57 fps (full speed)
		//So in order to visualize 60 fps on screen we'll wait a bit longer
		//(Some people may complain about not reaching fullspeed so to make them happy this is perfect)
		if(fps_timing - fps_previous_time >= 1060000){
			fps_previous_time = fps_timing;
			fps = drawn_frames;
			drawn_frames = 0;
		}
	}

	return 0;
}

} // anon namespace


PSP_MODULE_INFO("Gambatte", 0, 1, 1);

typedef struct fname{
	char name[256];
}f_name;

typedef struct flist{
	f_name fname[256];
	int cnt;
}f_list;

static void ClearFileList(f_list * filelist){
	filelist->cnt =0;
}


static int HasExtension(char *filename){
	if(filename[strlen(filename)-4] == '.'){
		return 1;
	}
	return 0;
}


static void GetExtension(const char *srcfile,char *outext){
	if(HasExtension((char *)srcfile)){
		strcpy(outext,srcfile + strlen(srcfile) - 3);
	}else{
		strcpy(outext,"");
	}
}

enum {
	EXT_GBC = 1,
	EXT_GB = 2,
	EXT_GZ = 16,
	EXT_ZIP = 4,
	EXT_UNKNOWN = 8,
};

const struct {
	char *szExt;
	int nExtId;
} stExtentions[] = {
	{"gbc",EXT_GBC},
	{"gb",EXT_GB},
	{NULL, EXT_UNKNOWN}
};

static int getExtId(const char *szFilePath) {
	char *pszExt;

	if ((pszExt = strrchr(szFilePath, '.'))) {
		pszExt++;
		int i;
		for (i = 0; stExtentions[i].nExtId != EXT_UNKNOWN; i++) {
			if (!strcasecmp(stExtentions[i].szExt,pszExt)) {
				return stExtentions[i].nExtId;
			}
		}
	}

	return EXT_UNKNOWN;
}


static void GetFileList(f_list &filelist, const char *root) {
DIR *dir; struct dirent *ent;
if ((dir = opendir (root)) != NULL) {
  while ((ent = readdir (dir)) != NULL) {
    if(getExtId(ent->d_name)!= EXT_UNKNOWN){
				strcpy(filelist.fname[filelist.cnt].name,ent->d_name);
				filelist.cnt++;
				}
  }
  closedir (dir);
} else {
  /* could not open directory */
  return;
}
}

f_list filelist;

const uint16_t available_clocks[3] = {166, 222, 333};
uint16_t cpuClock = 2;

void ROM_Menu()
{
	SceCtrlData pad,oldPad;

	ClearFileList(&filelist);
 
	GetFileList(filelist, "ROMS");

	uint16_t selpos = 0;
	while(1){
		pspDebugScreenSetXY(0,0);
		pspDebugScreenPrintf("CPU CLOCK: %d",available_clocks[cpuClock%3]);
		pspDebugScreenSetXY(0,1);
		pspDebugScreenPrintf("Audio Latency: %d    ",audio_latency);
		pspDebugScreenSetXY(0,4);

		uint16_t i = 0;

		for (auto f : filelist.fname){

			if (i++ == selpos) pspDebugScreenSetTextColor(0x77FF88);
			else 			 pspDebugScreenSetTextColor(0xFFFFFF);

			pspDebugScreenPrintf("%s",f.name);
			pspDebugScreenSetXY(0,4 + i);
		}

		if(sceCtrlPeekBufferPositive(&pad, 1))
		{
			if (pad.Buttons != oldPad.Buttons)
			{
				if(pad.Buttons & PSP_CTRL_HOME){
			      sceKernelExitGame();
				}

				if(pad.Buttons & PSP_CTRL_CROSS)
				{
				 sprintf(rom_filename,"ROMS/%s\0",filelist.fname[selpos].name);
				 break;
				}
			
				if(pad.Buttons & PSP_CTRL_UP){
					if(--selpos <= 0)selpos=0;
				}
				if(pad.Buttons & PSP_CTRL_DOWN){
					if(++selpos >= filelist.cnt -1)selpos=filelist.cnt-1;
				}
				if(pad.Buttons & PSP_CTRL_RIGHT){
					cpuClock++;
				}
				if(pad.Buttons & PSP_CTRL_LEFT){
					cpuClock--;
				}

				if(pad.Buttons & PSP_CTRL_RTRIGGER){
					audio_latency+=16;
				}
				if(pad.Buttons & PSP_CTRL_LTRIGGER){
					audio_latency-=16;
				}

			}
			oldPad = pad;
		}
	}
}

int main(int argc, char **argv) {

	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);
	PSP_HEAP_SIZE_KB(-256);

	pspDebugScreenInitEx((void*)(0x44000000), PSP_DISPLAY_PIXEL_FORMAT_8888, 1);

	ROM_Menu();

	scePowerSetClockFrequency(available_clocks[cpuClock%3], available_clocks[cpuClock%3], 166);

	GambatteSdl gambatteSdl;
	return gambatteSdl.exec(argc,argv);
}
