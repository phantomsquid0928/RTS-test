#include "object.h"

object::object() {
	this->position = vec2(0, 0);
}
object::object(int posx, int posy) {
	this->position = vec2(posx, posy);
}
void object::changepos(vec2 newpos) { //?
	this->position.x = newpos.x;
	this->position.y = newpos.y;
}
