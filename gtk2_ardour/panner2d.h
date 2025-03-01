/*
 * Copyright (C) 2005-2014 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2005 Taybin Rutkin <taybin@taybin.com>
 * Copyright (C) 2009-2011 Carl Hetherington <carl@carlh.net>
 * Copyright (C) 2009-2011 David Robillard <d@drobilla.net>
 * Copyright (C) 2014-2015 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __ardour_panner_2d_h__
#define __ardour_panner_2d_h__

#include <sys/types.h>
#include <map>
#include <vector>

#include <glibmm/refptr.h>
#include <ytkmm/drawingarea.h>
#include <ytkmm/window.h>
#include <ytkmm/box.h>
#include <ytkmm/button.h>
#include <ytkmm/spinbutton.h>
#include <ytkmm/adjustment.h>

#include "pbd/cartesian.h"

#include "ardour_window.h"

namespace ARDOUR {
	class PannerShell;
}

namespace Gtk {
	class Menu;
	class CheckMenuItem;
}

class Panner2dWindow;

class Panner2d : public Gtk::DrawingArea
{
	public:
	Panner2d (std::shared_ptr<ARDOUR::PannerShell>, int32_t height);
	~Panner2d ();

	void allow_target_motion (bool);

	int  add_speaker (const PBD::AngularVector&);
	int  add_signal (const char* text, const PBD::AngularVector&);
	void move_signal (int which, const PBD::AngularVector&);
	void reset (uint32_t n_inputs);
	void set_send_drawing_mode (bool);

	std::shared_ptr<ARDOUR::PannerShell> get_panner_shell() const { return panner_shell; }

	void cart_to_gtk (PBD::CartesianVector&) const;
	void gtk_to_cart (PBD::CartesianVector&) const;

	protected:
	bool on_expose_event (GdkEventExpose *);
	bool on_button_press_event (GdkEventButton *);
	bool on_button_release_event (GdkEventButton *);
	bool on_motion_notify_event (GdkEventMotion *);
	bool on_scroll_event (GdkEventScroll *);
	void on_size_allocate (Gtk::Allocation& alloc);

	private:
	class Target {
		public:
		PBD::AngularVector position;
		bool visible;
		std::string text;

		Target (const PBD::AngularVector&, const char* txt = 0);
		~Target ();

		void set_text (const char*);
		void set_selected (bool yn) {
			_selected = yn;
		}
		bool selected() const {
			return _selected;
		}

		private:
		bool _selected;
	};

	struct ColorScheme {
		uint32_t background;
		uint32_t crosshairs;
		uint32_t signalcircle_border;
		uint32_t signalcircle;
		uint32_t diffusion;
		uint32_t diffusion_inv;
		uint32_t pos_outline;
		uint32_t pos_fill;
		uint32_t signal_outline;
		uint32_t signal_fill;
		uint32_t speaker_fill;
		uint32_t text;
		uint32_t send_bg;
		uint32_t send_pan;
	};

	static ColorScheme colors;
	static void set_colors ();
	static bool have_colors;
	void color_handler ();

	std::shared_ptr<ARDOUR::PannerShell> panner_shell;
	Glib::RefPtr<Pango::Layout> layout;

	typedef std::vector<Target*> Targets;
	Targets speakers;
	Targets signals;
	Target  position;

	Target *drag_target;
	int     width;
	int     height;
	double  radius;
	double  border;
	double  hoffset;
	double  voffset;
	double  last_width;
	bool    did_move;
	bool    have_elevation;
	bool    _send_mode;

	Target *find_closest_object (gdouble x, gdouble y, bool& is_signal);

	gint handle_motion (gint, gint, GdkModifierType);

	void toggle_bypass ();
	void handle_state_change ();
	void handle_position_change ();
	void label_signals ();

	PBD::ScopedConnectionList panshell_connections;
	PBD::ScopedConnectionList panner_connections;

	/* cartesian coordinates in GTK units ; adjust to same but on a circle of radius 1.0
	   and centered in the middle of our area
	*/
	void clamp_to_circle (double& x, double& y);
	void sphere_project (double& x, double& y, double& z);
};

class Panner2dWindow : public ArdourWindow
{
	public:
	Panner2dWindow (std::shared_ptr<ARDOUR::PannerShell>, int32_t height, uint32_t inputs);

	void reset (uint32_t n_inputs);

	private:
	Panner2d widget;

	Gtk::HBox         hpacker;
	Gtk::VBox         button_box;
	Gtk::ToggleButton bypass_button;
	Gtk::VBox         spinner_box;
	Gtk::VBox         left_side;

	Gtk::Adjustment   width_adjustment;
	Gtk::SpinButton   width_spinner;

	PBD::ScopedConnectionList panshell_connections;
	PBD::ScopedConnectionList panvalue_connections;
	void set_bypassed();
	void set_width();

	void pannable_handler ();
	void bypass_toggled ();
	void width_changed ();
	bool on_key_press_event (GdkEventKey*);
	bool on_key_release_event (GdkEventKey*);
};

#endif /* __ardour_panner_2d_h__ */
