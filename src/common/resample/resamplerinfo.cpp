/***************************************************************************
 *   Copyright (C) 2008 by Sindre Aamås                                    *
 *   sinamas@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 as     *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License version 2 for more details.                *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   version 2 along with this program; if not, write to the               *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/
#include <common/resample/resamplerinfo.h>
#include <common/chainresampler.h>
#include <common/kaiser50sinc.h>
#include <common/kaiser70sinc.h>
#include <common/rectsinc.h>
#include <common/linint.h>

static Resampler * createLinint(long inRate, long outRate, std::size_t ) {
	return new Linint<ResamplerInfo::channels>(inRate, outRate);
}

ResamplerInfo const ResamplerInfo::resamplers_[] = {
	{ "Fast", createLinint },
	{ "High quality (polyphase FIR)", ChainResampler::create<RectSinc> },
// 	{ "Hamming windowed sinc (~50 dB SNR)", ChainResampler::create<HammingSinc> },
// 	{ "Blackman windowed sinc (~70 dB SNR)", ChainResampler::create<BlackmanSinc> },
	{ "Very high quality (polyphase FIR)", ChainResampler::create<Kaiser50Sinc> },
	{ "Highest quality (polyphase FIR)", ChainResampler::create<Kaiser70Sinc> },
};

std::size_t const ResamplerInfo::num_ =
	sizeof ResamplerInfo::resamplers_ / sizeof *ResamplerInfo::resamplers_;
