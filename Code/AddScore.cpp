#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <vector> 
#include <string>
#include <fstream>
#include <iostream>

#include "AddScore.h"
#include "Text.h"
#include "Engine.h"
#include "WallOfGlory.h"
#include "Convert.h"


AddScore::AddScore(size_t points) : m_is_done(false), m_player_nickname("__________"),
                                    m_next_letter(0), m_points(points), 
                                    m_highlighted_char(' ') 
{
}

void AddScore::Draw() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    Text t(0.08, 0.08);
    t.DrawText("Gratuluje", 0.1, 0.9);
    t.SetSize(0.04, 0.04);
    t.DrawText("punkty: " + IntToStr(m_points), 0.1, 0.8);
    t.SetSize(0.05, 0.05);
    t.DrawText("podaj swoje imie", 0.1, 0.7);
    t.DrawText(m_player_nickname, 0.25, 0.6);
    
    t.SetSize(0.05, 0.05);

    for (char ch = 'a'; ch <= 'z'; ++ch) 
    {
        std::pair<double, double> pos = LetterPosition(ch);
        if (ch == m_highlighted_char) 
        {
            Engine::Get().GetVisualization()->DrawQuad(pos.first - 0.005, pos.second - 0.005, 
                                               pos.first + 0.055, pos.second + 0.055, 
                                               1,0,0,1);
        }
        t.DrawLetter(ch, pos.first, pos.second);
    }

    SDL_GL_SwapBuffers();
}

std::pair<double, double> AddScore::LetterPosition(char ch) 
{
    int index = ch - 'a';
    int col = index % 7;
    int row = index / 7;

    return std::make_pair(0.25 + col * 0.07,0.45 - row * 0.07);
}

bool AddScore::Update(double /* dt */) 
{
    return !IsDone();
}

void AddScore::ProcessEvents(const SDL_Event& event) 
{
    if (event.type == SDL_QUIT) 
    {
        SetDone();
    } else 
    if (event.type == SDL_KEYDOWN) 
    {
        if (event.key.keysym.sym == SDLK_RETURN && m_player_nickname.at(0) != '_') 
        {
            StoreInFile();
            SetDone();
        }
        else 
        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z && m_next_letter < m_player_nickname.size()) 
        { 
            char key = event.key.keysym.sym - SDLK_a + 'a';
            m_player_nickname.at(m_next_letter++) = key;
        }
        else 
        if (event.key.keysym.sym == SDLK_BACKSPACE) 
        {
            if (m_player_nickname.at(0) != '_') 
            {
                m_player_nickname.at(--m_next_letter) = '_';
            }
        }
    }
    else 
    if (event.type == SDL_MOUSEMOTION) 
    {
        double x = event.motion.x / 600.0;
        double y = 1.0 - event.motion.y / 400.0;
        m_highlighted_char = ' ';
        for (char ch = 'a'; ch <= 'z'; ++ch) 
        {
            std::pair<double, double> ch_pos = LetterPosition(ch);
            if (x >= ch_pos.first && x <= ch_pos.first + 0.07
                && y <= ch_pos.second + 0.07 && y >= ch_pos.second) 
            {
                m_highlighted_char = ch;
            }
        }
    }
    else 
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (m_highlighted_char != ' ' && m_next_letter < m_player_nickname.size()) 
        {
            m_player_nickname.at(m_next_letter++) = m_highlighted_char;
        }
    }
}

void AddScore::StoreInFile() 
{
    std::fstream hof("data/ranking.txt");
    if (!hof) 
    {
        std::cerr << "Nie moge odczytac Wall of Glory\n";
        return;
    }

    std::vector<Entry> entries;
    Entry e;
    while (hof >> e.name >> e.points) 
    {
        entries.push_back(e);
    }

    Entry new_e;
    for (size_t i = 0; i < m_player_nickname.size(); ++i) 
    {
        if (m_player_nickname.at(i) != '_') 
        {
            new_e.name += m_player_nickname.at(i);
        }
    }
    new_e.points = m_points;
    entries.push_back(new_e);

    int j = entries.size() - 1;
    while (j > 0) 
    {
        if (entries.at(j-1).points < entries.at(j).points) 
        {
            std::swap(entries.at(j-1), entries.at(j));
        }
        --j;
    }

    hof.close();

    hof.clear();
    hof.open("data/ranking.txt", std::ios::out);
    for (size_t i = 0; i < 10 && i < entries.size(); ++i) 
    {
        hof << entries.at(i).name << " " << entries.at(i).points << "\n";
    }
    hof.close();
}

void AddScore::Initialization() 
{
}

void AddScore::Start() 
{
}

ApplicationStatePtr AddScore::NextApplicationState() const 
{
    return ApplicationStatePtr(new WallOfGlory);
}
