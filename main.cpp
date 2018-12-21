#include <iostream>
#include <list>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include <sstream>
#include <cmath>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;


// XInfo Structure
struct XInfo {
    Display	 *display;
    int		 screen;
    Window	 window;
    Pixmap pixmap;
    XEvent xevent;
    GC gc;
};

//Colors class. Create a new Color object and use it to make colorful objects.
class Colors{
public:
    XColor black_col;
    XColor white_col;
    XColor red_col;
    XColor green_col;
    XColor blue_col;
    XColor yellow_col;
    XColor orange_col;
    XColor purple_col;
    XColor turquoise_col;

    void initColors(XInfo & xinfo, XColor & color, Colormap & colormap, char bits []){
        XParseColor(xinfo.display, colormap, bits, &color);
        XAllocColor(xinfo.display, colormap, &color);
    }

    Colors(XInfo xinfo){
        Colormap colormap;
        char black_bits[] = "#000000";
        char white_bits[] = "#FFFFFF";
        char red_bits[] = "#FF0000";
        char green_bits[] = "#00FF00";
        char blue_bits[] = "#0000FF";
        char yellow_bits[] = "#FFFF00";
        char orange_bits[] = "#FFA500";
        char purple_bits[] = "#BF3FBF";
        char turquoise_bits[] = "#40E0D0";

         colormap = DefaultColormap(xinfo.display, 0);

        initColors(xinfo, black_col, colormap, black_bits);
        initColors(xinfo, white_col, colormap, white_bits);
        initColors(xinfo, red_col, colormap, red_bits);
        initColors(xinfo, green_col, colormap, green_bits);
        initColors(xinfo, blue_col, colormap, blue_bits);
        initColors(xinfo, yellow_col, colormap, yellow_bits);
        initColors(xinfo, orange_col, colormap, orange_bits);
        initColors(xinfo, purple_col, colormap, purple_bits);
        initColors(xinfo, turquoise_col, colormap, turquoise_bits);
    }
};

//Displayable Base Class.
class Displayable{
public:
    virtual void paint(XInfo &xinfo) = 0;

    bool isBrick;

    virtual ~Displayable(){}
};

class Text : public Displayable {
public:
    XInfo xinfo;
    XColor color;
    int x;
    int y;
    string s;

    Text(XInfo &xinfo, XColor &color, int x, int y, string s){
        this->xinfo = xinfo;
        this->color = color;
        this->x = x;
        this->y = y;
        this->s = s;
        this->isBrick = false;
    }

    virtual void paint(XInfo &xinfo){
        XSetForeground(xinfo.display, xinfo.gc, color.pixel);
        XDrawString(xinfo.display, xinfo.pixmap, xinfo.gc, x, y, s.c_str(), s.length());
    }

    ~Text(){}

};

class Brick : public Displayable{
public:
    XInfo xinfo;
    XColor color;
    int left;
    int top;
    int w;
    int h;

    Brick(XInfo &xinfo, XColor &color, int left, int top, int w, int h){
        this->xinfo = xinfo;
        this->color = color;
        this->left = left;
        this->top = top;
        this->w = w;
        this->h = h;
        this->isBrick = true;
    }

    virtual void paint(XInfo &xinfo){
        XSetForeground(xinfo.display, xinfo.gc, color.pixel);
        XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc, left, top, w, h);
    }

    ~Brick(){}
};

class Paddle : public Displayable{
public:
    XInfo xinfo;
    XColor color;
    int x1;
    int y1;
    int x2;
    int y2;

    Paddle(XInfo &xinfo, XColor &color, int x1, int y1, int x2, int y2){
        this->xinfo = xinfo;
        this->color = color;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->isBrick = false;
    }

    void update_paddle_x_coordinates(int x1, int x2){
        this->x1 = x1;
        this->x2 = x2;
    }

    virtual void paint(XInfo &xinfo){
        XSetForeground(xinfo.display, xinfo.gc, color.pixel);
        XSetLineAttributes(xinfo.display, xinfo.gc, 15, LineSolid, CapButt, JoinRound);
        XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc, x1, y1, x2, y2);
    }

    ~Paddle(){}

};

class Ball : public Displayable{
public:
    XInfo xinfo;
    XColor color;
    int x_coordinate;
    int y_coordinate;
    int x_dir;
    int y_dir;

    int x_angle;
    int y_angle;

    int w;
    int h;
    int angle1;
    int angle2;

    Ball(XInfo &xinfo, XColor &color, int x_coordinate, int y_coordinate, int w, int h, int angle1, int angle2, int ballspeed){
        this->xinfo = xinfo;
        this->color = color;
        this->x_coordinate = x_coordinate;
        this->y_coordinate = y_coordinate;
        this->w = w;
        this->h = h;
        this->angle1 = angle1;
        this->angle2 = angle2;
        x_dir = 0;
        if (ballspeed > 0){
            y_dir = -ballspeed;
        }else{
            y_dir = ballspeed;
        }
        x_angle = 0;
        y_angle = ballspeed;
        this->isBrick = false;
    }

    void bounce(){
        // bounce ball
        XWindowAttributes win;
        XGetWindowAttributes(xinfo.display, xinfo.window, &win);

        if (x_coordinate + w > win.width ||
            x_coordinate  < 0)
            x_dir = -x_dir;
        if (y_coordinate  < 0)
            y_dir = -y_dir;
    }

    void bounce_paddle(Paddle * paddle, int &score){
        int x_origin = x_coordinate + w/2;
        int y_origin = y_coordinate + h/2;

        int x_left = x_coordinate;
        int x_right = x_coordinate + w;
        int y_top = y_coordinate;
        int y_bottom = y_coordinate + h;


        int p_left = paddle->x1;
        int p_right = paddle->x2;
        int p_y = paddle->y1;

        if (((x_right >= p_left) && (x_left <= p_right)) && (y_bottom >= p_y)){
            int length = paddle->x2 - paddle->x1;
            int middle = paddle->x1 + length / 2;
            int angle =  (x_origin - middle) /10;
              y_dir = -y_dir;
              x_dir = angle;
        }
    }

    void update_ball_position(int fps){
        x_coordinate += (x_dir * 60/fps);
        y_coordinate += (y_dir * 60/fps);
    }

    void bounce_wall(vector <Displayable*> &dList, int &score){
        int x_left = x_coordinate;
        int x_right = x_coordinate + w;
        int y_top = y_coordinate;
        int y_bottom = y_coordinate + h;

        int x_origin = x_coordinate + w/2;
        int y_origin = y_coordinate + h/2;

        for (int i = 0; i < dList.size(); i++){
            if (dList[i]->isBrick){
                Brick * b = dynamic_cast<Brick *>(dList[i]);
                int b_left = b->left;
                int b_right = b_left + b->w;
                int b_top = b->top;
                int b_bottom = b_top + b->h;
                if ((x_origin >= b_left && x_origin <= b_right) && (y_top <= b_bottom && y_origin >= b_bottom)){
                    y_dir = -y_dir;
                    score++;
                    cout << "1" << endl;
                    delete dList[i];
                    dList.erase(dList.begin() + i);
                    return;
                }else if ((x_origin >= b_left && x_origin <= b_right) && (y_bottom >= b_top && y_origin <= b_top)){
                    y_dir = -y_dir;
                    score++;
                    cout << "2" << endl;
                    delete dList[i];
                    dList.erase(dList.begin() + i);
                    return;
                }

                else if((x_right >= b_left && x_origin <= b_left) && (y_origin >= b_top && y_origin <= b_bottom)){
                    x_dir = -x_dir;
                    score++;
                    cout << "3" <<endl;
                    delete dList[i];
                    dList.erase(dList.begin() + i);
                    return;
                }else if ((y_origin >= b_top && y_origin <= b_bottom) && (x_left <= b_right && x_origin >= b_right)){
                    x_dir = -x_dir;
                    score++;
                    cout << "4" <<endl;
                    delete dList[i];
                    dList.erase(dList.begin() + i);
                    return;
                }

            }
        }
    }

    bool hit_bottom(){
        XWindowAttributes win;
        XGetWindowAttributes(xinfo.display, xinfo.window, &win);
        if (y_coordinate + w / 2 > win.height){
            return true;
        }
        return false;
    }

    virtual void paint(XInfo &xinfo){
        XSetForeground(xinfo.display, xinfo.gc, color.pixel);
        XSetLineAttributes(xinfo.display, xinfo.gc, 15, LineSolid, CapButt, JoinRound);
        XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc, x_coordinate, y_coordinate, w, h, angle1, angle2);
    }

    ~Ball(){};
};

void repaint( vector<Displayable*> dList, XInfo &xinfo) {
    XWindowAttributes w;
    XGetWindowAttributes(xinfo.display, xinfo.window, &w);

    int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
    Pixmap	buffer = XCreatePixmap(xinfo.display, xinfo.window, w.width, w.height, depth);

    xinfo.pixmap = buffer;
    for  (int i = 0; i < dList.size(); i++) {
        dList[i]->paint(xinfo);
    }

     XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc,
                  0, 0, w.width, w.height,  // region of pixmap to copy
                  0, 0); // position to put top left corner of pixmap in window
    XFlush(xinfo.display);
}

void createWall(XInfo &xinfo, Colors &a, vector <Displayable*> &dList){

    int col = 80;
    int row = 70;
    for (int i = 1; i < 16; i++) {
        Brick *b = new Brick(xinfo, a.red_col, row + 64*i, col, 60, 25);
        dList.push_back(b);
    }
    col+= 28;
    for (int i = 1; i < 16; i++) {
        Brick *b = new Brick(xinfo, a.orange_col, row + 64*i,col , 60, 25);
        dList.push_back(b);
    }
    col+= 28;
    for (int i = 1; i < 16; i++) {
        Brick * b = new Brick(xinfo, a.yellow_col, row + 64*i,col , 60, 25);
        dList.push_back(b);
    }
    col+= 28;
    for (int i = 1; i < 16; i++) {
        Brick * b = new Brick(xinfo, a.green_col, row + 64*i,col , 60, 25);
        dList.push_back(b);
    }
    col+= 28;
    for (int i = 1; i < 16; i++) {
        Brick * b = new Brick(xinfo, a.blue_col, row + 64*i,col , 60, 25);
        dList.push_back(b);
    }
    col+= 28;
    for (int i = 1; i < 16; i++) {
        Brick * b = new Brick(xinfo, a.purple_col, row + 64*i,col , 60, 25);
        dList.push_back(b);
    }
    col+= 28;
}

//Send error message function.
void error( string str ) {
    cerr << str << endl;
    exit(0);
}

// get microseconds
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void destroyList(vector<Displayable*> &dList){
    for (int i = 0; i < dList.size(); i++){
       delete dList[i];
    }
}

bool areBricksLeft(vector <Displayable *> dlist){
    for (int i = 0; i < dlist.size(); i++){
        if (dlist[i]->isBrick == true){
            return true;
        }
    }
    return false;
}

int eventloop(XInfo &xInfo, int fps, int ballSpeed){
    XWindowAttributes w;
    XGetWindowAttributes(xInfo.display, xInfo.window, &w);
    int FPS = fps;
    // time of last window paint
    unsigned long lastRepaint = 0;
    XEvent &event = xInfo.xevent;
    Colors a (xInfo);
    vector <Displayable*> dList;

    Ball *ball = new Ball(xInfo, a.white_col, 600, 650, 25, 25, 0, 360 * 64, ballSpeed);
    Paddle *paddle = new Paddle(xInfo, a.white_col, 550, 700, 650, 700);

    XFontStruct * font;
    font = XLoadQueryFont (xInfo.display, "12x24");
    XSetFont (xInfo.display, xInfo.gc, font->fid);

    int player_score = 0;
    Text *score = new Text(xInfo, a.turquoise_col, 10, 700, "SCORE: 0");


    ostringstream fpsos;
    fpsos << "FPS: " << fps;
    string fps_string = fpsos.str();
    Text *fps_text = new Text(xInfo, a.yellow_col, 10, 750, fps_string);
    createWall(xInfo, a, dList);

    dList.push_back(ball);
    dList.push_back(paddle);
    dList.push_back(score);
    dList.push_back(fps_text);
    repaint(dList, xInfo);
    XFlush(xInfo.display);

    while (true){
        if (XPending(xInfo.display) > 0){
            XNextEvent( xInfo.display, &xInfo.xevent);
            if (xInfo.xevent.type == KeyPress){
                KeySym key;
                char text[10];
                int i = XLookupString((XKeyEvent*) & event, text, 10, &key, 0 );
                if ( i == 1 && text[0] == 'q' ) {
                    XCloseDisplay(xInfo.display);
                    destroyList(dList);
                    exit(0);
                }else if (i == 1 && text[0] == 'd'){
                    int new_x1 = paddle->x1 + (paddle->x2 - paddle->x1)/2;
                    int new_x2 = paddle->x2 + (paddle->x2 - paddle->x1)/2;
                    if (new_x2 <= w.width){
                        paddle->update_paddle_x_coordinates(new_x1, new_x2);
                    }
                }
                else if (i == 1 && text[0] == 'a'){
                    int new_x1 = paddle->x1 - (paddle->x2 - paddle->x1)/2;
                    int new_x2 = paddle->x2 - (paddle->x2 - paddle->x1)/2;
                    if (new_x1 >= 0){
                        paddle->update_paddle_x_coordinates(new_x1, new_x2);
                    }
                } else if ( i == 1 && text[0] == 'r' ) {
                    XCloseDisplay(xInfo.display);
                    destroyList(dList);
                    return false;
                }
            }
        }

        unsigned long endTime = now();

        if (endTime - lastRepaint > 1000000 / FPS){
            ball->bounce_paddle(paddle, player_score);
            ball->bounce();
            ball->update_ball_position(fps);
            ball->bounce_wall(dList, player_score);

            ostringstream oss;
            oss << "SCORE: " << player_score;
            string player_score_str = oss.str();
            score->s = player_score_str;

            if (!areBricksLeft(dList)){
                sleep(1);
                createWall(xInfo, a, dList);
                ball->x_coordinate = 600;
                ball->y_coordinate = 650;
                ball->x_dir = ball->x_angle;
                ball->y_dir = ball->y_angle;

                paddle->x1 = 550;
                paddle->x2 = 650;
            }


            if (ball->hit_bottom()){
                vector <Displayable *> endText;
                endText.push_back(new Text (xInfo, a.white_col, 500, 400, player_score_str));
                endText.push_back(new Text (xInfo, a.green_col, 500, 500, "GAME OVER"));
                endText.push_back(new Text(xInfo, a.orange_col, 500, 550, "Click q to Quit or r to Restart"));

                while (true){
                    if (XPending(xInfo.display) > 0){
                        XNextEvent( xInfo.display, &xInfo.xevent);
                        if (xInfo.xevent.type == KeyPress){
                            KeySym key;
                            char text[10];
                            int i = XLookupString((XKeyEvent*) & event, text, 10, &key, 0 );
                            if ( i == 1 && text[0] == 'r' ) {
                                XCloseDisplay(xInfo.display);
                                destroyList(dList);
                                destroyList(endText);
                                return false;
                            }else if (i == 1 && text[0] == 'q'){
                                XCloseDisplay(xInfo.display);
                                destroyList(dList);
                                exit(0);
                            }
                        }
                    }
                    repaint(endText, xInfo);
                }
            }
            repaint(dList, xInfo);
        }

        lastRepaint = now();


        // IMPORTANT: sleep for a bit to let other processes work
        if (XPending(xInfo.display) == 0) {
            usleep(1000000 / FPS - (endTime - lastRepaint));
        }
    }
}

//Setup display, window and screen init function.
void initX(XInfo &xinfo){
    //add int argc and argv[]

    xinfo.display = XOpenDisplay( "" );
    if ( !xinfo.display )	{
        error( "Can't open display." );
    }
    xinfo.screen = DefaultScreen( xinfo.display );

    unsigned long white = XWhitePixel( xinfo.display, xinfo.screen );
    unsigned long black = XBlackPixel( xinfo.display, xinfo.screen );

    XSizeHints hints;
    hints.x = 100;
    hints.y = 100;
    hints.width = 1200;
    hints.height = 800;
    hints.flags = PPosition | PSize;
    xinfo.window = XCreateSimpleWindow(
            xinfo.display,				// display where window appears
            DefaultRootWindow( xinfo.display ), // window's parent in window tree
            hints.x, hints.y,			// upper left corner location
            hints.width, hints.height,	// size of the window
            5,						// width of window's border
            white,						// window border colour
            black );					// window background colour

    xinfo.gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
    XSetForeground(xinfo.display, xinfo.gc, XBlackPixel(xinfo.display, xinfo.screen));
    XSetBackground(xinfo.display, xinfo.gc, XWhitePixel(xinfo.display, xinfo.screen));

    XSelectInput(xinfo.display, xinfo.window, ButtonPressMask | KeyPressMask);

    XMapRaised(xinfo.display, xinfo.window);
    XFlush(xinfo.display);

    sleep(1);
}

void init_Splash(XInfo &xinfo){
    xinfo.display = XOpenDisplay( "" );
    if ( !xinfo.display )	{
        error( "Can't open display." );
    }
    xinfo.screen = DefaultScreen( xinfo.display );


    unsigned long white = XWhitePixel( xinfo.display, xinfo.screen );
    unsigned long black = XBlackPixel( xinfo.display, xinfo.screen );

    XSizeHints hints;
    hints.x = 100;
    hints.y = 100;
    hints.width = 1200;
    hints.height = 800;
    hints.flags = PPosition | PSize;
    xinfo.window = XCreateSimpleWindow(
            xinfo.display,				// display where window appears
            DefaultRootWindow( xinfo.display ), // window's parent in window tree
            hints.x, hints.y,			// upper left corner location
            hints.width, hints.height,	// size of the window
            5,						// width of window's border
            white,						// window border colour
            black );					// window background colour




    XSelectInput(xinfo.display, xinfo.window, ButtonPressMask | KeyPressMask);

    XMapRaised(xinfo.display, xinfo.window);
    XFlush(xinfo.display);
    sleep(1);

    xinfo.gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
    XSetForeground(xinfo.display, xinfo.gc, XBlackPixel(xinfo.display, xinfo.screen));
    XSetBackground(xinfo.display, xinfo.gc, XWhitePixel(xinfo.display, xinfo.screen));
    XSetLineAttributes(xinfo.display, xinfo.gc,
                       4,       // 3 is line width
                       LineSolid, CapButt, JoinRound);
    XFontStruct * font;
    font = XLoadQueryFont (xinfo.display, "12x24");
    XSetFont (xinfo.display, xinfo.gc, font->fid);

    vector <Displayable *> dList;
    Colors color (xinfo);
    dList.push_back(new Text(xinfo, color.white_col, 500, 100, "Pratik Choudhary"));
    dList.push_back(new Text(xinfo, color.white_col, 530, 150, "p2choudh"));
    dList.push_back(new Text(xinfo, color.yellow_col, 500, 200, "BREAKOUT GAME"));

    dList.push_back (new Text(xinfo, color.green_col, 100, 275, "How to Play"));
    dList.push_back (new Text(xinfo, color.white_col, 100, 350,"Move Paddle left  : a"));
    dList.push_back(new Text(xinfo, color.white_col, 100, 400, "Move Paddle right : d"));
    dList.push_back(new Text(xinfo, color.white_col, 100, 450, "Quit              : q"));
    dList.push_back(new Text(xinfo, color.white_col, 100, 500, "Restart           : r"));
    dList.push_back (new Text(xinfo, color.green_col, 450, 275, "Description"));
    dList.push_back(new Text(xinfo, color.yellow_col, 450, 350,
            "Move the paddle to stop the ball from hitting the ground"
            ));
    dList.push_back(new Text(xinfo, color.yellow_col, 450, 400,
                             "Break maximum number of bricks to get most points"
    ));
    dList.push_back(new Text(xinfo, color.yellow_col, 450, 450,
                                "New Bricks will refill once all bricks are cleared."
    ));
    dList.push_back(new Text(xinfo, color.yellow_col, 450, 500,
                             "You can restart the game once you lose."
    ));

    dList.push_back(new Text(xinfo, color.white_col, 500, 600, "Press s to start playing"));

    XEvent &event = xinfo.xevent;

    while (true){
        if (XPending(xinfo.display) > 0){
            XNextEvent( xinfo.display, &xinfo.xevent);
            if (xinfo.xevent.type == KeyPress){
                KeySym key;
                char text[10];
                int i = XLookupString((XKeyEvent*) & event, text, 10, &key, 0 );
                if ( i == 1 && text[0] == 's' ) {
                    XCloseDisplay(xinfo.display);
                    destroyList(dList);
                    return;
                }
            }
        }
        repaint(dList, xinfo);
        XFlush(xinfo.display);
    }
}

int main(int argc, char *argv[]){
    XInfo splash;
    init_Splash(splash);
    XInfo xinfo;
    initX(xinfo);
    bool keepRestarting;
    int fps = 60;
    int ballSpeed = -5;

    if (argc > 1){
        istringstream fps_ss (argv[1]);
        istringstream bs_ss (argv[2]);

        if (!(fps_ss >> fps && bs_ss >> ballSpeed)){
            cerr << "Invalid parameters" << endl;
            exit(1);
        }
        cout << ballSpeed << endl;
        keepRestarting = eventloop(xinfo, fps, ballSpeed);
    }else {
        keepRestarting = eventloop(xinfo, fps, ballSpeed);
    }


    while (!keepRestarting){
        XInfo continue_xinfo;
        initX(continue_xinfo);
        keepRestarting = eventloop(continue_xinfo, fps, ballSpeed);
    }

}