#include <cctype>
#include <string> 
#include <sstream>
#include <algorithm>
#include <iostream>

#include "Convert.h"
#include "Engine.h"
#include "Text.h"

void Text::DrawDigit(char ch, double pos_x, double pos_y) 
{
    int digit = ch - '0';
    int tex_x = digit * 32;
    int tex_y = 7*32;
    Draw(tex_x, tex_y, pos_x, pos_y);
}

void Text::DrawLetter(char ch, double pos_x, double pos_y) 
{
    int letter = toupper(ch) - 'A';
    
    int letter_row = letter / 10; // wiersz, w kt�rym jest litera
    int letter_col = letter % 10; // kolumna, w kt�rym jest litera

    int tex_x = letter_col * 32;
    int tex_y = (8+letter_row) * 32;

    Draw(tex_x, tex_y, pos_x, pos_y);
}

void Text::DrawSpecial(char ch, double pos_x, double pos_y) 
{
    double tex_x = 0;
    double tex_y = 0;

    if (ch == '_') {
        tex_x = 192;
        tex_y = 320;
    }
    else {
        return; // pomijamy znaki, kt�rych nie znamy
    }
    Draw(tex_x, tex_y, pos_x, pos_y);
}

void Text::DrawText(const std::string& text, double pos_x, double pos_y) 
{
    double x = pos_x;
    double y = pos_y;

    for (size_t i = 0; i < text.size(); ++i) 
    {
        char ch = text.at(i);
        if (isdigit(ch)) 
        {
            DrawDigit(ch, x, y);
        }
        else 
        if (isalpha(ch)) 
        {
            DrawLetter(ch, x, y);
        }
        else 
        if (ch == '_') 
        {
            DrawSpecial(ch, x, y);
        }
        else 
        {
            ; // inne znaki pomijamy
        }
        x += m_width;
    }
}

void Text::DrawNumber(size_t number, double pos_x, double pos_y, size_t width) 
{
    std::string number_str = IntToStr(number);
    size_t spaces_count = std::max(0, static_cast<int> (width) - static_cast<int> (number_str.size()));
    for (size_t i = 0; i < spaces_count; ++i)
        number_str = " " + number_str;
    DrawText(number_str, pos_x, pos_y);
}

void Text::Draw(int tex_x, int tex_y, double pos_x, double pos_y) 
{
    glPushMatrix();
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        {
            Engine::Get().GetVisualization()->ResetProjection();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            Engine::Get().GetVisualization()->DrawSprite(tex_x, tex_y, 32, 32, 
                                                 pos_x, pos_y, m_width, m_height,
                                                 DL::DisplayLayer(m_layer));
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
