#pragma once
#include "m7_location.h"

#include "bn_point.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_item.h"

class M7Camera;

enum TurnFramesMode
{
	HALF,
	FULL,
	//TODO: LUT
};

class M7Sprite : public M7Location
{
	public:
	M7Sprite() = default;
	M7Sprite(const bn::sprite_item& item);
	void update(const M7Camera& cam);
	
	constexpr bool hidden() const
	{
		return _hidden || !_sprite.has_value();
	}
	constexpr void set_hidden(bool hidden)
	{
		_hidden = hidden;
		if(_hidden && _sprite.has_value())
		{
			_sprite->set_visible(false);
		}
	}
	
	constexpr bool visible() const
	{
		return !_hidden && _sprite.has_value() && _sprite->visible();
	}
	
	constexpr const bn::optional<bn::sprite_ptr>& sprite() const
	{
		return _sprite;
	}
	bn::fixed sprite_scale=0.25;
	bn::point anchor;
	bool handleRotFrames=true;
	TurnFramesMode turnFramesMode = HALF;

	protected:
	void set_turn_frame_half(const M7Camera& cam, const Vector3& vc);
	void set_turn_frame_full(const M7Camera& cam, const Vector3& vc);
	bn::optional<bn::sprite_ptr> _sprite;
	bn::optional<bn::sprite_item> _sprite_item;
	int _last_graphic;
	bool _hidden;
};
