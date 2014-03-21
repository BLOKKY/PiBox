#ifndef GRAPHIC_H
#define GRAPHIC_H
#include <string>
using std::string;
bool gbegin();
int gwidth();
int gheight();
void gend();
void gupdate();
void gclear(int r, int g, int b);
void gclear(int startR, int startG, int startB, int endR, int endG, int endB);
void gcol_text(int r, int g, int b);
void gcol_pen(int r, int g, int b);
void gcol_brush(int r, int g, int b);
void gtext_size(int size);
void grect(int sx, int sy, int ex, int ey);
void gplus(int x, int y, int r, int g, int b);
void gtext(int x, int y, string format, ...);
bool gfillbmp(string filename);
#endif
