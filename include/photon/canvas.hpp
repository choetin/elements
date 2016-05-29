/*=================================================================================================   Copyright (c) 2016 Joel de Guzman   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.   http://creativecommons.org/licenses/by-sa/4.0/=================================================================================================*/#if !defined(PHOTON_GUI_LIB_CANVAS_MAY_3_2016)#define PHOTON_GUI_LIB_CANVAS_MAY_3_2016#include <photon/point.hpp>#include <photon/rect.hpp>#include <photon/color.hpp>#include <photon/circle.hpp>#include <photon/exception.hpp>#include <nanovg.h>#include <vector>struct NVGcontext;namespace photon{   using paint = NVGpaint;   using glyph_position = NVGglyphPosition;	////////////////////////////////////////////////////////////////////////////////////////////////	// The Canvas   //   // Abstracts the NanoVG drawing context	////////////////////////////////////////////////////////////////////////////////////////////////   class canvas   {   public:      enum solidity      {         solid = NVG_SOLID,         hole  = NVG_HOLE      };      enum text_align      {         // Horizontal align         align_left     = NVG_ALIGN_LEFT,    // Default, align text horizontally to left.         align_center   = NVG_ALIGN_CENTER,	// Align text horizontally to center.         align_right    = NVG_ALIGN_RIGHT,	// Align text horizontally to right.         // Vertical align         align_top      = NVG_ALIGN_TOP,	   // Align text vertically to top.         align_middle   = NVG_ALIGN_MIDDLE,	// Align text vertically to middle.         align_bottom   = NVG_ALIGN_BOTTOM,	// Align text vertically to bottom.         align_baseline = NVG_ALIGN_BASELINE // Default, align text vertically to baseline.      };      enum winding      {         cw    = NVG_CW,         ccw   = NVG_CCW      };      class image      {      public:         image(canvas& canvas_, char const* filename);         ~image();         image(image&&) = delete;         image(image const&) = delete;         image& operator=(image&&) = delete;         image& operator=(image const&) = delete;         point          size() const;         void           draw(rect bounds);         void           draw(rect bounds, point img_offset, float scale = 1.0);      private:         NVGcontext*    _vg;         int            _handle;      };      canvas(NVGcontext* context = 0)       : _vg(context)      {}      void              context(NVGcontext* vg_) { _vg = vg_; }      void              translate(point p);      void              rotate(float rad);      void              skewx(float rad);      void              skewy(float rad);      void              scale(point p);      point             transform_point(point p);      class state      {      public:         state(canvas& cnv_) : cnv(&cnv_) { cnv->save(); }         state(state&& rhs) : cnv(rhs.cnv) { rhs.cnv = 0; }         ~state() { if (cnv) cnv->restore(); }         state& operator=(state&& rhs)         {            cnv = rhs.cnv;            rhs.cnv = 0;            return *this;         }      private:         state(state const&) = delete;         state& operator=(state const&) = delete;         canvas* cnv;      };      state             new_state();      void              clip(rect r);      void              begin_path();      void              path_winding(solidity dir);      void              rect(struct rect r);      void              round_rect(struct rect r, float radius);      void              circle(struct circle c);      void              arc(struct circle c, float from, float to, winding dir = cw);      void              fill();      void              fill_color(color c);      void              fill_paint(paint p);      paint             box_gradient(                           struct rect r, float radius, float feather                         , color inner_color, color outer_color);      paint             linear_gradient(                           point start, point end                         , color inner_color, color outer_color);      paint             radial_gradient(                           point center, float inr, float outr                         , color inner_color, color outer_color);      void              move_to(point p);      void              line_to(point p);      void              stroke_color(color c);      void              stroke_paint(paint p);      void              stroke_width(float w);      void              stroke();      bool              on_fill(point p);      bool              on_stroke(point p, float tolerance = 4);      void              new_font(char const* name, char const* path);      void              font_size(float size);      void              font_blur(float blur);      void              font_face(char const* font);      void              text_align(int align);      float             text(point p, char const* f, char const* l = 0);      float             text_width(char const* f, char const* l = 0);      float             line_height() const;      void              text_box(point p, float width, char const* f, char const* l = 0);      template <typename F>      void      for_each_line(F line_f, char const* f, char const* l, float width);      std::vector<glyph_position>      glyphs(point p, char const* f, char const* l) const;      friend class image;   private:      void              save();      void              restore();      NVGcontext*       _vg;   };	////////////////////////////////////////////////////////////////////////////////////////////////   // Inlines	////////////////////////////////////////////////////////////////////////////////////////////////   inline NVGcolor nvgRGBA(color c)   {      NVGcolor color;      color.r = c.red;      color.g = c.green;      color.b = c.blue;      color.a = c.alpha;      return color;   }   inline void canvas::translate(point p)   {      nvgTranslate(_vg, p.x, p.y);   }   inline void canvas::rotate(float rad)   {      nvgRotate(_vg, rad);   }   inline void canvas::skewx(float rad)   {      nvgSkewX(_vg, rad);   }   inline void canvas::skewy(float rad)   {      nvgSkewY(_vg, rad);   }   inline void canvas::scale(point p)   {      nvgScale(_vg, p.x, p.y);   }   inline point canvas::transform_point(point p)   {      float t[6];      nvgCurrentTransform(_vg, t);      point r;      nvgTransformPoint(&r.x, &r.y, t, p.x, p.y);      return r;   }   inline canvas::state canvas::new_state()   {      return { *this };   }   inline void canvas::save()   {      nvgSave(_vg);   }   inline void canvas::restore()   {      nvgRestore(_vg);   }   inline void canvas::clip(struct rect r)   {      nvgScissor(_vg, r.left, r.top, r.width(), r.height());   }   inline void canvas::begin_path()   {      nvgBeginPath(_vg);   }   inline void canvas::path_winding(solidity dir)   {      nvgPathWinding(_vg, int(dir));   }   inline void canvas::rect(struct rect r)   {      nvgRect(_vg, r.left, r.top, r.width(), r.height());   }   inline void canvas::round_rect(struct rect r, float radius)   {      nvgRoundedRect(_vg, r.left, r.top, r.width(), r.height(), radius);   }   inline void canvas::circle(struct circle c)   {      nvgCircle(_vg, c.cx, c.cy, c.radius);   }   inline void canvas::arc(struct circle c, float from, float to, winding dir)   {      nvgArc(_vg, c.cx, c.cy, c.radius, from, to, dir);   }   inline void canvas::fill()   {      nvgFill(_vg);   }   inline void canvas::fill_color(color c)   {      nvgFillColor(_vg, nvgRGBA(c));   }   inline void canvas::fill_paint(paint p)   {      nvgFillPaint(_vg, p);   }   inline bool canvas::on_fill(point p)   {      return nvgOnFill(_vg, p.x, p.y);   }   inline bool canvas::on_stroke(point p, float tolerance)   {      return nvgOnStroke(_vg, p.x, p.y, tolerance);   }   inline paint canvas::box_gradient(      struct rect r, float radius, float feather    , color inner_color, color outer_color)   {      return nvgBoxGradient(_vg,            r.left, r.top, r.width(), r.height(), radius, feather          , nvgRGBA(inner_color), nvgRGBA(outer_color)         );   }   inline paint canvas::linear_gradient(      point start, point end    , color inner_color, color outer_color)   {      return nvgLinearGradient(            _vg, start.x, start.y, end.x, end.y,            nvgRGBA(inner_color), nvgRGBA(outer_color)         );   }   inline paint canvas::radial_gradient(      point center, float inr, float outr    , color inner_color, color outer_color)   {      return nvgRadialGradient(            _vg, center.x, center.y, inr, outr,            nvgRGBA(inner_color), nvgRGBA(outer_color)         );   }   inline void canvas::move_to(point p)   {      nvgMoveTo(_vg, p.x, p.y);   }   inline void canvas::line_to(point p)   {      nvgLineTo(_vg, p.x, p.y);   }   inline void canvas::stroke_color(color c)   {      nvgStrokeColor(_vg, nvgRGBA(c));   }   inline void canvas::stroke_paint(paint p)   {      nvgStrokePaint(_vg, p);   }   inline void canvas::stroke_width(float w)   {      nvgStrokeWidth(_vg, w);   }   inline void canvas::stroke()   {      nvgStroke(_vg);   }   inline void canvas::new_font(char const* name, char const* path)   {      if (nvgCreateFont(_vg, name, path) == -1)         throw nanovg_exception("Failed to create font.");   }   inline void canvas::font_size(float size)   {      nvgFontSize(_vg, size);   }   inline void canvas::font_blur(float blur)   {      nvgFontBlur(_vg, blur);   }   inline void canvas::font_face(char const* font)   {      nvgFontFace(_vg, font);   }   inline void canvas::text_align(int align)   {      nvgTextAlign(_vg, align);   }   inline float canvas::text(point p, char const* f, char const* l)   {      return nvgText(_vg, p.x, p.y, f, l);   }   inline float canvas::text_width(char const* f, char const* l)   {      return nvgTextBounds(_vg, 0, 0, f, l, 0);   }   inline void canvas::text_box(point p, float width, char const* f, char const* l)   {      nvgTextBox(_vg, p.x, p.y, width, f, l);   }   inline float canvas::line_height() const   {      float lineh;      nvgTextMetrics(_vg, 0, 0, &lineh);      return lineh;   }   template <typename F>   inline void canvas::for_each_line(F line_f, char const* f, char const* l, float width)   {      NVGtextRow  rows[3];      while (int nrows = nvgTextBreakLines(_vg, f, l, width, rows, 3))      {         for (std::size_t i = 0; i < nrows; ++i)         {            if (!line_f(rows[i], i))               return;         }         // Keep going...         f = rows[nrows-1].next;      }   }   inline std::vector<glyph_position>   canvas::glyphs(point p, char const* f, char const* l) const   {      std::vector<NVGglyphPosition> r{ std::size_t(l-f) };      int nglyphs =         nvgTextGlyphPositions(            _vg, p.x, p.y, f, l, &r[0], int(r.size())         );      r.resize(nglyphs);      return r;   }   inline canvas::image::image(canvas& canvas_, char const* filename)    : _vg(canvas_._vg)    , _handle(nvgCreateImage(canvas_._vg, filename, 0))   {}   inline canvas::image::~image()   {      nvgDeleteImage(_vg, _handle);   }   inline point canvas::image::size() const   {      int w, h;      nvgImageSize(_vg, _handle, &w, &h);      return { float(w), float(h) };   }   inline void canvas::image::draw(struct rect bounds)   {      float x = bounds.left;      float y = bounds.top;      float w = bounds.width();      float h = bounds.height();      nvgSave(_vg);      NVGpaint image = nvgImagePattern(_vg, x, y, w, h, 0.0, _handle, 1.0);      nvgBeginPath(_vg);      nvgRect(_vg, x, y, w, h);      nvgFillPaint(_vg, image);      nvgFill(_vg);      nvgRestore(_vg);   }   inline void canvas::image::draw(struct rect bounds, point img_offset, float scale)   {      float x = bounds.left;      float y = bounds.top;      float w = bounds.width();      float h = bounds.height();      nvgSave(_vg);      point img_size = size();      NVGpaint image = nvgImagePattern(            _vg, x-(img_offset.x * scale), y-(img_offset.y * scale),            img_size.x * scale, img_size.y * scale, 0.0, _handle, 1.0);      nvgBeginPath(_vg);      nvgRect(_vg, x, y, w, h);      nvgFillPaint(_vg, image);      nvgFill(_vg);      nvgRestore(_vg);   }}#endif