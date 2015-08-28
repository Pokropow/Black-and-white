#include <iostream>
#include <windows.h>
#include <fstream>
#include <cmath>
using namespace std;

/// Function to save bitmap to file
void saveBitmapToFile(ofstream &file,HBITMAP hbitmap)
{

    BITMAP bitmap;
    HBITMAP hbmOLD;

    GetObject(hbitmap,sizeof(bitmap),&bitmap);
    bitmap.bmBitsPixel=32;
    HDC hdc = CreateCompatibleDC(0);
    hbmOLD = (HBITMAP)SelectObject(hdc,hbitmap);

    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER info;

    info.biSize=sizeof(BITMAPINFOHEADER);
    info.biWidth = bitmap.bmWidth;
    info.biHeight = abs(bitmap.bmHeight);
    info.biPlanes = 1;
    info.biBitCount = bitmap.bmBitsPixel;
    info.biCompression = BI_RGB;
    info.biSizeImage = bitmap.bmWidth*bitmap.bmHeight*info.biBitCount/8;
    info.biXPelsPerMeter = bitmap.bmWidth;
    info.biYPelsPerMeter = bitmap.bmHeight;
    info.biClrUsed=0;
    info.biClrImportant=0;

    fileheader.bfType=0x4d42;
    fileheader.bfOffBits=54;
    fileheader.bfSize=info.biSizeImage+sizeof(info)+sizeof(fileheader);


    file.write((char*)&fileheader,sizeof(fileheader));
    file.write((char*)&info,sizeof(info));

    BYTE  *bits = new BYTE  [info.biSizeImage];
    GetDIBits(hdc,hbitmap,0,info.biHeight,bits,(BITMAPINFO*)&info,DIB_RGB_COLORS);

    file.write((char*)bits,info.biSizeImage);

    bitmap.bmBitsPixel=32;
    SelectObject(hdc,hbmOLD);
    DeleteDC(hdc);
}


/// 3D vector to save pixel as RGB
class Vector3
{
public:
    Vector3(int x,int y,int z)
    {
        this->x=x;
        this->y=y;
        this->z=z;
    }
    Vector3()
    {
        x=y=z=0;
    }
    int x,y,z;
    Vector3 operator-(Vector3 v)
    {
        Vector3 q;
        q.x = x-v.x;
        q.y = y-v.y;
        q.z = z-v.z;
        return q;

    }
    double value()
    {

        return sqrt( sq(x)+sq(y)+sq(z) );
    }
private:
    double sq(int a)
    {
        return a*a;
    }
};
///change pixel color from binary to vector
Vector3 pixelToVec(unsigned int a)
{
        Vector3 v;
        v.x = a&0xFF;
        a = a >>8;
        v.y = a&0xFF;
        a = a >>8;
        v.z = a&0xFF;
        return v;
}

COLORREF colorToBlack_White(COLORREF col)
{
    Vector3 v = pixelToVec(col);
    const float r = 1.0/650,g = 1.0/510, b = 1.0/475; /// iversed wave lenghts of light - red green and blue
    int x = (v.x*r+v.y*g+v.z*b)/(r+g+b);
    return RGB(x,x,x);
}

int main(int argc,char**argv)
{
    float max_value = 200;
    string sciezka;
    if(argc==1){
        getline(cin,sciezka);
    }
    else if(argc==2)
        sciezka=argv[1];
    else
        return 3;
    cout << "argc: " << argc << endl;
    cout << "file path: " << sciezka << endl;

    ofstream fil((sciezka+"_converted.bmp").c_str(),std::ios::binary);


    HBITMAP hbitmap,hout;
    BITMAP bitmap,out;
    hbitmap = (HBITMAP) LoadImage (NULL,sciezka.c_str(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
    GetObject(hbitmap,sizeof(bitmap),&bitmap);

    COLORREF * buf = new COLORREF[bitmap.bmWidth*bitmap.bmHeight];
    for(int i=0;i<bitmap.bmWidth*bitmap.bmHeight;i++)
    {
        buf[i] = RGB(255,255,255);
    }

    hout = CreateBitmap(bitmap.bmWidth,bitmap.bmHeight,bitmap.bmPlanes,bitmap.bmBitsPixel,buf);
    HDC dco = CreateCompatibleDC(0);
    HDC dci = CreateCompatibleDC(0);
    HBITMAP oldBmp =(HBITMAP) SelectObject(dco,hout);
    HBITMAP oldBmp2 =(HBITMAP) SelectObject(dci,hbitmap);



    for(int x=0;x<bitmap.bmWidth;x++)
    for(int y=0;y<bitmap.bmHeight;y++)
    {
        COLORREF color = GetPixel(dci,x,y);
        color = colorToBlack_White(color);
        SetPixel(dco,x,y,color);
    }



    saveBitmapToFile(fil,hout);
    SelectObject(dco,oldBmp);
    DeleteDC(dco);
    SelectObject(dci,oldBmp2);
    DeleteDC(dci);
    system("pause");
}
