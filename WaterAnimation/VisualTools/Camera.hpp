#ifndef DEF_CAMERA_HPP
#define DEF_CAMERA_HPP

#include "stdafx.h"

class  Camera {
public:
	Camera(double _radius) :radius{ _radius } {
		if (radius < 0.0f) {
			cerr << "radius should be larger than zero.\n";
			throw logic_error{ "radius is smaller than zero." };
		}
		x = 0.0;
		y = 0.0;
		z = radius;
	}
	void modify_yaw(uint degree, bool increment) {
		auto delta = (double)degree * one;
		if (increment) {
			if (yaw + delta > pi) {}
			else {
				yaw += delta;
			}
		}
		else {
			if (yaw - delta < 0.0) {}
			else {
				yaw -= delta;
			}
		}
	}
	void modify_pitch(uint degree, bool increment) {
		auto delta = (double)degree * one;
		if (increment) {
			if (pitch + delta > pi / 2.0) {}
			else {
				pitch += delta;
			}
		}
		else {
			if (pitch - delta < -pi / 2.0) {}
			else {
				pitch -= delta;
			}
		}
	}
	void modify_z(double delta, bool increment) {
		if (delta < 0.0) {
			cerr << "delta must be larger than zero.\n";
			throw out_of_range{ "delta out of range." };
		}
		if (increment) {
			if (z + delta > radius) { return; }
			else {
				z += delta;
				cal_y();
			}
		}
		else {
			if (z - delta < 0.0) { return; }
			else {
				z -= delta;
				cal_y();
			}
		}
		update_camera_front();
	}
	void modify_x(double delta, bool increment) {
		if (delta < 0.0) {
			cerr << "delta must be larger than zero.\n";
			throw out_of_range{ "delta out of range." };
		}
		if (increment) {
			if (x + delta > radius) { return; }
			else {
				x += radius;
				cal_y();
			}
		}
		else {
			if (x - delta < -radius) { return; }
			else {
				x -= radius;
				cal_y();
			}
		}
		update_camera_front();
	}
	glm::mat4 get_view_matrix() {
		auto o = glm::vec3{ x,y,z };
		return glm::lookAt(o, o + camera_front, camera_up);
	}
protected:
	double one = pi / 180.0;
	double radius, pitch = 0.0, yaw = pi / 2.0;
	double x, y, z;
	glm::vec3 camera_up{ 0.0f,1.0f,0.0f };
	glm::vec3 camera_front{ 0.0f,0.0f,-1.0f };
private:
	void cal_y() {
		y = sqrt(radius * radius - x * x - z * z);
	}
	void update_camera_front() {
		camera_front.x = cos(yaw) * cos(pitch);
		camera_front.y = sin(pitch);
		camera_front.z = -sin(yaw) * cos(pitch);
	}
};

#endif // !DEF_CAMERA_HPP
