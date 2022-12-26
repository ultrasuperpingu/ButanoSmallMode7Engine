
#include "m7_sprite.h"
#include "m7_camera.h"

#include "bn_core.h"
#include "bn_keypad.h"

#include "bn_affine_bg_items_land.h"
#include "bn_regular_bg_items_backdrop.h"
#include "bn_sprite_items_cow_12_frames.h"
#include "bn_sprite_items_cow_5_frames.h"
#include "bn_sprite_items_cube_360.h"


void update_camera(M7Camera& camera)
{
	bn::fixed dir_x=0;
	bn::fixed dir_z=0;
	int phi = 0;
	int theta = 0;
	phi = camera.phi();
	theta = camera.theta();
	if(!bn::keypad::l_held() && bn::keypad::down_held())
	{
		dir_z += 2;
	}
	else if(!bn::keypad::l_held() && bn::keypad::up_held())
	{
		dir_z -= 2;
	}
	if(!bn::keypad::l_held() && bn::keypad::left_held())
	{
		dir_x -= 2;
	}
	else if(!bn::keypad::l_held() && bn::keypad::right_held())
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

	if(bn::keypad::l_held() && bn::keypad::left_held())
	{
		phi -= 4;

		if(phi < 0)
		{
			phi += 2048;
		}
	}
	else if(bn::keypad::l_held() &&  bn::keypad::right_held())
	{
		phi += 4;

		if(phi >= 2048)
		{
			phi -= 2048;
		}
	}
	if(bn::keypad::l_held() &&  bn::keypad::up_held())
	{
		theta -= 4;

		if(theta < 0)
		{
			theta += 2048;
		}
		if(theta > 512 && theta < 1536)
			theta = 1536;
	}
	else if(bn::keypad::l_held() && bn::keypad::down_held())
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

M7Sprite* findClosestSprite(M7Sprite* sprites, int count)
{
	bn::fixed closestDist = 10000;
	M7Sprite* closest = nullptr;
	for(int i=0;i<count;i++)
	{
		if(sprites[i].visible())
		{
			bn::fixed squareDist=sprites[i].sprite()->x()*sprites[i].sprite()->x()+sprites[i].sprite()->y()*sprites[i].sprite()->y();
			if(closestDist > squareDist)
			{
				closestDist = squareDist;
				closest = &(sprites[i]);
			}
		}
	}
	return closest;
}

int main()
{
	bn::core::init();
	
	bn::regular_bg_ptr backdrop = bn::regular_bg_items::backdrop.create_bg(0, 0);
	bn::affine_bg_ptr floor = bn::affine_bg_items::land.create_bg(0, 0);


	M7Sprite cowSpr(bn::sprite_items::cow_12_frames);
	cowSpr.pos=Vector3(450,0,200);
	cowSpr.anchor.set_x(32);
	cowSpr.anchor.set_y(52);
	cowSpr.set_phi(1024);
	cowSpr.handleRotFrames=true;
	cowSpr.sprite_scale=0.45;

	M7Sprite cow5Spr(bn::sprite_items::cow_5_frames);
	cow5Spr.pos=Vector3(490,0,200);
	cow5Spr.anchor.set_x(32);
	cow5Spr.anchor.set_y(52);
	cow5Spr.set_phi(1024);
	cow5Spr.handleRotFrames=true;
	cow5Spr.sprite_scale=0.45;

	M7Sprite cube(bn::sprite_items::cube_360);
	cube.pos=Vector3(470,0,130);
	cube.anchor.set_x(32);
	cube.anchor.set_y(45);
	cube.set_phi(1024);
	cube.handleRotFrames=true;
	cube.sprite_scale=0.45;
	cube.turnFramesMode=FULL;

	int spritesCount=3;
	M7Sprite sprites[]={cowSpr, cow5Spr, cube};

	M7Camera camera(floor, backdrop, true);
	camera.set_fog_intensity(4);
	camera.pos=Vector3(450,50,286);
	camera.look_at(cowSpr.pos+Vector3(0,2,0));

	M7Sprite *closest = nullptr;
	while(true)
	{
		update_camera(camera);
		if(bn::keypad::r_pressed()) // lock closest sprite
			closest = findClosestSprite(sprites, spritesCount);
		if(closest && bn::keypad::r_held())
			camera.look_at(closest->pos+Vector3(0,2,0));

		camera.update();
		for(int i=0;i<spritesCount;i++)
			sprites[i].update(camera);

		bn::core::update();
	}
}
