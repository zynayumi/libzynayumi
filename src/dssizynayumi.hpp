/****************************************************************************
    
    DSSI wrapper for Zynayumi

    dssizynayumi.hpp

    Copyleft (c) 2016-2019 Nil Geisweiller <ngeiswei@gmail.com>
 
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

#ifndef __ZYNAYUMI_DSSIZYNAYUMI_HPP
#define __ZYNAYUMI_DSSIZYNAYUMI_HPP

#include "dssi/dssiplugin.hpp"
#include "zynayumi/zynayumi.hpp"
#include "parameters.hpp"

namespace zynayumi {

static enum {
	// Audio outputs
	LEFT_OUTPUT=PARAMETERS_COUNT,
	RIGHT_OUTPUT
} enum_ports;

class DSSIZynayumi : public DSSIPlugin {
public:

	Zynayumi zynayumi;

	DSSIZynayumi(unsigned long frame_rate);

	void run_synth(unsigned long sample_count, snd_seq_event_t* events,
	               unsigned long event_count);
	int get_midi_controller_for_port(unsigned long port);

	const DSSI_Program_Descriptor* get_program(unsigned long index);
	void select_program(unsigned long bank, unsigned long program);

	char* configure(const char* key, const char* value);

protected:

	unsigned long m_frame_rate;
	unsigned long m_last_frame;

	LADSPA_Data m_velocity;
	LADSPA_Data m_pitch;
	unsigned char m_note;
	bool m_note_is_on;

	DSSI_Program_Descriptor m_prog;
	std::vector<DSSI_Program_Descriptor> m_progs;

	LADSPA_Data m_pitchbend;

	// Read the control ports and update the patch accordingly
	void update_patch();
};

} // ~namespace zynayumi

#endif
