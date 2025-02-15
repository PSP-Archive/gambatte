/***************************************************************************
 *   Copyright (C) 2008-2009 by Sindre Aamås                               *
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
#include <common/kaiser50sinc.h>
#include <common/i0.h>
#include <cmath>

double kaiser50SincWin(long const n, long const M) {
	double const beta = 4.62;
	static double const i0beta_rec = 1.0 / i0(beta);

	double x = static_cast<double>(n * 2) / M - 1.0;
	x = x * x;
	x = beta * std::sqrt(1.0 - x);

	return i0(x) * i0beta_rec;
}
