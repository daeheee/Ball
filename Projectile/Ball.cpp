#include "G2W.h"
#include "Ball.h"
#include "SDL_image.h"
#include <iostream>
#include <cmath>
using namespace std;

Ball::Ball(double radius, Room *room)
{
	room_ = room;
	radius_ = radius;

	v_[0] = 0;
	v_[1] = 0;

	mass_ = 2; // 2kg
	coeff_of_restitution_ = 0.7;
	Reset();
}

double Ball::CalLength(double x, double y) {
	return (x * x) + ((y - room_->vertiacal_fence_height()) * (y - room_->vertiacal_fence_height()));
}
double Ball::GetTan(double x, double y) {
	return std::atan2f(x ,(y - 1)) * PI_;
}

void
Ball::Reset()
{
	p_[0] = radius_ + room_->left_wall_x();
	p_[1] = radius_;

	v_[0] = 0;
	v_[1] = 0;
}

void
Ball::Launch(double initial_force_x, double initial_force_y)
{
	v_[0] = v_[0] + (initial_force_x/mass_);
	v_[1] = v_[1] + (initial_force_y/mass_);
}

void 
Ball::Update(double timestep_s)
{
	double dt = timestep_s;	// seconds
	
	// 가속도
	double a[2];
	a[0] = 0;
	a[1] = room_->gravitational_acc_y();// Gravity

	// Move
	p_[0] = p_[0] + dt * v_[0];
	p_[1] = p_[1] + dt * v_[1];


	// Collision with Ground
	if ( p_[1]-radius_ < room_->ground_height() && v_[1] < 0 ) 
	{
		p_[1] = radius_ + room_->ground_height();

		v_[1] = -1*v_[1];

		// Coefficient of restitution
		v_[1] = coeff_of_restitution_ * v_[1];
	}

	// Collision with Ceilling
	else if ( p_[1]+radius_ > room_->height() && v_[1] > 0 ) 
	{
		p_[1] = room_->height() - radius_;

		v_[1] = -1*v_[1];

		// Coefficient of restitution
		v_[1] = coeff_of_restitution_ * v_[1];
	}

	// Collision with Left Wall
	if ( p_[0] - radius_ < room_->left_wall_x() && v_[0] < 0 )
	{
		p_[0] = room_->left_wall_x() + radius_;

		v_[0] = -1 * v_[0];

		// Coefficient of restitution
		v_[0] = coeff_of_restitution_ * v_[0];
	}

	// Collision with Right Wall
	else if (p_[0] + radius_ > room_->right_wall_x() && v_[0] > 0)
	{
		p_[0] = room_->right_wall_x() - radius_;

		v_[0] = -1 * v_[0];

		// Coefficient of restitution
		v_[0] = coeff_of_restitution_ * v_[0];
	}
	// 펜스에 꼭지점으로 부딪칠 때(펜스 좌측에서)
	else if (CalLength(p_[0],p_[1])<=0.11*0.11
		&&p_[1]>=room_->vertiacal_fence_height()&& v_[0]>0 && v_[1]<0) {
		std::cout << -2 << '\n';

		if (GetTan(p_[0], p_[1]) > -90.f && GetTan(p_[0], p_[1]) < 0.f) {
			p_[0] = room_->vertiacal_fence_pos_x() - radius_;
			v_[0] = -1 * v_[0];
			v_[0] = coeff_of_restitution_ * v_[0];
		}
		else {
			p_[1]= room_->vertiacal_fence_height() + radius_;
			v_[1] = -1 * v_[1];
			v_[1] = coeff_of_restitution_ * v_[1];

		}
	}

	// 펜스에 꼭지점으로 부딪칠 때(펜스 우측에서)
	else if (CalLength(p_[0], p_[1]) <= 0.11 * 0.11 
		&& p_[1]>=room_->vertiacal_fence_height()&& v_[0] < 0) {

		if (GetTan(p_[0], p_[1]) > -90.f && GetTan(p_[0], p_[1]) < 0.f) {
			p_[1] = room_->vertiacal_fence_height() + radius_;
			v_[1] = -1 * v_[1];
			v_[1] = coeff_of_restitution_ * v_[1];
		}
		else {
			p_[0] = room_->vertiacal_fence_pos_x() + radius_;
			v_[0] = -1 * v_[0];
			v_[0] = coeff_of_restitution_ * v_[0];
		}
		
	}
	// Collision with fence left
	else if (p_[0]*p_[0]<0.11f*0.11f && p_[1]< room_->vertiacal_fence_height()&& v_[0] > 0) {
		std::cout << -1 << '\n';
		p_[0] = room_->vertiacal_fence_pos_x() - radius_;
		v_[0] = -1 * v_[0];
		// Coefficient of restitution
		v_[0] = coeff_of_restitution_ * v_[0];
	}
	// Collision with fence right
	else if (p_[0] * p_[0] < 0.11f * 0.11f && p_[1] < room_->vertiacal_fence_height() && v_[0] < 0) {
		p_[0] = room_->vertiacal_fence_pos_x() + radius_;
		v_[0] = -1 * v_[0];
		// Coefficient of restitution
		v_[0] = coeff_of_restitution_ * v_[0];
	}
	
	//속도 = 이전속도 + 시간(dt) * 가속도;
	if (p_[1] <= 0+radius_) {
		v_[0]*=0.96f;
		v_[1] = v_[1] + dt * a[1];
	}
	else {
		v_[0] = v_[0] + dt * a[0];
		v_[1] = v_[1] + dt * a[1];
	}
}
