#pragma once
#include "m7_location.h"

#include "bn_point.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_item.h"

class M7Camera;

class M7Sprite : public M7Location
{
	public:
	M7Sprite() = default;
	M7Sprite(const bn::sprite_item& item);
	void update(const M7Camera& cam);
	
	constexpr bool hidden() const
	{
		return _hidden || !sprite.has_value();
	}
	constexpr void set_hidden(bool hidden)
	{
		_hidden = hidden;
		if(_hidden && sprite.has_value())
		{
			sprite->set_visible(false);
		}
	}
	
	constexpr bool visible() const
	{
		return !_hidden && sprite.has_value() && sprite->visible();
	}
	
	
	bn::fixed sprite_scale=1;
	bool handleRotFrames=true;
	bn::point anchor;

	protected:
	void set_turn_frame(const M7Camera& cam, const Vector3& vc);
	bn::optional<bn::sprite_ptr> sprite;
	bn::optional<bn::sprite_item> sprite_item;
	int _last_graphic;
	bool _hidden;
};
