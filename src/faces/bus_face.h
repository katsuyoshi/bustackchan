#ifndef BUS_FACE
#define BUS_FACE


#include <Face.h>
#include <Avatar.h>
#include "BoundingRect.h"
#include "DrawContext.h"
#include "Drawable.h"
#include "lgfx/Fonts/efont/lgfx_efont_ja.h"

using namespace m5avatar;

class BusMouth : public Drawable {
 private:
  uint16_t minWidth;
  uint16_t maxWidth;
  uint16_t minHeight;
  uint16_t maxHeight;

 public:
  BusMouth() : BusMouth(90, 90, 20, 20) {}
  BusMouth(uint16_t minWidth, uint16_t maxWidth, uint16_t minHeight,
           uint16_t maxHeight)
      : minWidth{minWidth},
        maxWidth{maxWidth},
        minHeight{minHeight},
        maxHeight{maxHeight} {}

  void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
    //uint32_t primaryColor = ctx->getColorDepth() == 1 ? 1 : ctx->getColorPalette()->get(COLOR_PRIMARY);
    //uint32_t secondaryColor = ctx->getColorDepth() == 1 ? 1 : ctx->getColorPalette()->get(COLOR_SECONDARY);
    uint32_t primaryColor = ctx->getColorPalette()->get(COLOR_PRIMARY);
    uint32_t secondaryColor = ctx->getColorPalette()->get(COLOR_SECONDARY);
    uint32_t backgroundColor = ctx->getColorPalette()->get(COLOR_BACKGROUND);
    float breath = _min(1.0f, ctx->getBreath());
    float openRatio = ctx->getMouthOpenRatio();
    int h = minHeight + (maxHeight - minHeight) * openRatio;
    int w = minWidth + (maxWidth - minWidth) * (1 - openRatio);
    int x = rect.getLeft() - w / 2;
    int y = rect.getTop() - h / 2 + breath * 2;
    spi->fillRect(x, y, w, h, primaryColor);

    if (openRatio == 1.0) {
      spi->setCursor(x + 4, y + 4);                        // 座標を指定（x, y）
      spi->setTextSize(2.0);            // 文字倍率変更
      spi->setTextColor(secondaryColor, primaryColor);
      spi->println("BUSTACK");                // 表示内容をcanvasに準備
    }


    // 窓枠
    spi->fillRect(1, 40, 320 - 2, 120, primaryColor);

    // 行き先表示
    spi->setCursor(0, 4);                        // 座標を指定（x, y）
    spi->setFont(&fonts::lgfxJapanGothic_12);
    spi->setTextDatum( textdatum_t::top_center      );
    spi->setTextSize(2.0);            // 文字倍率変更
    spi->setTextColor(primaryColor, backgroundColor);
    spi->println(heading_title);                // 表示内容をcanvasに準備
  }

  void set_heading_title(const char *title) {
    heading_title = title;
  }

  private:
    const char *heading_title = "　　　バスタックちゃん　　";

};

class BusEye : public Drawable {
 private:
  uint16_t width;
  uint16_t height;
  uint16_t r;
  bool isLeft;

 public:
  // constructor
  BusEye(uint16_t width, uint16_t height, uint16_t r, bool isLeft)
        : width{width},
          height{height},
          r{r},
          isLeft{isLeft} {}
  //~Eye() = default;
  //Eye(const Eye &other) = default;
  //Eye &operator=(const Eye &other) = default;

  void draw(M5Canvas *spi, BoundingRect rect,
            DrawContext *ctx) {
    uint32_t primaryColor = ctx->getColorPalette()->get(COLOR_PRIMARY);
    rect.setSize(width, height);
    int x = rect.getLeft();
    int y = rect.getTop();
    int w = rect.getWidth();
    int h = rect.getHeight();
    //spi->fillRect(x, y, w, h, primaryColor);
    spi->drawRect(x, y, w, h, primaryColor);

    {
      Expression exp = ctx->getExpression();
      uint32_t x = rect.getCenterX();
      uint32_t y = rect.getCenterY();
      Gaze g = ctx->getGaze();
      float openRatio = ctx->getEyeOpenRatio();
      uint32_t offsetX = g.getHorizontal() * 3 + (isLeft ? - 8 : 8);
      uint32_t offsetY = g.getVertical() * 3;
      uint32_t primaryColor = ctx->getColorPalette()->get(COLOR_PRIMARY);
      uint32_t backgroundColor = ctx->getColorDepth() == 1 ? 0 : ctx->getColorPalette()->get(COLOR_BACKGROUND);

      if (openRatio > 0) {
        spi->fillCircle(x + offsetX, y + offsetY, r, primaryColor);
        // TODO(meganetaaan): Refactor
        if (exp == Expression::Angry || exp == Expression::Sad) {
          int x0, y0, x1, y1, x2, y2;
          x0 = x + offsetX - r;
          y0 = y + offsetY - r;
          x1 = x0 + r * 2;
          y1 = y0;
          x2 = !isLeft != !(exp == Expression::Sad) ? x0 : x1;
          y2 = y0 + r;
          spi->fillTriangle(x0, y0, x1, y1, x2, y2, backgroundColor);
        }
        if (exp == Expression::Happy || exp == Expression::Sleepy) {
          int x0, y0, w, h;
          x0 = x + offsetX - r;
          y0 = y + offsetY - r;
          w = r * 2 + 4;
          h = r + 2;
          if (exp == Expression::Happy) {
            y0 += r;
            spi->fillCircle(x + offsetX, y + offsetY, r / 1.5, backgroundColor);
          }
          spi->fillRect(x0, y0, w, h, backgroundColor);
        }
      } else {
        int x1 = x - r + offsetX;
        int y1 = y - 2 + offsetY;
        int w = r * 2;
        int h = 4;
        spi->fillRect(x1, y1, w, h, primaryColor);
      }
    }

  }
};


class BusFace : public Face {
 public:
  BusFace()
      : Face(
          new BusMouth(90, 45, 5, 20), new BoundingRect(220, 163),
           new BusEye(100, 60, 24, false), new BoundingRect(170, 1),
           new BusEye(100, 60, 24, true), new BoundingRect(170, 220),
           new Eyeblow(32, 0, false), new BoundingRect(67, 96),
           new Eyeblow(32, 0, true), new BoundingRect(72, 230)) {}

  void set_heading_title(const char *title) {
    BusMouth *m = (BusMouth *)this->mouth;
    m->set_heading_title(title);
  }

};

#endif
