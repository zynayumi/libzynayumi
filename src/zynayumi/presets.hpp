/****************************************************************************
    
    Presets for Zynayumi

    presets.hpp

    Copyleft (c) 2017 Nil Geisweiller
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef __ZYNAYUMI_PRESETS_HPP
#define __ZYNAYUMI_PRESETS_HPP

#include "patch.hpp"
// #include "parameters.hpp"
#include <vector>

namespace zynayumi {

/**
 * Class holding collection of patches (or user parameters ???).
 *
 * TODO: not clear this is redundant with Factory.
 */
class Presets {
public:
	Presets();

	std::vector<Patch> patches;
	// TODO: or vector of Parameters ???
	// std::vector<Parameters> presets;
};

} // ~namespace zynayumi

#endif
