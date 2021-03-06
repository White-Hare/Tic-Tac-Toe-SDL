#include <SDL.h>
#include <SDL_ttf.h>
#include<gfx/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include<time.h>


#define WIDTH 320
#define HEIGHT 320


//Drawing
void DrawBoard(SDL_Renderer* renderer, int x, int y);
void DrawX(SDL_Renderer* renderer, int x, int y);
void DrawO(SDL_Renderer* renderer, int x, int y);

//AI
int get_best_move(int* x, int* y,char symbol);


bool is_winning(char symbol,char board[3][3]);
SDL_Texture* writeToTextue(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
const int side_length = 100;

char board[3][3];
enum Players
{
    PLAYER1,
    PLAYER2
};

Players currentPlayer = PLAYER1;

class XOX
{

	friend void DrawX(SDL_Renderer* renderer, int x, int y);
	friend void DrawO(SDL_Renderer* renderer, int x, int y);

	int x, y;
	int number;
	SDL_Rect* rect;
	bool set;

public:
	XOX(int x, int y, unsigned int number)
	{
		this->x = x;
		this->y = y;
		this->number = number;
		rect = new SDL_Rect{ x,y,side_length,side_length };
		set = false;
	}

	void place(int x, int y)
	{
		if (set == false) {

			if (
				(x > this->x&&x < this->x + this->rect->w) &&
				(y > this->y&&y < this->y + this->rect->h))
			{

				if (currentPlayer == PLAYER1) {
					board[number / 3][number % 3] = 'o';
					currentPlayer = PLAYER2;
				}
				else if (currentPlayer == PLAYER2) {
					board[number / 3][number % 3] = 'x';
					currentPlayer = PLAYER1;
				}
				set = true;
			}
		}
	}

    bool set_occupied()
	{
	
	    return this->set = true;
	}

	void render(SDL_Renderer* renderer)
	{

		if (board[number / 3][number % 3] == 'o')
			DrawO(renderer, x, y);

		else if (board[number / 3][number % 3] == 'x')
			DrawX(renderer, x, y);
	}

	~XOX() {}
};


int main(int argc,char* args[])
{
	srand(time(NULL));



	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		printf("SDL Couldn't started. SDL_ERROR:%s", SDL_GetError());

    if(TTF_Init()==-1)
		printf("TTF Couldn't started. TTF_ERROR:%s", TTF_GetError());


	SDL_Window* window = SDL_CreateWindow("XOX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,SDL_WINDOW_SHOWN);
    
	SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

	SDL_Event* event = new SDL_Event;

	TTF_Font* font = TTF_OpenFont("arial.ttf", 40);
    if(!font)
		printf("arial.ttf not found. TTF_ERROR:%s", TTF_GetError());

        
	XOX *cells[9];

	SDL_Rect* text_pos =new SDL_Rect { 100,80,WIDTH/2,HEIGHT/2 };
	SDL_Texture* player1_victory_text = writeToTextue(renderer, font, "PLAYER 1 WIN PRESS R TO RESTART OR ESC TO QUIT", SDL_Color{ 200, 0, 0 });
	SDL_Texture* player2_victory_text = writeToTextue(renderer, font, "PLAYER 2 WIN PRESS R TO RESTART OR ESC TO QUIT", SDL_Color{ 0, 0, 200 });
	SDL_Texture* tie_text = writeToTextue(renderer, font, "DRAW PRESS R TO RESTART OR ESC TO QUIT", SDL_Color{ 0, 0, 0 });


	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++)
		{
			cells[y * 3 + x] = new XOX(x * (side_length+10), y * (side_length+10), x + y * 3);
		}
	}

	Players winner;
	bool winner_setted = false, victory_of_o = false, victory_of_x = false;


	bool running = true;
    while(running)
    {
		while (SDL_PollEvent(event))
		{
			if (event->type == SDL_QUIT)
				running = false;

			if (event->type == SDL_KEYDOWN) {
				if (event->key.keysym.sym == SDLK_ESCAPE)
					running = false;

				if (event->key.keysym.sym == SDLK_r) {
					for (int y = 0; y < 3; y++) {
						for (int x = 0; x < 3; x++)
						{
							cells[y * 3 + x] = new XOX(x * (side_length + 10), y * (side_length + 10), x + y * 3);
							board[y][x] = NULL;
						}
					}
					winner_setted = false;
					victory_of_o = false;
					victory_of_x = false;
				}
			}
		}



		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		DrawBoard(renderer, 0, 0);

		if (currentPlayer == PLAYER2 && !winner_setted)
		{
			int x, y;
			get_best_move(&x, &y,'x');
			board[y][x] = 'x';
			cells[x + 3 * y]->set_occupied();

			currentPlayer = PLAYER1;
		}


        for (int i = 0; i < 9; i++) {
            if(!winner_setted)
				if (event->button.button == SDL_BUTTON_LEFT && currentPlayer == PLAYER1) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					cells[i]->place(x, y);
				}

				cells[i]->render(renderer);
			}
		


        if(!winner_setted)
        {
			 victory_of_o = is_winning('o', board);
			 victory_of_x = is_winning('x', board);
        }


		if (victory_of_o)//player changing happens before this function
		{
			SDL_RenderCopy(renderer, player1_victory_text, NULL, text_pos);
			winner_setted = true;
		}
		
		if (victory_of_x) {
			SDL_RenderCopy(renderer, player2_victory_text, NULL, text_pos);
			winner_setted = true;
		}
		bool all_cells_full=true;
        for (int i=0;i<9;i++)
			if (board[i / 3][i % 3] == NULL) {
				all_cells_full = false;
				break;
			}
        if(all_cells_full && !victory_of_o && !victory_of_x)
			SDL_RenderCopy(renderer, tie_text, NULL, text_pos);



		SDL_RenderPresent(renderer);


    }


    
    return 0;
}
void DrawX(SDL_Renderer* renderer, int x, int y)
{
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

	thickLineRGBA(renderer, x, y, x + side_length, y + side_length, 3, 0, 0, 255, 255);
	thickLineRGBA(renderer, x + side_length, y, x, y + side_length, 3, 0, 0, 255, 255);

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

}

void DrawO(SDL_Renderer* renderer, int x, int y)
{
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

	circleRGBA(renderer, x + side_length / 2, y + side_length / 2, side_length / 2, 200, 0, 0, 255);

	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void DrawBoard(SDL_Renderer* renderer, int xpos, int ypos)
{
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

	for (int x = 1; x < 3; x++)
		thickLineRGBA(
			renderer, xpos + (side_length + 10)*x-5, ypos,
			xpos + (side_length + 10)*x-5, ypos+(side_length + 10) * 3,
            2,0,0,0,255
		);
	for (int x = 1; x < 3; x++)
		thickLineRGBA(
			renderer, xpos , ypos + (side_length + 10)*x - 5,
			xpos + (side_length + 10) * 3, ypos+ (side_length + 10)*x - 5,
			2, 0, 0, 0, 255
		);


	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

SDL_Texture* writeToTextue(SDL_Renderer* renderer,TTF_Font* font,std::string text,SDL_Color color)
{
	SDL_Surface* surface =TTF_RenderText_Blended_Wrapped(font, text.c_str(), color,WIDTH);

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr)
		std::cout << "texture couldn't be created\n";

	return texture;
}



//AI

bool is_winning(char symbol,char board[3][3])
{
	for (int y = 0; y < 3; y++)
		if (board[y][0] == symbol && board[y][1] == symbol && board[y][2] == symbol) {
			return true;
		}
        
	for (int x = 0; x < 3; x++)
		if (board[0][x] == symbol && board[1][x] == symbol && board[2][x] == symbol) {
			return true;
		}



	if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
		return true;
	}

	if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
		return true;
	}

    return false;
}


int get_best_move(int* x, int* y, char symbol)
{
	char cpy_board[3][3];
	for (int py = 0; py < 3; py++)
		for (int px = 0; px < 3; px++)
			cpy_board[py][px] = board[py][px];


    for(int py=0;py<3;py++)
		for (int px = 0; px < 3; px++)
		{
			if (!board[py][px]) {
				cpy_board[py][px] = 'x';

				if (is_winning('x', cpy_board))
				{
				    *x = px;
					*y = py;
					return 1;
				}

				cpy_board[py][px] = NULL;
			}
		}

	for (int py = 0; py < 3; py++)
		for (int px = 0; px < 3; px++) {
			if (!board[py][px]) {
				cpy_board[py][px] = 'o';
				if (is_winning('o', cpy_board))
				{
					*x = px;
					*y = py;
					return -1;
				}
				cpy_board[py][px] = NULL;
			}
		}

    for(int py=0;py<3;py++)
        for(int px=0;px<3;px++)
            if(!board[py][px])
            {
				*y = py;
				*x = px;
            }
                

	return 0;
}


