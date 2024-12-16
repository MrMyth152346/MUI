#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>

#include "../headers/MUI.hh"

typedef enum
{
    MUI_CALCULATOR_SIN = 0,
    MUI_CALCULATOR_COS = 1,
    MUI_CALCULATOR_HTAN = 2,
    MUI_CALCULATOR_SIGMOID = 3,
    MUI_CALCULATOR_RELU = 4,
} MUI_CALCULATOR_FUNCS;

int main(int argc, char *argv[])
{
    int isInit = MUI_Init(SDL_INIT_VIDEO);

    if (isInit != 0)
        return -1;

    bool running = true;

    SDL_Window *window = SDL_CreateWindow("MUI Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 500, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;

    TTF_Font *font1 = TTF_OpenFont("files/fonts/Roboto-Regular.ttf", 36);
    TTF_Font *font2 = font1;

    if (font1 == NULL)
        std::cout << TTF_GetError() << std::endl;
    if (font2 == NULL)
        std::cout << TTF_GetError() << std::endl;

    MUI_Updater *updater = MUI_CreateUpdater(window);

    MUI_Element *numberFrame   = MUI_CreateFrame(renderer, SDL_Color{72,0,72,255}, MUI_Vector2(0,0.25), MUI_Vector2(0.75,0.75), MUI_SCALING_SCALE, MUI_SCALE_XY, false, false);
    MUI_Element *operatorFrame = MUI_CreateFrame(renderer, SDL_Color{50,0,50,255}, MUI_Vector2(0.75,0.25), MUI_Vector2(0.25,0.75), MUI_SCALING_SCALE, MUI_SCALE_XY, false, false);
    MUI_Element *resultFrame   = MUI_CreateFrame(renderer, SDL_Color{61,0,61,255}, MUI_Vector2(0,0), MUI_Vector2(1,0.25), MUI_SCALING_SCALE, MUI_SCALE_XY, false, false);
    MUI_Element *functionFrame = MUI_CreateFrame(renderer, SDL_Color{80,0,80,255}, MUI_Vector2(0.1,0.1), MUI_Vector2(0.5,0.5), MUI_SCALING_SCALE, MUI_SCALE_XY, false, true);

    functionFrame->visible = false;

    std::vector<int>  funcs = {MUI_CALCULATOR_SIN, MUI_CALCULATOR_COS, MUI_CALCULATOR_HTAN, MUI_CALCULATOR_SIGMOID, MUI_CALCULATOR_RELU};
    std::vector<char> ops   = {'+', '-', '/', '*'};

    char op = '+';
    double sum = 0.0;

    MUI_Element *funcOpenButton = MUI_CreateText(renderer, "funcs", font1, SDL_Color{255,255,255,255}, SDL_Color{0,0,0,255}, MUI_Vector2(0,0.75f), MUI_Vector2((1.0f / 3.0f), 0.25f), MUI_SCALING_SCALE, MUI_SCALE_XY, true, false);
    MUI_Element *sumText        = MUI_CreateText(renderer, std::to_string(sum).c_str(), font1, SDL_Color{255,255,255,255}, SDL_Color{110,0,110,255}, MUI_Vector2(0,0), MUI_Vector2(1.0f, 1.0f), MUI_SCALING_SCALE, MUI_SCALE_XY, false, false);

    funcOpenButton->Clicked = [&functionFrame]()
    {
        if (functionFrame->visible == false)
            functionFrame->visible = true;
        else
            functionFrame->visible = false;
    };

    MUI_ElementSetParent(funcOpenButton, numberFrame);
    MUI_ElementSetParent(sumText, resultFrame);


    for (int i = 0; i < funcs.size(); i++)
    {
        std::string text = "";

        switch (funcs[i])
        {
        case MUI_CALCULATOR_SIN:
            text = "sin";
            break;
        case MUI_CALCULATOR_COS:
            text = "cos";
            break;
        case MUI_CALCULATOR_SIGMOID:
            text = "sigmoid";
            break;
        case MUI_CALCULATOR_HTAN:
            text = "htan";
            break;
        case MUI_CALCULATOR_RELU:
            text = "relu";
            break;
        }
    
        MUI_Element *funcButton = MUI_CreateText(renderer, text.c_str(), font2, SDL_Color{255,255,255,255}, SDL_Color{80,0,80,255}, MUI_Vector2(0, i * (1.0f / ops.size())), MUI_Vector2(1.0f, 1.0f / ops.size()), MUI_SCALING_SCALE, MUI_SCALE_XY, true, false);

        funcButton->Clicked = [i, &sum, &funcs, text]()
        {
            std::cout << text << std::endl;
            switch (funcs[i])
            {
            case MUI_CALCULATOR_SIN:
                sum = SDL_sin(sum);
                break;
            case MUI_CALCULATOR_COS:
                sum = SDL_cos(sum);
                break;
            case MUI_CALCULATOR_SIGMOID:
                sum = (1.0 / (exp(-(sum)) + 1.0));
                break;
            case MUI_CALCULATOR_HTAN:
                sum = (exp((sum)) - exp(-(sum))) / (exp((sum)) + exp(-(sum)));
                break;
            case MUI_CALCULATOR_RELU:
                sum = (((sum) > 0.0) ? (sum) : 0.0);
                break;
            }
        };

        MUI_ElementSetParent(funcButton, functionFrame);
    }


    for (int i = 0; i < ops.size(); i++)
    {
        MUI_Element *opButton = MUI_CreateText(renderer, std::string(1, ops[i]).c_str(), font2, SDL_Color{255,255,255,255}, SDL_Color{90,0,90,255}, MUI_Vector2(0, i * (1.0f / ops.size())), MUI_Vector2(1.0f, 1.0f / ops.size()), MUI_SCALING_SCALE, MUI_SCALE_XY, true, false);
    
        opButton->Clicked = [i, &op, &ops]()
        {
            op = ops[i];
        };

        MUI_ElementSetParent(opButton, operatorFrame);
    }

    float x = 0;
    float y = 0;
    for (int i = 0; i < 10; i++)
    {
        if (x > 2.0f)
        {
            y++;
            x = 0.0f;
        }

        if (i == 9)
            x = 1.0f;

        MUI_Element *numberButton = MUI_CreateText(renderer, std::to_string(i).c_str(), font2, SDL_Color{255,255,255,255}, SDL_Color{100,0,100,255}, MUI_Vector2(x * (1.0f / 3.0f), y * 0.25f), MUI_Vector2((1.0f / 3.0f), 0.25f), MUI_SCALING_SCALE, MUI_SCALE_XY, true, false);

        numberButton->Clicked = [&sum, i, &op]()
        {
            std::cout << i << std::endl;

            switch (op)
            {
            case '+':
                sum += i;
                break;
            case '-':
                sum -= i;
                break;
            case '*':
                sum *= i;
                break;
            case '/':
                sum /= i;
                break;
            
            default:
                sum += i;
                break;
            }
        };

        MUI_ElementSetParent(numberButton, numberFrame);

        x++;
    }

    //operatorFrame->visible = false;
    //resultFrame->visible = false;

    MUI_UpdateCopy(updater, numberFrame);
    MUI_UpdateCopy(updater, operatorFrame);
    MUI_UpdateCopy(updater, resultFrame);
    MUI_UpdateCopy(updater, functionFrame);

    while (running)
    {
        MUI_UpdateText(renderer, sumText, std::to_string(sum).c_str(), font1, SDL_Color{255,255,255,255});
        
        SDL_PollEvent(&event);
        SDL_RenderClear(renderer);
        MUI_Update(updater, renderer, event);
        SDL_RenderPresent(renderer);

        switch (updater->event)
        {
        case MUI_HOVERED:
            

            break;
        case MUI_CLICKED:
            
            break;
        
        default:
            
            break;
        }

        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        
        default:
            break;
        }
    }

    return 0;
}