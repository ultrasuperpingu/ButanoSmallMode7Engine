#pragma once


#include "m7_location.h"
#include "bn_display.h"
#include "bn_affine_bg_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_affine_bg_pa_register_hbe_ptr.h"
#include "bn_affine_bg_pc_register_hbe_ptr.h"
#include "bn_affine_bg_dx_register_hbe_ptr.h"
#include "bn_affine_bg_dy_register_hbe_ptr.h"
#include "bn_blending_fade_alpha_hbe_ptr.h"
#include "bn_blending_fade_alpha.h"
#include "bn_rect_window.h"


class M7Camera : public M7Location
{
	public:
	static const int FOCAL=256;		//!< Focal length
	static const int FOCAL_SHIFT=8;
	static const int NORM_SHIFT=2;		//!< Object renormalization shift (by /4)

	// View frustrum limits
	static const int LEFT	=	(-120);		//!< Viewport left
	static const int RIGHT	=	120;		//!< Viewport right
	static const int TOP	=	80;			//!< Viewport top (y-axis up)
	static const int BOTTOM	=	(-80);		//!< Viewport bottom (y-axis up!)
	static const int NEAR	=	24;			//!< Near plane (objects)
	static const int FAR	=	512;		//!< Far plane (objects)

	static const int FAR_BG=	768;		//!< Far plane (floor)

	M7Camera(const bn::affine_bg_ptr& bg, const bn::regular_bg_ptr& backdrop_, bool fog=false, int fog_intensity=2);
	void update();
	constexpr bool fog() const
	{
		return _fog;
	}
	void set_fog(bool fog);

	constexpr int fog_intensity() const
	{
		return _fog_intensity;
	}
	constexpr void set_fog_intensity(int intensity)
	{
		BN_ASSERT(intensity >= 0);
		_fog_intensity=intensity;
	}
	inline void set_visible(bool v)
	{
		_floor->set_visible(v);
		_backdrop->set_visible(v);
	}
	inline void destroy()
	{
		_internal_window->restore_boundaries();
		_internal_window->set_show_nothing();
		bn::window outside_window = bn::window::outside();
		outside_window.set_show_all();
		_pa_hbe.reset();
		_pc_hbe.reset();
		_dx_hbe.reset();
		_dy_hbe.reset();
		_blend_hbe.reset();
		_internal_window.reset();
		_floor.reset();
		_backdrop.reset();
	}
	protected:
	int16_t _pa_values[bn::display::height()];
	bn::optional<bn::affine_bg_pa_register_hbe_ptr> _pa_hbe;

	int16_t _pc_values[bn::display::height()];
	bn::optional<bn::affine_bg_pc_register_hbe_ptr> _pc_hbe;

	int _dx_values[bn::display::height()];
	bn::optional<bn::affine_bg_dx_register_hbe_ptr> _dx_hbe;

	int _dy_values[bn::display::height()];
	bn::optional<bn::affine_bg_dy_register_hbe_ptr> _dy_hbe;
	
	bn::blending_fade_alpha _blend_values[bn::display::height()];
	bn::optional<bn::blending_fade_alpha_hbe_ptr> _blend_hbe;
	
	
	bn::optional<bn::affine_bg_ptr> _floor;
	bn::optional<bn::regular_bg_ptr> _backdrop;
	bn::optional<bn::rect_window> _internal_window;
	bool _fog;
	int _fog_intensity;
	
};
