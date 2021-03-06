#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "Visualization.h"

void Visualization::SetTileSize(double width, double height) 
{
    m_tile_width = width;
    m_tile_height = height;
}

void Visualization::LoadTexture(const std::string & filename) 
{
    std::cout << "Ładowanie obrazka z pliku " + filename + "\n";

// załaduj dane z pliku
    SDL_Surface* surface = SDL_LoadBMP(filename.c_str());
    if (!surface) 
    {
        std::cerr << "Ładowanie " + filename + " FAILED: " + SDL_GetError() + "\n";
        exit(1);
    }

//sprawdzanie poprawności wymiarów
    const int width = surface->w;
    const int height = surface->h;
    if (((width & (width - 1)) != 0) || ((height & (height - 1)) != 0)) 
    {
        std::cerr << "Obrazek " + filename + " ma nieprawidłowe wymiary: " << width << "x"
                << height << "\n";
        exit(1);
    }

    GLenum format;
    switch (surface->format->BytesPerPixel) 
    {
    case 3:
        format = GL_BGR;
        break;
    case 4:
        format = GL_BGRA;
        break;
    default:
        std::cerr << "Nieznany format pliku " + filename + "\n";
        exit(1);
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, width, height,
                 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    if (surface) {
        SDL_FreeSurface(surface);
    }

}


void Visualization::DrawSprite(double tex_x, double tex_y, double tex_w, double tex_h,
                          double pos_x, double pos_y, double width, double height, 
                          DL::DisplayLayer layer) 
{
    const double texture_w = 1024.0;
    const double texture_h = 1024.0;

    const double left = tex_x / texture_w;
    const double right = left + tex_w / texture_w;
    const double bottom = tex_y / texture_h;
    const double top = bottom - tex_h / texture_h;

    glColor3f(1, 1, 1);
    glPushMatrix();
    {
        glTranslatef(0, 0, -static_cast<double> (layer));
        glBegin(GL_QUADS);
        {
            glTexCoord2f(right, top);    glVertex2f(pos_x + width, pos_y + height);
            glTexCoord2f(left, top);     glVertex2f(pos_x, pos_y + height);
            glTexCoord2f(left, bottom);  glVertex2f(pos_x, pos_y);
            glTexCoord2f(right, bottom); glVertex2f(pos_x + width, pos_y);
        }
        glEnd();
    }
    glPopMatrix();
}

void Visualization::DrawAabb(const Aabb& box, double r, double g, double b, double a) const 
{
    const double tw = m_tile_width;
    const double th = m_tile_height;

    DrawQuad(box.GetMinX()*tw,box.GetMinY()*tw,box.GetMaxX()*th,box.GetMaxY()*th, r, g, b, a);
}

void Visualization::DrawQuad(double min_x, double min_y,double max_x, double max_y,
                        double r, double g, double b, double a) const 
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    {
        glVertex2f(min_x, min_y);
        glVertex2f(max_x, min_y);
        glVertex2f(max_x, max_y);
        glVertex2f(min_x, max_y);
    }
    glEnd();

    glPopAttrib();
}

void Visualization::SetProjection(size_t width, size_t height) 
{
    glViewport(0, 0, static_cast<GLsizei> (width), static_cast<GLsizei> (height));
    ResetProjection();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Visualization::ResetProjection() 
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 10);
}
