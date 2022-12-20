
#include "m7_sprite.h"
#include "m7_camera.h"

#include "bn_core.h"
#include "bn_keypad.h"

#include "bn_affine_bg_items_land.h"
#include "bn_regular_bg_items_backdrop.h"
#include "bn_sprite_items_cow.h"


void update_camera(M7Camera& camera)
{
	bn::fixed dir_x=0;
	bn::fixed dir_z=0;
	int phi = 0;
	int theta = 0;
	phi = camera.phi();
	theta = camera.theta();
	if(bn::keypad::down_held())
	{
		dir_z += 2;
	}
	else if(bn::keypad::up_held())
	{
		dir_z -= 2;
	}
	if(bn::keypad::left_held())
	{
		dir_x -= 2;
	}
	else if(bn::keypad::right_held())
	{
		dir_x += 2;
	}

	if(bn::keypad::b_held())
	{
		camera.pos.y -= bn::fixed::from_data(2048);

		if(camera.pos.y < 0)
		{
			camera.pos.y = 0;
		}
	}
	else if(bn::keypad::a_held())
	{
		camera.pos.y += bn::fixed::from_data(2048);
	}

	if(!bn::keypad::select_held() && bn::keypad::l_held())
	{
		phi -= 4;

		if(phi < 0)
		{
			phi += 2048;
		}
	}
	else if(!bn::keypad::select_held() && bn::keypad::r_held())
	{
		phi += 4;

		if(phi >= 2048)
		{
			phi -= 2048;
		}
	}
	if(bn::keypad::select_held() && bn::keypad::r_held())
	{
		theta -= 4;

		if(theta < 0)
		{
			theta += 2048;
		}
		if(theta > 512 && theta < 1536)
			theta = 1536;
	}
	else if(bn::keypad::select_held() && bn::keypad::l_held())
	{
		theta += 4;

		if(theta > 2048)
		{
			theta -= 2048;
		}
		if(theta > 512 && theta < 1536)
			theta = 512;
	}
	camera.translate_floor_plane(dir_x,dir_z);
	camera.set_orientation(phi, theta);
}

int main()
{
	bn::core::init();
	
	bn::regular_bg_ptr backdrop = bn::regular_bg_items::backdrop.create_bg(0, 0);
	//backdrop.set_priority(3);
	bn::affine_bg_ptr floor = bn::affine_bg_items::land.create_bg(0, 0);
	
	
	M7Sprite cowSpr(bn::sprite_items::cow);
	cowSpr.pos=Vector3(465,0,200);
	cowSpr.anchor.set_x(32);
	cowSpr.anchor.set_y(52);
	cowSpr.set_phi(1024);
	cowSpr.handleRotFrames=true;
	cowSpr.sprite_scale=1.8;

	M7Camera camera(floor, backdrop, true);
	camera.pos=Vector3(460,50,286);
	//camera.set_orientation(5, 140);
	camera.look_at(cowSpr.pos+Vector3(0,2,0));
	
	
	while(true)
	{
		update_camera(camera);
		if(bn::keypad::start_pressed())
			camera.look_at(cowSpr.pos+Vector3(0,2,0));
		camera.update();
		cowSpr.update(camera);
		bn::core::update();
	}
}
