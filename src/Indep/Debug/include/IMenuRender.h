#ifndef IMENURENDER_H
#define IMENURENDER_H

class IMenuRender {
public:
    virtual void renderOverlay(int x, int y, int width, int height, float alpha, unsigned int color) = 0;
    virtual void print(int x, int y, unsigned int color, const char* str) = 0;
    virtual void getStringSize(const char* str, int* x, int* y) = 0;
};

#endif