#include "entity.h"
#include "headers.hpp"
#include "path.h"

using namespace input;
using namespace mapinfo;
//이등변삼각형 예각부분 dir
//latest version of rendering objects
inline bool isvalid(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return x >= 0 && y >= 0 && x < sizex&& y < sizey;
}
inline bool iswall(int x, int y, const vector<vector<int>>* map) {
	return !isvalid(x, y, map) || ((*map)[x][y] == 1 || (*map)[x][y] == 2);
}
inline bool ispassible(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return isvalid(x, y, map) && ((*map)[x][y] != 1 && (*map)[x][y] != 2);
}

entity::entity(const vector<array<float, 3>>& location, flowfield* f) {
	vcode = readFile("shader/entity_vertex.glsl"); //mod
	fcode = readFile("shader/entity_fragment.glsl");
	
	/// <summary>
	/// test
	/// </summary>
	/// <param name="location"></param>

	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());

	cout << "entity shader : " << shaderProgram << endl;

	size = 10;

	this->location = &location;
	this->f = f;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}
entity::~entity() {
}
void entity::create() { //TODO : refactoring of vertices, indices needed
	vertices.clear();
	indices.clear();
	paths.resize(location->size(), nullptr);   //같은 entity들의 path들
	curpathidx.resize(location->size(), -1);
	rotations.resize(location->size(), 0.0); //같은 entity 갯수
	velocity.resize(location->size(), vec2(0, 0));
	accel.resize(location->size(), vec2(0, 0));
	reached.reset();
	
	if (mode_status[trace]) {
		poopaths.resize(location->size(), nullptr);
		for (int i = 0; i < location->size(); i++) {
			vector<vec2>* emptytrail = new vector<vec2>();
			poopaths[i] = emptytrail;
		}
	}
	int i = 0;
	for (auto p : *location) {  //삼각형으로 바꿈.
		
		float x1 = 0.0f;
		float x2 = 2.0f * size;
		float y1 = 0.0f;
		float y2 = (float)size / 2;

		vertices.insert(vertices.end(), {
			x2, y1, 0.0f,    // 2, 0
			x1, y2, 0.0f,    //0, 1
			x1, -y2, 0.0f    //0, -1
			});

		unsigned int base = i * 3;
		indices.insert(indices.end(), {
			base, base + 1, base + 2,
			});
		i++;
	}
	//generate buffers

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void entity::render() { //TODO: need to assume shaderProgram called.
	//glUseProgram(shaderProgram);	
	//glDrawArrays(GL_LINE_LOOP, 0, 4);
	
	glUseProgram(shaderProgram);
	
	for (int i = 0; i < rotations.size(); i++) {
		
		//TODO: 이거 이용해서로 수정하기
		
		mat4 model = mat4(1.0f);
		mat4 project = mat4(1.0f);

		//model = translate(model, vec3(2 * entitylist[i][0] / sizex - 1, 1 - 2 * entitylist[i][1] / sizey, 0));
		model = translate(model, vec3(entitylist[i][1], entitylist[i][0], 0));
		model = rotate(model, rotations[i], vec3(0.0f, 0.0f, 1.0f));
		project = ortho(0.0f, (float)sizey, (float)sizex, 0.0f, -1.0f, 1.0f);

		GLuint colorizer = glGetUniformLocation(shaderProgram, "color");
		if (selected[i]) {
			glUniform4f(colorizer, 0.0f, 1.0f, 0.0f, 1.0f);
		}
		else {
			glUniform4f(colorizer, 0.0f, 0.0f, 1.0f, 1.0f);
		}

		GLuint modelrotation = glGetUniformLocation(shaderProgram, "model");
		GLuint projection = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(modelrotation, 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(project));
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		//glDrawArrays(GL_LINE_LOOP, i, i * 3);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int))); //green line but works
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	

	
	glUseProgram(0);
}
/// <summary>
/// have to fix  : make it follow generated path, not dirpath
/// </summary>
void entity::update(float tick) { //will be called per tick?
	//apply seperation
	//apply allginment
	//apply cohesion


	//flowfield movement
	

	//TODO: artificial potential field 적용?
	for (auto& [x, y, i] : destlist) { //calc accel
		if (x == -1) //no dest
			continue;
		//routing sequence
		accel[i] = vec2(0, 0);
		vec2 curpos = vec2(entitylist[i][0], entitylist[i][1]);

		vec2 olddirv = velocity[i];
		vec2 realv = vec2(x - entitylist[i][0], y - entitylist[i][1]);
		vec2 dirv = normalize(vec2(x - entitylist[i][0], y - entitylist[i][1])); //vector to go
		vec2 selected = 1 > hypot(realv.x, realv.y) ? realv : dirv; //v vector maximum len 1
		if (mode_status[flowmode]) {
			dirv = f->getNext(point(x, y), entitylist[i][0], entitylist[i][1]);
			selected = 3 * f->getNext(point(x, y), entitylist[i][0], entitylist[i][1]);
		}
		//selected = delta v = accel
		//vec2 dir = selected;


		for (int k = 0; k < 60; k++) {
			for (int j = 1; j <= size; j++) {
				vec2 radius = curpos + j * vec2(cos(6 * k), sin(6 * k));
				if (radius.x < 0 || radius.x >= sizex || radius.y < 0 || radius.y >= sizey) continue;
				if (iswall(radius.x, radius.y, &arr)) {
					vec2 vertical = vec2(cross(vec3(selected, 0), vec3(0, 0, 1)));
					selected += 0.02 * size / j * -vec2(cos(k * 6), sin(k * 6));
				}
			}
		}
		//selected = normalize(selected);
		/*
		for (auto& e : entitylist) {
			if (e[2] == i) continue;
			vec2 neighborpos = vec2(e[0], e[1]);
			vec2 diff = curpos - neighborpos;
			double dist = hypot(diff.x, diff.y);
			if (dist < (double)2 * size) {

				vec2 vertical = vec2(cross(vec3(selected, 0), vec3(0, 0, 1)));
				selected += (double)size/ dist * normalize(diff + vertical);
				accel[e[2]] -=  selected;
				//velocity[e[2]] += selected;
			}
		}*/
		accel[i] = selected;
		if (length(accel[i]) > 5) {
			accel[i] = normalize(accel[i]) * 5;
		}
		velocity[i] = 0.9 * velocity[i] + 1.1 * accel[i];
	}

	//RVO
	if (mode_status[rvo]) {
		for (auto& [xa, ya, a] : entitylist) { // a
			
			vector<vo> obstacles; //a ~ others RVOs
			multimap<float, vec2> possibles;

			for (auto& [xb, yb, b] : entitylist) { //get rvo here
				//b = other

				if (a == b) continue;
				vec2 pa = vec2(xa, ya);
				vec2 pb = vec2(xb, yb);
				vec2 relp = pb - pa;
				vec2 relv = velocity[a] - velocity[b];

				//if (length(relp) > 14 * size) continue;

				float rad = 2 * size; //size + other.size but all sizes r same now
				float square1 = length2(relp);
				float leg = sqrt(abs(square1 -  size * size));

				vo r;
				r.owner = b;
				r.position = (velocity[a] + velocity[b]) * 0.5f;//RVO  //velocity[b] for only VO
				r.midpos = (r.position + pb) / 2;
				r.left = vec2(relp.x * leg - relp.y * rad, relp.x * rad + relp.y * leg) / square1;
				r.right = vec2(relp.x * leg + relp.y * rad, -relp.x * rad + relp.y * leg) / square1; //if malfuncs then change to s c t method
				obstacles.emplace_back(r);
			}

			constexpr float alldir = 2 * pi<float>();

			float anglestart = 0;
			float anglemid = 0;
			float angleend = alldir;
			/*if (hypot(velocity[a].x, velocity[a].y) > 0.01) {
				float res = dot(vec2(0, 1), normalize(velocity[a]));

				anglemid = velocity[a].x > 0 ? acos(res) / 360 * alldir : (-acos(res) + 180) / 360 * alldir;
				anglestart = anglemid - alldir / 6 + alldir;
				angleend = anglemid + alldir / 6 + alldir;
			}*/
			for (float i = anglestart; i < angleend; i += 0.1) {
				float vsize = length(velocity[a]);
				float vstep = vsize / 10.f;
				for (float rad = 0.02f; rad < 10; rad += 0.1) {
					vec2 newv = vec2(rad * cos(i), rad * sin(i));
					bool valid = true;
					for (auto& ob : obstacles) {
						vec2 changedv = newv - ob.position;
						if (det(ob.left, changedv) < 0 && det(ob.right, changedv) > 0/* && length(newv) > length(ob.midpos - ob.position)*/) { valid = false; break; }
					}
					if (valid) {
						possibles.insert({ length2(newv - velocity[a]), newv});
					}
				}
			}
			
			
			//select optimal v
			//newvel -= minpen * 0.5;
			if (possibles.empty()) {
				//cout << "oh" << endl;
				//velocity[a] = -0.5 * velocity[a];
				velocity[a] = vec2(0, 0);
			}
			else {
				//cout << "velocity changed to" << "";
				//cout << length(velocity[a]) << " ";
				velocity[a] = possibles.begin()->second;
				if (length(velocity[a]) < 0.5) { //
					velocity[a] = vec2(0, 0);
				}
			}
		}
	}

	//apply final velocity
	// u have to make this smoother then before
	if (mode_status[flowmode]) {
		for (auto& [x, y, i] : entitylist) {
			if (abs(destlist[i][0] - x) < 2 * size && abs(destlist[i][1] - y) < 2 * size || reached[i] == true) {
				//entitylist[i] = { destlist[i][0], destlist[i][1], i };
				destlist[i] = { x, y, i };
				if (paths[i] != nullptr) {
					delete paths[i];
					paths[i] = nullptr;
					/*if (mode_status[trace])
						poopaths[i]->clear();*/
				}
				vec2 oldloc = vec2(x, y);
				reached[i] = true;
				
				if (length2(velocity[i]) > 100) {
					velocity[i] = 10 * normalize(velocity[i]);
				}
				//velocity[i] = 0.9 * velocity[i];
				vec2 newloc = oldloc + 10 * velocity[i] * tick;

				entitylist[i] = { newloc.x, newloc.y, i };
				continue;
			}
			//velocity[i] = 0.9 * velocity[i] + 80 * f->getNext(point(destlist[i][0], destlist[i][1]), (int)x, (int)y) + 30 * accel[i];
			if (length2(velocity[i]) > 100) {
				velocity[i] = 10 * normalize(velocity[i]);
			}
			vec2 oldloc = vec2(x, y);
			//vec2 newloc = oldloc + 10 * accel[i] * tick + 70 * f->getNext((int)x, (int)y) * tick;
			//vec2 newloc = oldloc + 0.6 * (80 * f->getNext(point(destlist[i][0], destlist[i][1]), (int)x, (int)y) + 30 * accel[i]) * tick;
			
			vec2 newloc = oldloc + 10 * velocity[i] * tick;
			entitylist[i] = { newloc.x, newloc.y, i };

			if (mode_status[trace])
				poopaths[i]->emplace_back(newloc);

			if (hypot(velocity[i].x, velocity[i].y) > 0.01) {
				float res = dot(vec2(0, 1), normalize(velocity[i]));

				rotations[i] = velocity[i].x > 0 ? acos(res) : -acos(res);
			}
		}
		return;
	}
	
	for (auto& [x, y, i] : destlist) { //move by pathmode
		//if path is allocated for this entitiy and entity is far from destination
		//velocity[i] = 0.9 * velocity[i] + 1.1 * accel[i];
		if (hypot(entitylist[i][0] - x, entitylist[i][1] - y) > 1.2 * size && curpathidx[i] != -1) { //size수정 필요?
		/*bool condition = true;
		if (curpathidx[i] != -1) {
			if (hypot(entitylist[i][0] - x, entitylist[i][1] - y) > 1.2 * size) condition = true;
			if (curpathidx[i] > 0 && curpathidx[i] != paths[i]->size()) {
				float nx = paths[i]->at(curpathidx[i] - 1).x;
				float ny = paths[i]->at(curpathidx[i] - 1).y;
				if (hypot(entitylist[i][0] - nx, entitylist[i][1] - ny) <= 1.2 * size) condition = false;
			}
		//	if (curpathidx[i] > 0 && curpathidx[i] != paths[i]->size()) {
		//		vec2 curloc = vec2(entitylist[i][0], entitylist[i][1]);
		//		vec2 pathslice = paths[i]->at(curpathidx[i]) - paths[i]->at(curpathidx[i] - 1);
		//		vec2 relativeloc = curloc - paths[i]->at(curpathidx[i] - 1);
		//		if (dot(pathslice, relativeloc) < length(pathslice)) { // not reached
		//			condition = false;
		//		}
		//	}
		}
		else condition = false;

		
		if (condition) {*/
			
			//sqrt(pow(entitylist[i][0] - x, 2) + pow(entitylist[i][1] - y, 2)) > 11) {
			reached[i] = false;
			///velocity[i] = 0.9 * velocity[i] + 1.1 * accel[i]; //v = friction * v + delta v
			if (length2(velocity[i]) > 100) {
				velocity[i] = 10 * normalize(velocity[i]);
			}
			
			vec2 oldloc = vec2(entitylist[i][0], entitylist[i][1]);
			//vec2 newloc = oldloc + 70 * selected * tick; //time slice by fps
			vec2 newloc = oldloc + 10 * velocity[i] * tick; //60 is velocity... friction * v * t + (a) * t^2
			//vec2 newloc = oldloc + dirv * 1000 * tick;

			

			//vec2 destv = vec2(x, y);
			entitylist[(int)i] = { newloc.x, newloc.y, i };
			if (mode_status[trace])
				poopaths[i]->emplace_back(newloc);
			//entitylist[(int)i] = { x, y, i };
			 //newloc.x 로 할지 x로 할지 생각해야 아님 다른 방법 필요

			if (hypot(velocity[i].x, velocity[i].y) > 0.01) {
				float res = dot(vec2(0, 1), normalize(velocity[i]));

				rotations[i] = velocity[i].x > 0 ? acos(res) : -acos(res);
			}
		}
		else { //no path or reached.
			reached[i] = true;
			
			velocity[i] = 0.9 * velocity[i]; //v = v + delta v

			if (length2(velocity[i]) > 100) {
				velocity[i] = 10 * normalize(velocity[i]);
			}
			vec2 oldloc = vec2(entitylist[i][0], entitylist[i][1]);
			//vec2 newloc = oldloc + 70 * selected * tick; //time slice by fps
			vec2 newloc = oldloc + 10 * velocity[i] * tick; //60 is velocity... vold * t + a * t^2
			//vec2 newloc = oldloc + dirv * 1000 * tick;



			//vec2 destv = vec2(x, y);
			//entitylist[(int)i] = { newloc.x, newloc.y, i };
			entitylist[(int)i] = { newloc.x, newloc.y, i };
			 //newloc.x 로 할지 x로 할지 생각해야 아님 다른 방법 필요

			//if ()
			x = newloc.x;
			y = newloc.y;

			if (hypot(velocity[i].x, velocity[i].y) > 0.01) {
				float res = dot(vec2(0, 1), normalize(velocity[i]));

				rotations[i] = velocity[i].x > 0 ? acos(res) : -acos(res);
			}
			

			
			
			//velocity[i] = vec2(0, 0);

			
			int curidx = getpathidx(i);
			if (curidx == -1) continue;

			//cout << i << " : " << curidx << endl;
			//cout << recentt ime << endl;
			if (curidx == paths[i]->size()) { //reached endpoint
				delete paths[i];
				paths[i] = nullptr;
				
				//velocity[i] = vec2(0, 0);
				//accel[i] = vec2(0, 0);
				continue;
			}

			//cout << i << " : "<< curidx << endl;
			destlist[i] = { paths[i]->at(curidx).x, paths[i]->at(curidx).y, (float)i };
		}

		//or do collision at here.

	}
	//collision testcode
	
}
/// <summary>
/// get eye direction of entity num : offset.  initial eyevec will be (1, 0)
/// </summary>
/// <param name="offset"></param>
/// <returns></returns>
vec2 entity::getdir(int offset) {
	return vec2(0, 0);
}
/// <summary>
/// set the path of offset numed entity 
/// </summary>
/// <param name="offset"></param>
/// <param name="path"></param>
void entity::setpath(int offset, vector<vec2>* path) {
	if (path == nullptr) {
		if (paths[offset] != nullptr) return;
		curpathidx[offset] = -1;
		reached[offset] = false;
		return;
	}
	if (paths[offset] != nullptr) {
		delete paths[offset];
	}
	destlist[offset] = { path->at(0).x , path->at(0).y, (float)offset };
	reached[offset] = false;
	curpathidx[offset] = 1;
	this->paths[offset] = path;
}

vector<vector<vec2>*> entity::getentitypaths() {
	return this->paths;
}
vector<vector<vec2>*> entity::gettrailpath() {
	return this->poopaths;
}
void entity::cleartrail() {
	for (auto &p : poopaths) {
		p->clear();
	}
}

int entity::getpathidx(int offset) {
	if (paths[offset] == nullptr) {
		curpathidx[offset] = -1;
		reached[offset] = false;
		return -1;
	}
	if (curpathidx[offset] == paths[offset]->size()) {
		//delete paths[offset];
		//if (reached[offset]) paths[offset] = nullptr;

		return curpathidx[offset];
	}
	return this->curpathidx[offset]++;
}
void entity::clearvelacc() {
	
}