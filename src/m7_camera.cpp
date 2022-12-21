#include "m7_camera.h"

#include "bn_size.h"
#include "bn_log.h"

#define INT_MAX 2147483647
#define INT_MIN -2147483648

M7Camera::M7Camera(const bn::affine_bg_ptr& bg, const bn::regular_bg_ptr& backdrop_, bool fog, int fog_intensity) : _floor(bg), _backdrop(backdrop_), _fog(false), _fog_intensity(fog_intensity)
{
	_pa_hbe = bn::affine_bg_pa_register_hbe_ptr::create(bg, _pa_values);
	_pc_hbe = bn::affine_bg_pc_register_hbe_ptr::create(bg, _pc_values);
	_dx_hbe = bn::affine_bg_dx_register_hbe_ptr::create(bg, _dx_values);
	_dy_hbe = bn::affine_bg_dy_register_hbe_ptr::create(bg, _dy_values);
	set_fog(fog);
	bn::window outside_window = bn::window::outside();
	outside_window.set_show_bg(*_backdrop, false);
	_internal_window = bn::rect_window::internal();
	_internal_window->set_boundaries(-80, -120, 80, 120);
	_internal_window->set_show_bg(*_backdrop, true);
}
void M7Camera::update()
{
	int xc = pos.x.data()>>4;
	int yc = pos.y.data() >> 4;
	int zc = pos.z.data()>>4;
	int cf = this->cf().data() >> 4;
	int sf = this->sf().data() >> 4;
	int ct= this->ct().data() >> 4;
	int st= this->st().data() >> 4;
	int horiz=-1;
	if(up().y != 0) // camera.ct = camup.y in camera coord
	{
		horiz = FAR_BG*backward().y.data() - pos.y.data();
		horiz = TOP - horiz*FOCAL / (FAR_BG*up().y.data());
	}
	else    // looking straight down ((camera.st = cambackward.y) > 0) means horizon at -inf scanline
	{
		horiz = backward().y.data() > 0 ? INT_MIN : INT_MAX;
	}
	for(int index = 0; index < bn::display::height(); ++index)
	{
		if(index<horiz)
		{
			// Ugly hack to be sure to have transparent color above horizon
			_pa_values[index]=0;
			_pc_values[index]=0;
			_dx_values[index]=0xFFFFFFFF;
			_dy_values[index]=0xFFFFFFFF;
			continue;
		}
		int yb= (index-TOP)*ct + FOCAL*st;
		int lam=0;
		if(yb != 0)
			lam=(yc<<12)/yb;     // .12f

		int lcf= lam*cf>>8;                 // .12f
		int lsf= lam*sf>>8;                 // .12f

		_pa_values[index]= int16_t(lcf>>4);              // .8f
		_pc_values[index]= int16_t(lsf>>4);              // .8f

		// lambda·Rx·b
		int zb= (index-TOP)*st - FOCAL*ct;    // .8f
		_dx_values[index]= xc + (lcf>>4)*LEFT - (lsf*zb>>12); // .8f
		_dy_values[index]= zc + (lsf>>4)*LEFT + (lcf*zb>>12); // .8f
		if(_fog)
		{
			auto fogval=lam*_fog_intensity>>8;
			if(fogval>256)
				fogval=256;
			if(fogval<0)
			{
				BN_LOG(index," ", lam, " ", horiz);
				fogval=0;
			}
			_blend_values[index].set_value(bn::fixed::from_data(fogval<<4));
		}
	}
	_backdrop->set_y(horiz-80-_backdrop->dimensions().height()/2);
	_backdrop->set_x(-phi());
	_internal_window->set_bottom(-80+(horiz > 0 && horiz < bn::display::height()+_backdrop->dimensions().height()?horiz:0));

	_pa_hbe->reload_values_ref();
	_pc_hbe->reload_values_ref();
	_dx_hbe->reload_values_ref();
	_dy_hbe->reload_values_ref();
	if(_fog)
	{
		_blend_hbe->reload_alphas_ref();
	}
}

void M7Camera::set_fog(bool fog)
{
	if(_fog && !fog)
	{
		_blend_hbe.reset();
		_floor->set_blending_enabled(false);
	}
	if(!_fog && fog)
	{
		_blend_hbe = bn::blending_fade_alpha_hbe_ptr::create(_blend_values);
		_floor->set_blending_enabled(true);
	}
	_fog = fog;
}
