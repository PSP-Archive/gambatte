//
//   Copyright (C) 2008 by sinamas <sinamas at users.sourceforge.net>
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

#ifndef STATE_OSD_ELEMENTS_H
#define STATE_OSD_ELEMENTS_H

#include <gambatte/osd_element.h>
#include <common/transfer_ptr.h>

#include <string>

namespace gambatte {
transfer_ptr<OsdElement> newStateLoadedOsdElement(unsigned stateNo);
transfer_ptr<OsdElement> newStateSavedOsdElement(unsigned stateNo);
transfer_ptr<OsdElement> newSaveStateOsdElement(const std::string &fileName, unsigned stateNo);
}

#endif
