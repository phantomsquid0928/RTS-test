#include "object.h"

object::object() {
	this->posx = 0;
	this->posy = 0;
}
object::object(int posx, int posy) {
	this->posx = posx;
	this->posy = posy;
}
void object::changepos(int posx, int posy) {
	this->posx = posx;
	this->posy = posy;
}
