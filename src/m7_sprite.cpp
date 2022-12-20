
#include "m7_sprite.h"
#include "m7_camera.h"
#include "bn_sprite_double_size_mode.h"

class Rect
{
	public:
	int left, top;
	int right, bottom; 
};
// TODO: remove this (it's just a scaling factor (1/4 is the same as >>2), it can be integrated to sprite_scale
static const int M7O_NORM=2;

M7Sprite::M7Sprite(const bn::sprite_item& item)
{
	sprite_item=item;
	sprite = item.create_sprite(0,0);
	sprite->set_double_size_mode(bn::sprite_double_size_mode::ENABLED);
}
void M7Sprite::update(const M7Camera& cam)
{
	if(hidden())
		return;
	Vector3 vc;		// Inverted-cam vector
	int sx, sy;		// Object size
	Rect rect;		// Object rectangle 

	vc=cam.global_to_local(pos);

	sx= sprite->shape_size().width();
	sy= sprite->shape_size().height();

	// --- Check with viewbox ---
	do 
	{
		if(M7Camera::NEAR > vc.z || vc.z > M7Camera::FAR)
			break;	

		rect.left= (vc.x - bn::fixed::from_data(anchor.x()<<(12-M7O_NORM))).integer();
		rect.right= rect.left + (sx>>M7O_NORM);
		if(M7Camera::LEFT*vc.z > rect.right*M7Camera::FOCAL || rect.left*M7Camera::FOCAL > M7Camera::RIGHT*vc.z)
			break;

		rect.top= (vc.y - bn::fixed::from_data(anchor.y()<<(12-M7O_NORM))).integer();
		rect.bottom= rect.top + (sy>>M7O_NORM);
		if(-M7Camera::TOP*vc.z > rect.bottom*M7Camera::FOCAL || rect.top*M7Camera::FOCAL > -M7Camera::BOTTOM*vc.z)
			break;

		sprite->set_scale(1/bn::fixed::from_data(vc.z.data()>>(M7Camera::FOCAL_SHIFT-M7O_NORM))*sprite_scale);

		bn::fixed_t<8> scale=1;
		int vcz8=vc.z.data()>>4;
		if(vcz8 != 0)
			scale= bn::fixed_t<8>::from_data((M7Camera::FOCAL<<16)/vcz8);	// (.16 / .8) = .8 

		// Base anchoring equation:
		// x = q0 - s - A(p0 - s/2)
		// In this case A = 1/lam; and q0 = xc/lam
		// -> x = (xc - p0 + s/2)/lam - s + screen/2
		int xscr, yscr;
		xscr  = (anchor.x()*256 - sx*128);				// .8
		xscr  = (((vc.x.data()>>4) - ((xscr>>M7O_NORM)*sprite_scale).integer())*(scale.data()))>>16;	// .0
		//xscr += -sx - M7_LEFT;
		
		
		yscr  = (anchor.y()*256 - sy*128);				// .8
		yscr  = (((vc.y.data()>>4) - ((yscr>>M7O_NORM)*sprite_scale).integer())*(scale.data()))>>16;	// .0
		//yscr += -sy + M7_TOP;

		sprite->set_visible(true);
		sprite->set_x(xscr);
		sprite->set_y(yscr);

		if(handleRotFrames)
		{
			set_turn_frame(cam, vc);
		}
		sprite->set_z_order(vc.z.integer());
		return;
	}
	while(0);

	sprite->set_visible(false);

	// TODO: add shadow
}

void M7Sprite::set_turn_frame(const M7Camera& cam, const Vector3& vc)
{
	int angle = cam.phi() - phi();
	if(angle < -1024)
		angle+=2048;
	if(angle > 1024)
	{
		angle-=2048;
	}
	auto diffAngle=(bn::atan2(vc.z.integer(), vc.x.integer())-0.25)*2048;
	angle-=diffAngle.integer();
	bool mirror=false;
	if(angle<0)
	{
		mirror=true;
		angle=-angle;
	}
	// Ugly quick fix... TODO: a real fix
	if(angle > 1024)
	{
		angle=1024-(angle-1024);
	}
	sprite->set_horizontal_flip(mirror);

	int nbPos=sprite_item->tiles_item().graphics_count();
	
	int index=angle*nbPos/1024;
	index%=nbPos; // by security
	if(_last_graphic != index)
	{
		_last_graphic=index;
		sprite->set_item(*sprite_item, index);
	}
}
