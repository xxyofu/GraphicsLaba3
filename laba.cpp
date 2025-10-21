#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

#define PI 3.14159265358979323846 

using namespace std;

struct RGB {
    unsigned char r, g, b;
};
struct Point {
    int x, y;
};
class Image1
{
public:
    int width, height;
    std::string magic_number;
    std::vector<RGB> pixels;

    Image1(const string& filepath)
    {
        ifstream stream(filepath, ios::binary);
        if (!stream) {
            cout << "Cant open file" << endl;
            return;
        }
        stream >> magic_number;
        stream.ignore(100, '\n');
        stream >> width >> height;
        stream.ignore(100, '\n');
        stream.ignore(100, '\n');

        pixels.resize(width * height);
        if(magic_number=="P6"){
            stream.read(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(RGB));
        }else if (magic_number=="P3"){
            for (int i = 0; i < width * height; i++) {
                int r, g, b;
                stream >> r >> g >> b;
                pixels[i] = {static_cast<unsigned char>(r), 
                            static_cast<unsigned char>(g), 
                            static_cast<unsigned char>(b)};
            }
        }else{
            return;
        }
    }
    void Replace_Pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
    {
        pixels[y * width + x] = { r, g, b };
    }
    bool Save_image(const string& filepath)
    {
        ofstream stream(filepath, ios::binary);
        if (!stream) {
            cout << "Cant make file" << endl;
            return false;
        }
        stream << "P6\n" << width << " " << height << "\n255\n";
        stream.write(reinterpret_cast<const char*>(pixels.data()), pixels.size() * sizeof(RGB));
        return true;
    }
};
double Sign(double x){
    if(x>0)
        return 1.0;
    else if(x==0)
        return 0;
    else
        return -1.0;
}

class Canvas1 {
public:
    int width, height;
    std::vector<RGB> pixels;
    Canvas1(const int& w, const int& h) 
    {
        width = w;
        height = h;
        pixels.assign(width * height,{255,255,255});
    }
    void Replace_Pixel(int x, int y, RGB& color)
    {
        pixels[y * width + x] = color;
    }
    void DrawLine(Point p1, Point p2, RGB& color)
    {
        int stepx = abs(p2.x - p1.x);
        int stepy = abs(p2.y - p1.y);
        int d = (stepx > stepy) ? stepx : stepy;
        int sx = (p1.x < p2.x) ? 1 : -1;
        int sy = (p1.y < p2.y) ? 1 : -1;
        int bx = 0;
        int by = 0;
        while (true) {
            if (p1.x == p2.x && p1.y == p2.y) break;
            Replace_Pixel(p1.x, p1.y, color);
            bx += stepx;
            by += stepy;
            p1.x += sx*(bx / d);
            p1.y += sy * (by / d);
            bx %= d;
            by %= d;
        }
       
    }
    void CopyLine(Point p1, Point p2, Point i1, Point i2, Image1 image)
    {
        int stepx = abs(p2.x - p1.x);
        int stepy = abs(p2.y - p1.y);
        int d = (stepx > stepy) ? stepx : stepy;
        int sx = (p1.x < p2.x) ? 1 : -1;
        int sy = (p1.y < p2.y) ? 1 : -1;
        int bx = 0;
        int by = 0;

        int istepx = abs(i2.x - i1.x);
        int istepy = abs(i2.y - i1.y);
        int id = (istepx > istepy) ? istepx : istepy;
        int isx = (i1.x < i2.x) ? 1 : -1;
        int isy = (i1.y < i2.y) ? 1 : -1;
        int ibx = 0;
        int iby = 0;

        while (true) {
            if (p1.x == p2.x && p1.y == p2.y) break;
            Replace_Pixel(p1.x, p1.y, image.pixels[i1.y*image.width+i1.x]);
            bx += stepx;
            by += stepy;
            p1.x += sx * (bx / d);
            p1.y += sy * (by / d);
            bx %= d;
            by %= d;

            ibx += istepx;
            iby += istepy;
            i1.x += isx * (ibx / id);
            i1.y += isy * (iby / id);
            ibx %= id;
            iby %= id;
        }
    }
    void CDA(Point start, Point end,RGB color) 
    {
        double L;
        if (abs(start.x - end.x) >= abs(start.y - end.y)) {
            L = abs(start.x - end.x);
        }
        else {
            L = abs(start.y - end.y);
        }
        double dx = (double)(end.x - start.x) / L;
        double dy = (double)(end.y - start.y) / L;
        double x = start.x + 0.5 * Sign(dx);
        double y = start.y + 0.5 * Sign(dy);
        for (int i = 1; i < L + 1; i++) {
            Replace_Pixel(floor(x),floor(y), color);
            x += dx;
            y += dy;
        }
    }
    void Brezenhem(Point start, Point end, RGB color) 
    {
        double dx = (double)(end.x - start.x);
        double dy = (double)(end.y - start.y);
        double sx = Sign(dx);
        double sy = Sign(dy);
        dx = abs(dx);
        dy = abs(dy);
        int flag = 0;
        if (dx < dy) {
            double t = dx;
            dx = dy;
            dy = t;
            flag = 1;
        }
        double f = dy / dx - 0.5;
        double x = start.x;
        double y = start.y;
        for(int i =0;i<dx;i++)
        {
            Replace_Pixel(floor(x), floor(y), color);
            if (f >= 0) {
                if (flag == 1) {
                    x = x + sx;
                }
                else {
                    y = y + sy;
                }
                f -= 1;
            }
            if (flag == 1) {
                y = y + sy;
            }
            else {
                x = x + sx;
            }
            f = f + dy / dx;
        };   
    }
    void BrezenhemC(Point start, Point end, RGB color)
    {
        int dx = (end.x - start.x);
        int dy = (end.y - start.y);
        int sx = Sign(dx);
        int sy = Sign(dy);
        dx = abs(dx);
        dy = abs(dy);
        int flag = 0;
        if (dx < dy) {
            int t = dx;
            dx = dy;
            dy = t;
            flag = 1;
        }
        int f = 2*dy - dx;
        int x = start.x;
        int y = start.y;
        for (int i = 0; i < dx; i++)
        {
            Replace_Pixel(floor(x), floor(y), color);
            if (f >= 0) {
                if (flag == 1) {
                    x = x + sx;
                }
                else {
                    y = y + sy;
                }
                f -= 2 * dx;
            }
            if (flag == 1) {
                y = y + sy;
            }
            else {
                x = x + sx;
            }
            f = f + 2*dy;
        };
    }
    void CopyPart(Image1& image, Point destcircle_center, Point circle_center, int radius)
    {
        //Point e{ place.x + end.x - start.x, place.y + end.y - start.y };
        //(i-radius)/radius
        // for (int i = 0; i < radius*2; i++)
        // {
        //     CopyLine({destcircle_center.x+(i-radius),destcircle_center.y+int(acos((i-radius)/radius)*radius/PI/2)}, {destcircle_center.x+(i-radius),destcircle_center.y-int(acos((i-radius)/radius)*radius/PI/2)}, {circle_center.x+(i-radius),circle_center.y+int(acos((i-radius)/radius)*radius/PI/2)}, {circle_center.x+(i-radius),circle_center.y-int(acos((i-radius)/radius)*radius/PI/2)}, image);
        // }

        for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            
            if (x*x + y*y <= radius*radius)
            {
                
                int src_x = circle_center.x + x;
                int src_y = circle_center.y + y;
                int dest_x = destcircle_center.x + x;
                int dest_y = destcircle_center.y + y;
                
                
                if (src_x >= 0 && src_x < image.width && 
                    src_y >= 0 && src_y < image.height &&
                    dest_x >= 0 && dest_x < image.width && 
                    dest_y >= 0 && dest_y < image.height)
                {
                    Replace_Pixel(dest_x, dest_y, image.pixels[src_y * image.width + src_x]);
                }
            }
        }
    }
    }
    void DrawFunk(Point zero, int l, RGB color)
    {
        for (int i = 1; i <= l; i += 1) 
        {
            //cout << zero.x + i << ' '<< sin(2 * PI * ((double)i / step)) << '\n';
            DrawLine({zero.x+i,zero.y-int(log(double(i)/100.0))}, {zero.x+i+1,zero.y-int(log(double(i+1)/100.0))}, color);
        }
    }
    bool Save_Canvas(const string& filepath, std::string magic_number)
    {
        ofstream stream(filepath, ios::binary);
        if (!stream) {
            cout << "Cant make file" << endl;
            return false;
        }
        if(magic_number == "P6"){
            stream << "P6\n" << width << " " << height << "\n255\n";
            stream.write(reinterpret_cast<const char*>(pixels.data()), pixels.size() * sizeof(RGB));
            return true;
        }else if (magic_number=="P3"){
            stream << "P3\n" << width << " " << height << "\n255\n";
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    const RGB& pixel = pixels[i * width + j];
                    stream << static_cast<int>(pixel.r) << " "
                        << static_cast<int>(pixel.g) << " "
                        << static_cast<int>(pixel.b);
                    if (j < width - 1) stream << " ";
                    }
                stream << "\n";
            }
            return true;
        }else{
            return false;
        }
    }
};

int main(int argc, char *argv[])
{
    //string input = "input.ppm";
    string output = "output";
    if(argc==2){
        output = argv[1];
    }
    // }else if(argc==3){
    //     input = argv[1];
    //     output = argv[2];
    // }
    RGB color = { 0,0,0 };
    Canvas1 cda(100, 100);
    Canvas1 brezenhem(100,100);
    Canvas1 int_brezenhem(100,100);
    // cda.CDA({40, 40},{50,50},color);
    Point polyline[4];
    for (int i = 0; i < 4; i++) {
        cout << "Point " << i + 1 << ": ";
        cin >> polyline[i].x >> polyline[i].y;
    }
    // polyline.push_back({10, 20});
    // polyline.push_back({30, 20});
    // polyline.push_back({10, 40});
    // polyline.push_back({30, 40});

    for(int i = 0; i<3; i++){
        cda.CDA(polyline[i], polyline[i+1], color);
        brezenhem.Brezenhem(polyline[i], polyline[i+1], color);
        int_brezenhem.BrezenhemC(polyline[i], polyline[i+1], color);
    }
    cda.Save_Canvas(output+".ppm", "P6");
    brezenhem.Save_Canvas(output+"(1).ppm", "P3");
    int_brezenhem.Save_Canvas(output+"(2).ppm", "P6");
}