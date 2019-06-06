#include <utility>

#include <Rendering/Font.h>
#include <GameManager.h>
#include <Parsers/Gm1File.h>

using namespace Sourcehold;
using namespace Rendering;
using namespace Game;

static std::shared_ptr<GameManager> _mgr;
static std::shared_ptr<Gm1File> _fonts[3];
static std::pair<uint8_t, uint8_t> _table_width_height[3] = {
    { 15, 18 }, /* Slanted */
    {  7, 17 }, /* Small */
    { 19, 32 }, /* Large */
};

bool Rendering::LoadFonts(std::shared_ptr<GameManager> mgr) {
    _mgr = mgr;

    _fonts[0] = mgr->GetGm1(mgr->GetDirectory() / "gm/font_slanted.gm1").lock();
    _fonts[1] = mgr->GetGm1(mgr->GetDirectory() / "gm/font_stronghold.gm1").lock();
    _fonts[2] = mgr->GetGm1(mgr->GetDirectory() / "gm/font_stronghold_aa.gm1").lock();

    return true;
}

void Rendering::UnloadFonts() {
    _mgr.reset();
    _fonts[0].reset();
    _fonts[1].reset();
    _fonts[2].reset();
}

void Rendering::RenderText(const std::wstring& text, int32_t x, int32_t y, double scaleFactor, Font type, bool illumination) {
    auto font = _fonts[type];
    if (illumination && text.size() != 1) {
        return;
    }

    SDL_Rect glyph;
    for(wchar_t c : text) {
        if(c < 0x20 || (c-0x20) > font->GetTextureAtlas().lock()->GetNumTextures()) continue;
        /* Space */
        if(c == 0x20) {
            x += _table_width_height[type].first;
        }else {
            int8_t lowercaseOffset = 0;
            if(c == 0x67 || c == 0x70 || c == 0x71 || c == 0x79) lowercaseOffset = _table_width_height[type].second / 2 - (type == FONT_SMALL ? 1 : 5); /*g p q y*/
            if(c == 0x6A) lowercaseOffset = _table_width_height[type].second / 2; /*j*/
            if(c == 0x2D) lowercaseOffset = FONT_SMALL ? -8 : -12;
            if(c == 0x27) lowercaseOffset = FONT_SMALL ? -12 : -17;
            glyph = font->GetTextureAtlas().lock()->Get(c - 0x21);
            _mgr->Render(*font->GetTextureAtlas().lock(),
                         illumination ? (x + (13 - glyph.w)/2) : x,
                         y + _table_width_height[type].second - glyph.h + lowercaseOffset,
                         glyph.w,
                         glyph.h,
                         &glyph);

            x += glyph.w +1;
        }
    }
}

void Rendering::RenderText(const std::wstring& text, double x, double y, double scaleFactor, Font type, bool illumination) {
    auto font = _fonts[type];
    if (illumination && text.size() != 1) {
        return;
    }

    SDL_Rect glyph;
    for(wchar_t c : text) {
        if(c < 0x20 || (c-0x20) > font->GetTextureAtlas().lock()->GetNumTextures()) continue;
        /* Space */
        if(c == 0x20) {
            x += _mgr->NormalizeX(_table_width_height[type].first);
        }else {
            glyph = font->GetTextureAtlas().lock()->Get(c - 0x21);
            /* TODO possibly a table of some sort*/
            int8_t lowercaseOffset = 0;
            if(c == 0x67 || c == 0x70 || c == 0x71 || c == 0x79) lowercaseOffset = _table_width_height[type].second / 2 - (type == FONT_SMALL ? 1 : 2); /* g p q y */
            if(c == 0x6A) lowercaseOffset = _table_width_height[type].second / 2; /* j */
            if(c == 0x2D) lowercaseOffset = FONT_SMALL ? -8 : -12; /* - */
            if(c == 0x27) lowercaseOffset = FONT_SMALL ? -12 : -17; /* ' */
            _mgr->Render(*font->GetTextureAtlas().lock(),
                         illumination ? (x + _mgr->NormalizeX((13 - glyph.w)/2)) : x,
                         double(_mgr->NormalizeY(_mgr->ToCoordY(y) + _table_width_height[type].second - glyph.h + lowercaseOffset)),
                         double(_mgr->NormalizeX(glyph.w)),
                         double(_mgr->NormalizeY(glyph.h)),
                         &glyph);

            x += _mgr->NormalizeX(glyph.w+1);
        }
    }
}
