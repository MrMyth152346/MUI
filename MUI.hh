#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <functional>
#include <algorithm>
#include <math.h>

typedef enum
{
    MUI_SCALING_SCALE = 0,
    MUI_SCALING_OFFSET = 1,

    MUI_SCALE_XX = 2,
    MUI_SCALE_YY = 3,
    MUI_SCALE_XY = 4
} MUI_SCALING;

typedef enum
{
    MUI_NOEVENT = 0,
    MUI_CLICKED = 1,
    MUI_HOVERED = 2,
    MUI_DRAGGED = 3
} MUI_EVENTS;


class MUI_Vector2
{
public:
    float X;
    float Y;

    MUI_Vector2 operator+(MUI_Vector2 vector2)
    {
        MUI_Vector2 newVector = *this;

        newVector.X += vector2.X;
        newVector.Y += vector2.Y;

        return newVector;
    }

    MUI_Vector2 operator-(MUI_Vector2 vector2)
    {
        MUI_Vector2 newVector = *this;

        newVector.X -= vector2.X;
        newVector.Y -= vector2.Y;

        return newVector;
    }

    MUI_Vector2 operator/(MUI_Vector2 vector2)
    {
        MUI_Vector2 newVector = *this;

        newVector.X /= vector2.X;
        newVector.Y /= vector2.Y;

        return newVector;
    }

    MUI_Vector2 operator*(MUI_Vector2 vector2)
    {
        MUI_Vector2 newVector = *this;

        newVector.X *= vector2.X;
        newVector.Y *= vector2.Y;

        return newVector;
    }

    MUI_Vector2 operator/(float number)
    {
        MUI_Vector2 newVector = *this;

        newVector.X /= number;
        newVector.Y /= number;

        return newVector;
    }


    MUI_Vector2 operator*(float number)
    {
        MUI_Vector2 newVector = *this;

        newVector.X *= number;
        newVector.Y *= number;

        return newVector;
    }

    float Magnitude()
    {
        return SDL_sqrt(SDL_pow(this->X, 2) + SDL_pow(this->X, 2));
    }

    MUI_Vector2 ToScale(MUI_Vector2 windowSize)
    {
        MUI_Vector2 newVector2 = *this;

        newVector2.X /= windowSize.X;
        newVector2.Y /= windowSize.Y;

        return newVector2;
    }

    MUI_Vector2 ToOffset(MUI_Vector2 parentSize)
    {
        MUI_Vector2 newVector2 = *this;

        newVector2.X *= parentSize.X;
        newVector2.Y *= parentSize.Y;

        return newVector2;
    }

    MUI_Vector2 Normalize()
    {
        return MUI_Vector2((this->X != 0.0f) ? this->X / this->Magnitude() : 0.0f, (this->Y != 0.0f) ? this->Y / this->Magnitude() : 0.0f);
    }

    MUI_Vector2(float X, float Y)
    {
        this->X = X;
        this->Y = Y;
    }

    MUI_Vector2()
    {
        this->X = 0.0f;
        this->Y = 0.0f;
    }
};

class MUI_Element
{
public:
    SDL_Color    backgroundColor;
    SDL_Texture *texture;
    SDL_Rect     destRect;
    SDL_Rect    *srcRect;

    int scaling;
    int scaleTo;

    bool visible;

    bool draggable;
    bool clickable;

    bool mouseDown;

    std::vector<MUI_Element*> childs;
    MUI_Element* parent = nullptr;

    std::function<void()> Clicked;
    std::function<void()> Hovered;

    MUI_Vector2 position;
    MUI_Vector2 size;

} typedef MUI_Element;

class MUI_Updater
{
public:
    int mouseX;
    int mouseY;

    int windowSizeX;
    int windowSizeY;

    std::vector<MUI_Element*> elements;
    MUI_Element* clickedElement;
    MUI_Element* hoveredElement;
    MUI_Element* draggedElement;

    int event;

    MUI_Updater(SDL_Window *window)
    {
        SDL_GetWindowSize(window, &this->windowSizeX, &this->windowSizeY);
        SDL_GetMouseState(&this->mouseX, &this->mouseY);

        this->clickedElement = nullptr;
        this->hoveredElement = nullptr;
        this->draggedElement = nullptr;
    }

    MUI_Updater()
    {
        this->clickedElement = nullptr;
        this->hoveredElement = nullptr;
        this->draggedElement = nullptr;
    }
};

class MUI_Text
{
public:
    std::string text;

    MUI_Element element;
} typedef MUI_Text;

void MUI_ElementSetParent(MUI_Element *element, MUI_Element *parent)
{
    if (element->parent != parent)
    {
        if (element->parent != nullptr)
            element->parent->childs.erase(std::find(element->parent->childs.begin(), element->parent->childs.end(), element));

        element->parent = parent;

        if (parent != nullptr)
            parent->childs.push_back(element);
    }
}

MUI_Updater *MUI_CreateUpdater(SDL_Window *window)
{
    MUI_Updater *updater = new MUI_Updater(window);

    return updater;
}

MUI_Element *MUI_CreateFrame(SDL_Renderer *renderer, SDL_Color backgroundColor, MUI_Vector2 position, MUI_Vector2 size, int scaling, int scaleTo, bool clickable, bool draggable)
{
    MUI_Element *element = new MUI_Element;

    element->draggable       = draggable;
    element->clickable       = clickable;
    element->backgroundColor = backgroundColor;
    element->texture         = nullptr;
    element->position        = position;
    element->size            = size;
    element->scaling         = scaling;
    element->scaleTo         = scaleTo;
    element->visible         = true;

    element->mouseDown       = false;

    element->Hovered         = nullptr;
    element->Clicked         = nullptr;

    return element;
}

MUI_Element *MUI_CreateText(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color textColor, SDL_Color backgroundColor, MUI_Vector2 position, MUI_Vector2 size, int scaling, int scaleTo, bool clickable, bool draggable)
{
    MUI_Element *element = new MUI_Element;

    SDL_Surface *surface =  TTF_RenderText_Blended(font, text, textColor);

    if (surface ==  NULL)
        std::cout << TTF_GetError() << std::endl;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture ==  NULL)
        std::cout << SDL_GetError() << std::endl;

    element->srcRect         = &element->destRect;

    element->draggable       = draggable;
    element->clickable       = clickable;
    element->backgroundColor = backgroundColor;
    element->texture         = texture;
    element->position        = position;
    element->size            = size;
    element->scaling         = scaling;
    element->scaleTo         = scaleTo;
    element->visible         = true;

    element->mouseDown       = false;

    element->Hovered         = nullptr;
    element->Clicked         = nullptr;

    SDL_FreeSurface(surface);

    return element;
}

void MUI_UpdateText(SDL_Renderer *renderer, MUI_Element *element, const char *text, TTF_Font *font, SDL_Color textColor)
{
    SDL_Surface *surface =  TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_DestroyTexture(element->texture);
    SDL_FreeSurface(surface);

    element->texture = texture;
}

void MUI_ElementCopy(MUI_Element *copyFrom, MUI_Element *copyTo)
{
    copyTo->draggable       = copyFrom->draggable;
    copyTo->clickable       = copyFrom->clickable;
    copyTo->backgroundColor = copyFrom->backgroundColor;
    copyTo->texture         = copyFrom->texture;
    copyTo->position        = copyFrom->position;
    copyTo->size            = copyFrom->size;
    copyTo->scaling         = copyFrom->scaling;
    copyTo->scaleTo         = copyFrom->scaleTo;
    copyTo->Hovered         = copyFrom->Hovered;
    copyTo->Clicked         = copyFrom->Clicked;
    copyTo->mouseDown       = copyFrom->mouseDown;
}

void MUI_UpdateCopy(MUI_Updater *muiUpdater, MUI_Element *element)
{
    muiUpdater->elements.push_back(element);
}

void MUI_UpdateClear(MUI_Updater *muiUpdater)
{
    muiUpdater->elements = {};
}

bool mouseUp = false;
bool mouseDown = false;
bool dragged = false;

constexpr void MUI_ElementUpdatedestRect(MUI_Element *element, MUI_Updater *updater)
{
    switch (element->scaling)
    {
    case MUI_SCALING_SCALE:
        if (element->parent == nullptr)
        {
            element->destRect.x = element->position.X * updater->windowSizeX;
            element->destRect.y = element->position.Y * updater->windowSizeY;
            element->destRect.w = element->size.X     * updater->windowSizeX;
            element->destRect.h = element->size.Y     * updater->windowSizeY;
        }
        else
        {
            element->destRect.x = element->position.X * element->parent->destRect.w;
            element->destRect.y = element->position.Y * element->parent->destRect.h;
            element->destRect.w = element->size.X     * element->parent->destRect.w;
            element->destRect.h = element->size.Y     * element->parent->destRect.h;
        }
        break;
    case MUI_SCALING_OFFSET:
        element->destRect.x = element->position.X;
        element->destRect.y = element->position.Y;
        element->destRect.w = element->size.X;
        element->destRect.h = element->size.Y;
        break;
    }

    if (element->parent != nullptr)
    {
        element->destRect.x += element->parent->destRect.x;
        element->destRect.y += element->parent->destRect.y;
    }

    switch (element->scaleTo)
    {
    case MUI_SCALE_XX:
        element->destRect.h = element->destRect.w;
        break;
    case MUI_SCALE_YY:
        element->destRect.w = element->destRect.h;
        break;
    }
}

constexpr void MUI_UpdaterChangeEvent(MUI_Updater *updater, MUI_Element *element, int muiEvent_)
{
    switch (muiEvent_)
    {
    case MUI_CLICKED:
        updater->clickedElement = element;
        updater->hoveredElement = nullptr;
        updater->draggedElement = nullptr;
        updater->event = MUI_CLICKED;
        break;
    case MUI_DRAGGED:
        updater->draggedElement = element;
        updater->hoveredElement = nullptr;
        updater->clickedElement = nullptr;
        updater->event = MUI_DRAGGED;
        break;
    case MUI_HOVERED:
        updater->hoveredElement = element;
        updater->draggedElement = nullptr;
        updater->clickedElement = nullptr;
        updater->event = MUI_HOVERED;
        break;
    default:
        updater->hoveredElement = nullptr;
        updater->draggedElement = nullptr;
        updater->clickedElement = nullptr;
        updater->event = MUI_NOEVENT;
        break;
    }
}

void MUI_ElementCheckEvent(MUI_Updater *updater, MUI_Element *element)
{
    if (element->visible)
    {
        if (updater->event == MUI_NOEVENT)
        {
            bool collision      = (updater->mouseX > element->destRect.x)           && (updater->mouseX < (element->destRect.x + element->destRect.w))
                                    && (updater->mouseY > element->destRect.y)      && (updater->mouseY < (element->destRect.y + element->destRect.h));
            bool collisionHover = (updater->mouseX > element->destRect.x)           && (updater->mouseX < (element->destRect.x + element->destRect.w))
                                    && (updater->mouseY > element->destRect.y - 10) && (updater->mouseY < (element->destRect.y));

            if ((updater->draggedElement == element) || (collisionHover && element->draggable == true && updater->draggedElement == nullptr))
            {
                MUI_UpdaterChangeEvent(updater, element, MUI_HOVERED);

                if (mouseDown)
                {
                    MUI_UpdaterChangeEvent(updater, element, MUI_DRAGGED);

                    MUI_Vector2 windowSize = MUI_Vector2(updater->windowSizeX, updater->windowSizeY);
                    MUI_Vector2 mousePosition = MUI_Vector2(updater->mouseX, updater->mouseY);

                    if (element->scaling == MUI_SCALING_SCALE)
                        if (element->parent != nullptr)
                            element->position = (mousePosition - MUI_Vector2((float)element->destRect.w / 2.0f, -5.0f)).ToScale(MUI_Vector2(element->parent->destRect.w, element->parent->destRect.h));
                        else
                            element->position = (mousePosition - MUI_Vector2(element->size.ToOffset(windowSize).X / 2.0f, -5.0f)).ToScale(windowSize);

                        
                    else if (element->scaling == MUI_SCALING_OFFSET)
                        element->position = mousePosition - MUI_Vector2(element->size.X / 2.0f, 5.0f);
                    
                    if (element->parent != nullptr)
                        element->position = element->position - ((element->scaling == MUI_SCALING_SCALE)
                                            ? MUI_Vector2(element->parent->destRect.x, element->parent->destRect.y).ToScale(MUI_Vector2(element->parent->destRect.w, element->parent->destRect.h))
                                            : MUI_Vector2(element->parent->destRect.x, element->parent->destRect.y));
                    
                }
            }
            else if (collision && updater->draggedElement == nullptr)
            {

                MUI_UpdaterChangeEvent(updater, element, MUI_HOVERED);

                if (element->mouseDown == false && mouseDown == true && element->clickable == true)
                {
                    element->backgroundColor.r /= (Uint8)2;
                    element->backgroundColor.g /= (Uint8)2;
                    element->backgroundColor.b /= (Uint8)2;
                    element->mouseDown = true;
                }
                else if (element->mouseDown == true && mouseDown == false)
                {
                    element->backgroundColor.r *= (Uint8)2;
                    element->backgroundColor.g *= (Uint8)2;
                    element->backgroundColor.b *= (Uint8)2;
                    element->mouseDown = false;
                }

                if (mouseUp && updater->hoveredElement == element && updater->draggedElement == nullptr && updater->clickedElement == nullptr && element->clickable == true)
                {
                    MUI_UpdaterChangeEvent(updater, element, MUI_CLICKED);

                    if (element->Clicked != nullptr)
                        element->Clicked();
                }
            }
            else if (element->mouseDown == true)
            {
                element->backgroundColor.r *= (Uint8)2;
                element->backgroundColor.g *= (Uint8)2;
                element->backgroundColor.b *= (Uint8)2;
                element->mouseDown = false;
            }

            if (mouseUp && updater->draggedElement != nullptr)
                MUI_UpdaterChangeEvent(updater, nullptr, MUI_NOEVENT);
                
        }
        else if (element->mouseDown == true)
        {
            element->backgroundColor.r *= (Uint8)2;
            element->backgroundColor.g *= (Uint8)2;
            element->backgroundColor.b *= (Uint8)2;
            element->mouseDown = false;
        }
    }
}

std::vector<MUI_Element*> MUI_ElementEventVector;

void MUI_ElementEventUpdate(MUI_Updater *updater)
{
    for (int i = MUI_ElementEventVector.size() - 1; i >= 0; i--)
        MUI_ElementCheckEvent(updater, MUI_ElementEventVector[i]);
}

void MUI_RecursiveCopy(SDL_Renderer *renderer, MUI_Updater *updater, std::vector<MUI_Element*> elements)
{

    for (int64_t i = 0;  i < elements.size();  i++)
    {
        MUI_Element *element = elements[i];
        if (element->visible)
        {
            SDL_SetRenderDrawColor(renderer, element->backgroundColor.r, element->backgroundColor.g, element->backgroundColor.b, element->backgroundColor.a);
            SDL_RenderFillRect(renderer, &element->destRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            
            if (element->srcRect != &element->destRect)
                SDL_RenderCopy(renderer, element->texture, NULL, &element->destRect);
            else
            {
                int textureW;
                int textureH;

                SDL_QueryTexture(element->texture, nullptr, nullptr, &textureW, &textureH);

                SDL_Rect destRect = element->destRect;
                //srcRect.w = (int)(((float)element->destRect.h / (float)textureH + (float)element->destRect.w / (float)textureW) * (float)textureW);
                //srcRect.h = (int)(((float)element->destRect.h / (float)textureH + (float)element->destRect.w / (float)textureW) * (float)textureH);

                float ratio = (float)textureW / (float)textureH;

                float clampedW = SDL_clamp(textureW, 1, element->destRect.w);
                float clampedH = SDL_clamp(textureH, 1, element->destRect.h);

                float diffW = ((float)textureW - clampedW);
                float diffH = ((float)textureH - clampedH);

                float diffA = (float)(diffW + diffH) / 16.0f;

                float newWidth = clampedW - diffA;
                float newHeight = clampedH - diffA;

                // SDL RECT CAUSES PIXELATED TEXT //

                if (newHeight * ratio > element->destRect.w)
                {
                    newWidth = SDL_roundf((float)(SDL_roundf(newWidth)) / 10.0f) * 10.0f;
                    destRect.w = SDL_roundf(newWidth);
                    destRect.h = SDL_roundf((float)newWidth / ratio);
                }
                else
                {
                    newHeight = SDL_roundf((float)(SDL_roundf(newHeight)) / 10.0f) * 10.0f;
                    destRect.h = SDL_roundf(newHeight);
                    destRect.w = SDL_roundf((float)newHeight * ratio);
                }

                float newRatio = (float)destRect.w / (float)destRect.h;

                //newWidth = ratio * (int)((float)destRect.w / newRatio);
                //newHeight = (1.0f / ratio) * (int)((float)destRect.h / (1.0f / newRatio));

                //std::cout << "TEXTURE: " << textureW << ", " << textureH << " "
                //          << "RECT:"     << newWidth << ", " << newHeight
                // << std::endl;

                //destRect.w = newWidth;
                //destRect.h = newHeight;

                newRatio = (float)destRect.w / (float)destRect.h;

                //std::cout << ratio << ", " << newRatio << std::endl;

                //destRect.w = textureW;
                //destRect.h = textureH;

                destRect.x += SDL_floorf((element->destRect.w - destRect.w) / 2.0f);
                destRect.y += SDL_floorf((element->destRect.h - destRect.h) / 2.0f);

                SDL_RenderCopy(renderer, element->texture, nullptr, &destRect);
            }

            if (element->draggable)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, element->backgroundColor.a);
                
                SDL_Rect rect = element->destRect;
                rect.y -= 10;
                rect.h  = 10;
                
                SDL_RenderFillRect(renderer, &rect);
            }

            MUI_ElementUpdatedestRect(element, updater);

            MUI_ElementEventVector.push_back(elements[i]);

            if (element->childs.size() > 0)
                MUI_RecursiveCopy(renderer, updater, element->childs);
            
        }
    }
    
}

void MUI_Update(MUI_Updater *updater, SDL_Renderer *renderer, SDL_Event event)
{
    MUI_ElementEventVector = {};
    updater->event = MUI_NOEVENT;
    updater->clickedElement = nullptr;
    updater->hoveredElement = nullptr;
    
    mouseUp = false;
    dragged = false;
    

    switch (event.type)
    {
    case SDL_MOUSEBUTTONUP:
        mouseUp   = true;
        mouseDown = false;
        break;
    case SDL_MOUSEBUTTONDOWN:
        mouseDown = true;
        mouseUp   = false;
        break;
    case SDL_MOUSEMOTION:
        SDL_GetMouseState(&updater->mouseX, &updater->mouseY);
        break;

    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            SDL_GetWindowSize(SDL_RenderGetWindow(renderer), &updater->windowSizeX, &updater->windowSizeY);
        break;
    
    default:
        break;
    }


    MUI_RecursiveCopy(renderer, updater, updater->elements);

    MUI_ElementEventUpdate(updater);
}

int MUI_Init(uint32_t flags)
{
    int sdlInit = 0;
    int ttfInit = 0;

    if (!SDL_WasInit(flags))
        sdlInit = SDL_Init(flags);

    if (TTF_WasInit() == 0)
        ttfInit = TTF_Init();

    if (sdlInit == -1)
        std::cout << "MUI FAILED INITILIAZING SDL." << std::endl;
    if (ttfInit == -1)
        std::cout << "MUI FAILED INITILIAZING TTF." << std::endl;

    return ttfInit && sdlInit;
}