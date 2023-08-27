#include <iostream>

#include "ProjectileGame.h" 
#include "SDL_image.h"
#include "math.h"
using namespace std;

extern int g_current_game_phase;
extern bool g_flag_running;
extern SDL_Renderer* g_renderer;
extern SDL_Window* g_window;
extern float g_timestep_s;
SDL_Rect orb_rect[21];
int m_x ,m_y;

ProjectileGame::ProjectileGame()
{
	g_flag_running = true;
	// Texture
	{
		SDL_Surface* ball_surface = IMG_Load("../../Resources/ball.png");
		ball_src_rectangle_.x = 0;
		ball_src_rectangle_.y = 0;
		ball_src_rectangle_.w = ball_surface->w;
		ball_src_rectangle_.h = ball_surface->h;

		ball_texture_ = SDL_CreateTextureFromSurface(g_renderer, ball_surface);
		SDL_FreeSurface(ball_surface);

		SDL_Surface* orbit_surface = IMG_Load("../../Resources/orbit.png");
		orbit_rec.x = 0;
		orbit_rec.y = 0;
		orbit_rec.w = (orbit_surface->w);
		orbit_rec.h = orbit_surface->h;

		orbit_texture = SDL_CreateTextureFromSurface(g_renderer, orbit_surface);
		SDL_FreeSurface(orbit_surface);
	}

	// Initialize ball pointers
	for ( int i=0; i<MAX_BALL_NUM; i++ )
	{
		balls_[i] = 0;
	}

	num_of_balls_ = 0;

	mouse_win_x_ = 0;
	mouse_win_y_ = 0;
	orb = new Ball(0.11f, &room_);
	AddNewBall();
}



ProjectileGame::~ProjectileGame()
{
	// Delete balls
	for ( int i=0; i<MAX_BALL_NUM; i++ )
	{
		if ( balls_[i] != 0 )
			delete balls_[i];

		balls_[i] = 0;
	}
	delete orb;
	num_of_balls_ = 0;
	SDL_DestroyTexture(ball_texture_);
	SDL_DestroyTexture(orbit_texture);
}


void
ProjectileGame::AddNewBall()
{
	if ( num_of_balls_ == MAX_BALL_NUM ) return;

	// Create new Ball
	Ball *ball = new Ball(0.11f, &room_);

	// Add to the list
	balls_[num_of_balls_] = ball;

	// Increase Num
	num_of_balls_++;

}


void
ProjectileGame::Update()
{
	// Update balls
	for ( int i=0; i<num_of_balls_; i++ )
	{
		balls_[i]->Update(g_timestep_s);
	}

}

void 
ProjectileGame::Render()
{
	

	SDL_SetRenderDrawColor(g_renderer, 255,255,255,255);
	SDL_RenderClear(g_renderer); // clear the renderer to the draw color
	
	
	// Draw room_
	{
		SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);

		// Left Wall
		SDL_RenderDrawLine(g_renderer, G2W_X(room_.left_wall_x()),
										G2W_Y(0),
										G2W_X(room_.left_wall_x()),
										G2W_Y(room_.height()) );

		
		// Right Wall
		SDL_RenderDrawLine(g_renderer, G2W_X(room_.right_wall_x()),
										G2W_Y(0),
										G2W_X(room_.right_wall_x()),
										G2W_Y(room_.height()) );

		// Top Wall
		SDL_RenderDrawLine(g_renderer, G2W_X(room_.left_wall_x()),
										G2W_Y(room_.height()),
										G2W_X(room_.right_wall_x()),
										G2W_Y(room_.height()) );

		// Bottom Wall
		SDL_RenderDrawLine(g_renderer, G2W_X(room_.left_wall_x()),
										G2W_Y(0),
										G2W_X(room_.right_wall_x()),
										G2W_Y(0) );

		// Fence
		SDL_RenderDrawLine(g_renderer, G2W_X( room_.vertiacal_fence_pos_x()),
										G2W_Y(0),
										G2W_X( room_.vertiacal_fence_pos_x()),
										G2W_Y( room_.ground_height()+room_.vertiacal_fence_height()) );
	}


	// Draw Balls
	for ( int i=0; i<num_of_balls_; i++ )
	{
		Ball *ball = balls_[i];

		int ball_win_x = G2W_X(balls_[i]->pos_x());
		int ball_win_y = G2W_Y(balls_[i]->pos_y());

		double win_radius = G2W_Scale * balls_[i]->radius();

		SDL_Rect dest_rect;
		dest_rect.w = (int)(2*win_radius);
		dest_rect.h = (int)(2*win_radius);
		dest_rect.x = (int)(ball_win_x - win_radius);
		dest_rect.y = (int)(ball_win_y - win_radius);

		SDL_RenderCopy(g_renderer, ball_texture_, &ball_src_rectangle_, &dest_rect);
	}
	if (num_of_balls_ < MAX_BALL_NUM) {
		for (int i = 0;i < 10;i++) {
			SDL_RenderCopy(g_renderer, orbit_texture, &orbit_rec, &orb_rect[i]);
		}
	}

	// Draw the Guide Line 
	if ( num_of_balls_ <MAX_BALL_NUM )
	{
		SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 100);
		SDL_RenderDrawLine(g_renderer, G2W_X(balls_[num_of_balls_-1]->pos_x()),
										G2W_Y(balls_[num_of_balls_-1]->pos_y()),
										mouse_win_x_, 
										mouse_win_y_ );
	}
	

	SDL_RenderPresent(g_renderer); // draw to the screen
}



void 
ProjectileGame::HandleEvents()
{
	SDL_Event event;
	if(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			g_flag_running = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
		
			// If the mouse left button is pressed. 
			if ( event.button.button == SDL_BUTTON_LEFT )
			{
				// Get the cursor's x position.
				mouse_win_x_ = event.button.x;
				mouse_win_y_ = event.button.y;

				
				double mouse_game_x = W2G_X(mouse_win_x_);
				double mouse_game_y = W2G_Y(mouse_win_y_);
				
				// Luanch
				if ( num_of_balls_ < MAX_BALL_NUM )
				{
					Ball *ball = balls_[num_of_balls_-1];

					// Guide Line Vector
					double guide_line_x = mouse_game_x - ball->pos_x();
					double guide_line_y = mouse_game_y - ball->pos_y();

					// Lauching Force
					double launcing_force_x = 8.0 * guide_line_x;
					double launcing_force_y = 8.0 * guide_line_y;

					// Launch
					balls_[num_of_balls_-1]->Launch(launcing_force_x, launcing_force_y);

					
					// Add a new ball for the next
					AddNewBall();
					
				}
			}

			
		case SDL_MOUSEMOTION:
			{
				
				
				// Get the cursor's x position.
				mouse_win_x_ = event.button.x;
				mouse_win_y_ = event.button.y;
				
				if (mouse_win_x_ != m_x && mouse_win_y_ != m_y && num_of_balls_ < MAX_BALL_NUM) {
					m_x = mouse_win_x_, m_y = mouse_win_y_;
					orb->Reset();
					double guide_line_x = W2G_X(mouse_win_x_) - orb->pos_x();
					double guide_line_y = W2G_Y(mouse_win_y_) - orb->pos_y();

					// Lauching Force
					double launcing_force_x = 8.0 * guide_line_x;
					double launcing_force_y = 8.0 * guide_line_y;

					orb->Launch(launcing_force_x, launcing_force_y);
					orb_rect[0].x = (int)(G2W_X(orb->pos_x()));
					orb_rect[0].y = (int)(G2W_Y(orb->pos_y()));
					for (int i = 1;i <= 30;i++) {
						
						orb->Update(g_timestep_s);
						if (i % 6 != 0)continue;
						int ball_win_x = G2W_X(orb->pos_x());
						int ball_win_y = G2W_Y(orb->pos_y());

						double win_radius = G2W_Scale * orb->radius();

						orb_rect[i/6].w = (int)(2 * win_radius);
						orb_rect[i/6].h = (int)(2 * win_radius);
						orb_rect[i/6].x = (int)(ball_win_x - win_radius);
						orb_rect[i/6].y = (int)(ball_win_y - win_radius);
						
					}
				}
			}
			break;
		case SDL_KEYDOWN :
				switch (event.key.keysym.sym) {
				case SDLK_1:
					if (G2W_Scale == 150.0f)G2W_Scale *= 2, W2G_Scale *= 2;
					break;
				case SDLK_2:
					if (G2W_Scale == 300.0f)G2W_Scale /= 2, W2G_Scale /= 2;
					break;
				case SDLK_KP_1:
					if (G2W_Scale == 150.0f)G2W_Scale *= 2, W2G_Scale *= 2;
					break;
				case SDLK_KP_2:
					if (G2W_Scale == 300.0f)G2W_Scale /= 2, W2G_Scale /= 2;
					break;
				default:
					break;
				}
		default:
			break;
		}
	}
}
