
#include "m7_sprite.h"
#include "m7_camera.h"
#include "bn_sprite_double_size_mode.h"
#include "bn_log.h"
class Rect
{
	public:
	int left, top;
	int right, bottom; 
};

M7Sprite::M7Sprite(const bn::sprite_item& item) : _sprite_item(item), _last_graphic(-1), _hidden(false)
{
	_sprite = item.create_sprite(0,0);
	_sprite->set_double_size_mode(bn::sprite_double_size_mode::ENABLED);
}
void M7Sprite::update(const M7Camera& cam)
{
	if(hidden())
		return;
	Vector3 vc;		// Inverted-cam vector
	int sx, sy;		// Object size
	Rect rect;		// Object rectangle 

	vc=cam.global_to_local(pos);

	sx= _sprite->shape_size().width();
	sy= _sprite->shape_size().height();

	// --- Check with viewbox ---
	do 
	{
		if(M7Camera::NEAR > vc.z || vc.z > M7Camera::FAR)
			break;	

		rect.left = (vc.x - anchor.x()*sprite_scale).integer();
		rect.right = rect.left + (sx*sprite_scale).integer()+1;
		if(M7Camera::LEFT*vc.z > rect.right*M7Camera::FOCAL || rect.left*M7Camera::FOCAL > M7Camera::RIGHT*vc.z)
			break;

		rect.top = (vc.y - anchor.y()*sprite_scale).integer();
		rect.bottom = rect.top + (sy*sprite_scale).integer()+1;
		if(-M7Camera::TOP*vc.z > rect.bottom*M7Camera::FOCAL || rect.top*M7Camera::FOCAL > -M7Camera::BOTTOM*vc.z)
			break;

		//_sprite->set_scale(1/bn::fixed::from_data(vc.z.data()>>M7Camera::FOCAL_SHIFT)*sprite_scale);
		_sprite->set_scale(1/bn::fixed::from_data(vc.norm().data()>>M7Camera::FOCAL_SHIFT)*sprite_scale);

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
		xscr  = (((vc.x.data()>>4) - (xscr*sprite_scale).integer())*(scale.data()))>>16;	// .0
		//xscr += -sx - M7_LEFT;
		
		
		yscr  = (anchor.y()*256 - sy*128);				// .8
		yscr  = (((vc.y.data()>>4) - (yscr*sprite_scale).integer())*(scale.data()))>>16;	// .0
		//yscr += -sy + M7_TOP;

		_sprite->set_visible(true);
		_sprite->set_x(xscr);
		_sprite->set_y(yscr);

		if(handleRotFrames)
		{
			if(turnFramesMode == HALF)
				set_turn_frame_half(cam, vc);
			else
				set_turn_frame_full(cam, vc);
		}
		_sprite->set_z_order(vc.z.integer());
		return;
	}
	while(0);

	_sprite->set_visible(false);

	// TODO: add shadow
}


// For N frames provided, divide the half circle in N-2 equal sections (of angle PI/(N-1) => 1024/(N-1))
//    and 2 half sized sections (of angle PI/(2*(N-1)) => 1024/(2*(N-1)))
//
// Example if N=3 images are provided for 180°
//          0
//    1792____256
//       /\ | /\                       //
//      |  \|/  |
// 1536 |   X   | 512
//      |  /|\  |
//       \/ | \ /
//    1280-----768
//        1024
// [0   - 256[: image indice=0, no mirror (angle section is 1024/(2*(N-1))=256)
// [256 - 768[: image indice=1, no mirror (angle section is 1024/(N-1)=512)
// [768 -1024[: image indice=2, no mirror (angle section is 1024/(2*(N-1))=256)
// [1024-1280[: image indice=2, mirror?
// [1280-1792[: image indice=1, mirror
// [1792-2048[: image indice=0, mirror?
//
//
// Example if N=4 images are provided for 180°
//          0
//    1877____171
//       /\ | /\                       //
//      |  \|/  |
// 1536 |---X---| 512
//      |  /|\  |
//       \/ | \ /
//    1195-----853
//        1024
// [0   - 171[: image indice=0, no mirror (angle section is 1024/(2*(N-1))~=171)
// [171 - 512[: image indice=1, no mirror (angle section is 1024/(N-1)~=341)
// [512 - 853[: image indice=2, no mirror (angle section is 1024/(N-1)~=341)
// [853 -1024[: image indice=3, no mirror (angle section is 1024/(2*(N-1))~=171)
// [1024-1195[: image indice=3, mirror?
// [1280-1536[: image indice=2, mirror
// [1536-1877[: image indice=1, mirror
// [1877-2048[: image indice=0, mirror?
void M7Sprite::set_turn_frame_half(const M7Camera& cam, const Vector3& vc)
{
	int angle = cam.phi() - phi();
	auto diffAngle=(bn::atan2(vc.z.integer(), vc.x.integer())-0.25)*2048;
	angle-=diffAngle.integer();
	if(angle < -1024)
		angle+=2048;
	if(angle >= 1024)
	{
		angle-=2048;
	}
	bool mirror=false;
	if(angle<0)
	{
		mirror=true;
		angle=-angle;
	}
	
	_sprite->set_horizontal_flip(mirror);

	int nbPos=_sprite_item->tiles_item().graphics_count();
#ifdef SIMPLE_TURN_FRAME
	int index=angle*nbPos/1025;
#else
	int section=1024/(nbPos-1);
	int halfSection=section>>1;
	int index=0;
	if(angle > halfSection)
	{
		index = 1+(angle-halfSection)/section;
	}
#endif
	BN_LOG("half angle ", angle," index", index);
	if(_last_graphic != index)
	{
		_last_graphic=index;
		_sprite->set_item(*_sprite_item, index);
	}
}
void M7Sprite::set_turn_frame_full(const M7Camera& cam, const Vector3& vc)
{
	int angle = cam.phi() - phi();
	auto diffAngle=(bn::atan2(vc.z.integer(), vc.x.integer())-0.25)*2048;
	angle-=diffAngle.integer();
	angle=normalize_angle(angle);
	int nbPos=_sprite_item->tiles_item().graphics_count();
#ifdef SIMPLE_TURN_FRAME
	int index=angle*nbPos/2048;
#else
	int section=2048/nbPos;
	int halfSection=section>>1;
	int index=0;
	if(angle > halfSection)
	{
		index = 1+(angle-halfSection)/section;
	}
	index%=nbPos;
#endif
	BN_LOG("full angle ", angle," index", index);
	if(_last_graphic != index)
	{
		_last_graphic=index;
		_sprite->set_item(*_sprite_item, index);
	}
}