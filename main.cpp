#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

constexpr char EVENT[] = "Advent of Code 2020";
constexpr char DAY[]   = "Day 1 - Report Repair";

const std::vector<std::string> AOC_LOGO {
    "        |        ",
    "       \\|/       ",
    "      --*--      ",
    "       >o<       ",
    "      >O<<<      ",
    "     >>o>>*<     ",
    "    >o<<<o<<<    ",
    "   >>@>*<<O<<<   ",
    "  >o>>@>>>o>o<<  ",
    " >*>>*<o<@<o<<<< ",
    ">o>o<<<O<*>>*>>O<",
    "   _ __| |__ _   "
};

Fl_Color get_tree_color(const char ch)
{
    switch (ch) {
        case '-':   [[fallthrough]];
        case '|':   [[fallthrough]];
        case '/':   [[fallthrough]];
        case '\\':  [[fallthrough]];
        case '*':   return FL_YELLOW;
        case '<':   [[fallthrough]];
        case '>':   return FL_GREEN;
        case 'o':   return 91u;          // orange
        case '_':   [[fallthrough]];
        case '@':   return FL_RED;
        case 'O':   return FL_BLUE;
        default:    return FL_WHITE;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Data_box : public Fl_Box {
public:
    Data_box(int xx, int yy, int ww, int hh, int val)
        : Fl_Box{xx, yy, ww, hh}, data{val}
    {
        copy_label(std::to_string(val).c_str());
        box(FL_FLAT_BOX);
    }

    int value() const { return data; }

    void draw() override
    {
        color(box_color);
        labelcolor(lab_color);
        Fl_Box::draw();
    }

    void color_change(Fl_Color b, Fl_Color l)
    {
        box_color = b;
        lab_color = l;
    }

    void change_value(int v)
    {
        data = v;
        copy_label(std::to_string(v).c_str());
    }

private:
    int data = 0;
    Fl_Color box_color = FL_BLACK;
    Fl_Color lab_color = FL_WHITE;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Calculation_row : public Fl_Group {
public:
    enum class Calc { SUM, PRODUCT };

    Calculation_row(int xx, int yy, int ww, int hh, Calc cc);

    void draw() override { Fl_Group::draw(); }

    Data_box* get_box(int i);
    const char* get_symbol() const;

private:
    Calc c;
};

Calculation_row::Calculation_row(int xx, int yy, int ww, int hh, Calc cc)
    : Fl_Group{xx, yy, ww, hh}, c{cc}
{
    const int NUM_BOXES = 4;
    const int BOX_WIDTH = w() / (NUM_BOXES + 1);
    const int BOX_HEIGHT = h();
    const int BOX_GAP = BOX_WIDTH / NUM_BOXES;
    const int FONT_SZ = h() - 8;
    const int SHIM = 24;

    for (auto i = 0; i < NUM_BOXES; ++i) {
        auto p = new Data_box{x() + i * (BOX_WIDTH + BOX_GAP), y(),
                BOX_WIDTH, BOX_HEIGHT, 0};
        p->labelfont(FL_BOLD);
        p->labelsize(FONT_SZ);
    }

    get_box(0)->color_change(FL_CYAN, FL_BLACK);
    get_box(1)->color_change(FL_MAGENTA, FL_BLACK);
    get_box(2)->color_change(FL_YELLOW, FL_BLACK);
    auto pdb = get_box(3);
    pdb->color_change(FL_WHITE, FL_BLACK);
    pdb->size(BOX_WIDTH + SHIM, BOX_HEIGHT);

    auto op1 = new Fl_Box(x() + 1 * BOX_WIDTH + 0 * BOX_GAP, y(),
            BOX_GAP, BOX_HEIGHT, get_symbol());
    op1->color(FL_BLACK);
    op1->labelcolor(FL_WHITE);
    op1->labelfont(FL_BOLD);
    op1->labelsize(FONT_SZ);
    auto op2 = new Fl_Box(x() + 2 * BOX_WIDTH + 1 * BOX_GAP, y(),
            BOX_GAP, BOX_HEIGHT, get_symbol());
    op2->color(FL_BLACK);
    op2->labelcolor(FL_WHITE);
    op2->labelfont(FL_BOLD);
    op2->labelsize(FONT_SZ);
    auto eq  = new Fl_Box(x() + 3 * BOX_WIDTH + 2 * BOX_GAP, y(),
            BOX_GAP, BOX_HEIGHT, "=");
    eq->color(FL_BLACK);
    eq->labelcolor(FL_WHITE);
    eq->labelfont(FL_BOLD);
    eq->labelsize(FONT_SZ);

    end();
}

Data_box* Calculation_row::get_box(int i)
{
    return reinterpret_cast<Data_box*>(child(i));
}

const char* Calculation_row::get_symbol() const
{
    switch (c) {
        case Calc::SUM:     return "+";
        case Calc::PRODUCT: return "*";
        default:            return " ";
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Director;

class Viewport : public Fl_Group {
private:
    static constexpr double FRAME_S = 1.0 / 4.0;
    const int OFFSET_X = 40;
    const int OFFSET_Y = 20;
    double scale = 1.0;
    bool game_over = false;
    std::unique_ptr<Director> dir;
    int num_inputs = 0;
    Calculation_row* sum_row = nullptr;
    Calculation_row* mul_row = nullptr;
public:
    Viewport(int xx, int yy, int ww, int hh);

    void draw() override;

    void start();
    void done();
    int check_sum(int i, int j, int k) const;
    double frame_rate() { return scale * FRAME_S; }
    void reset_scale() { scale = 1.0; }
    void scale_scale(double m) { scale *= m; }

    void make_plain(int i);
    void make_cyan(int i);
    void make_magenta(int i);
    void make_yellow(int i);
    void make_red(int i);
    void make_green(int i);

    bool step();

private:
    void position_inputs();

    static void timer_cb(void* userdata)
    {
        auto p = static_cast<Viewport*>(userdata);
        if (p->step())
            Fl::repeat_timeout(p->frame_rate(), timer_cb, userdata);
    }
};

enum class State {
    START,
    CHECK_BASE,
    CHECK_SUM,
    NEW_I,
    NEW_J,
    NEW_K,
    WIN,
    PROBLEM
};

class Director {
public:
    Director() = default;

    void query(Viewport* vpp);

private:
    int i = 0;
    int j = 1;
    int k = 2;
    int max = 0;
    State state = State::START;
};

void Director::query(Viewport* vpp)
{
    //std::cout << "DEBUG : " << vpp->check_sum(i, j, k);
    //std::cout << '\t' << i << ',' << j << ',' << k << '\n';
    switch (state) {
        case State::START:
            vpp->make_cyan(i);
            vpp->make_magenta(j);
            vpp->make_yellow(k);
            state = State::CHECK_BASE;
            return;
        case State::CHECK_BASE:
            {
                auto res = vpp->check_sum(i, j, k);
                if (res < 0)        state = State::NEW_K;
                else if (res > 0)   state = State::PROBLEM;
                else                state = State::WIN;
            }
            return;
        case State::CHECK_SUM:        // check a moving 'k'
            {
                auto res = vpp->check_sum(i, j, k);
                if (res < 0)        state = State::NEW_K;
                else if (res > 0)   state = State::NEW_J;
                else                state = State::WIN;
            }
            return;
        case State::NEW_I:          // inc 'i', reset 'j' and 'k'
            vpp->make_plain(k);
            vpp->make_plain(j);
            vpp->make_plain(i++);
            j = i + 1;
            k = j + 1;
            vpp->make_cyan(i);
            vpp->make_magenta(j);
            vpp->make_yellow(k);
            state = State::CHECK_BASE;
            vpp->reset_scale();
            return;
        case State::NEW_J:
            if (k == j + 1) {
                state = State::NEW_I;
                return;
            }
            vpp->make_plain(k);
            vpp->make_plain(j);
            j = j + 1;
            k = j + 1;
            vpp->make_magenta(j);
            vpp->make_yellow(k);
            state = State::CHECK_SUM;
            vpp->reset_scale();
            return;
        case State::NEW_K:          // inc 'k'
            if (k == vpp->children() - 1) {
                state = State::PROBLEM;
                return;
            }
            vpp->make_plain(k);
            vpp->make_yellow(++k);
            state = State::CHECK_SUM;
            vpp->scale_scale(0.9);
            return;
        case State::WIN:
            vpp->done();
            return;
        case State::PROBLEM:
            std::cerr << "Error: should not reach State:PROBLEM\n";
            vpp->done();
            return;
        default:    return;
    }
}

int Viewport::check_sum(int i, int j, int k) const
{
    auto n = reinterpret_cast<Data_box*>(child(i))->value();
    auto m = reinterpret_cast<Data_box*>(child(j))->value();
    auto l = reinterpret_cast<Data_box*>(child(k))->value();
    auto sum = n + m + l;

    sum_row->get_box(0)->change_value(n);
    sum_row->get_box(1)->change_value(m);
    sum_row->get_box(2)->change_value(l);

    auto psum = sum_row->get_box(3);
    psum->change_value(sum);
    if (sum > 2020) {
        psum->color_change(FL_RED, FL_BLACK);
    } else if (sum == 2020) {
        psum->color_change(FL_GREEN, FL_BLACK);
    } else {
        psum->color_change(FL_WHITE, FL_BLACK);
    }

    mul_row->get_box(0)->change_value(n);
    mul_row->get_box(1)->change_value(m);
    mul_row->get_box(2)->change_value(l);
    mul_row->get_box(3)->change_value(n * m * l);
    return sum - 2020;
}

Viewport::Viewport(int xx, int yy, int ww, int hh)
    : Fl_Group{xx, yy, ww, hh}, dir{std::make_unique<Director>()}
{
        const int BOX_WIDTH = 64;
        const int BOX_HEIGHT = 20;
        const int CALC_ROW_W = w() - 2 * OFFSET_X;
        const int CALC_ROW_H = 36;
        const int CALC_ROW_X = x() + OFFSET_X;
        const int CALC_ROW_Y = h() - 4 * CALC_ROW_H;

        // get data and sort
        std::vector<int> vi;
        for (auto n = 0; std::cin >> n; ) {
            vi.push_back(n);
        }
        std::sort(std::begin(vi), std::end(vi));

        // create boxes
        for (const auto& i : vi) {
            new Data_box{0, 0, BOX_WIDTH, BOX_HEIGHT, i};
        }
        num_inputs = children();

        // calculations
        sum_row = new Calculation_row{CALC_ROW_X, CALC_ROW_Y,
                CALC_ROW_W, CALC_ROW_H,
                Calculation_row::Calc::SUM};
        mul_row = new Calculation_row{CALC_ROW_X, CALC_ROW_Y + 2 * CALC_ROW_H,
                CALC_ROW_W, CALC_ROW_H,
                Calculation_row::Calc::PRODUCT};
        end();
        position_inputs();
}

void Viewport::draw()
{
    color(FL_BLACK);
    Fl_Group::draw();
}

void Viewport::start()
{
    show();
    Fl::add_timeout(frame_rate(), timer_cb, static_cast<void*>(this));
}

void Viewport::done()
{
    game_over = true;
}

void Viewport::make_plain(int i)
{
    auto p = reinterpret_cast<Data_box*>(child(i));
    p->color_change(FL_BLACK, FL_WHITE);
}

void Viewport::make_cyan(int i)
{
    auto p = reinterpret_cast<Data_box*>(child(i));
    p->color_change(FL_CYAN, FL_BLACK);
}

void Viewport::make_magenta(int i)
{
    auto p = reinterpret_cast<Data_box*>(child(i));
    p->color_change(FL_MAGENTA, FL_BLACK);
}

void Viewport::make_yellow(int i)
{
    auto p = reinterpret_cast<Data_box*>(child(i));
    p->color_change(FL_YELLOW, FL_BLACK);
}

void Viewport::make_red(int i)
{
    auto p = reinterpret_cast<Data_box*>(child(i));
    p->color_change(FL_RED, FL_BLACK);
}

void Viewport::make_green(int i)
{
    auto p = reinterpret_cast<Data_box*>(child(i));
    p->color_change(FL_GREEN, FL_BLACK);
}

bool Viewport::step()
{
    dir->query(this);
    redraw();
    return !game_over;
}

void Viewport::position_inputs()
{
    const int BOXES_PER_COL = 20;
    const int GAP_Y = 2;

    int x = 0;
    int y = 0;
    for (auto i = 0; i < num_inputs; ++i) {
        auto p = child(i);
        p->position(OFFSET_X + x * p->w(), OFFSET_Y + y * p->h() + GAP_Y);
        ++y;
        if (y == BOXES_PER_COL) {
            y = 0;
            ++x;
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Tree_logo : public Fl_Box {
public:
    Tree_logo(int xx, int yy, int ww, int hh)
        : Fl_Box{xx, yy, ww, hh}
    {
        box(FL_FLAT_BOX);
        color(FL_BLACK);

        Fl_Font old_font = fl_font();
        fl_font(FL_COURIER, CHAR_H);
        const int char_w = fl_width('#');
        fl_font(old_font, CHAR_H);

        const int cols = AOC_LOGO.front().size();
        const int rows = AOC_LOGO.size();
        const int new_width = OFFSET_X * 2 + cols * char_w;
        const int new_height = OFFSET_Y * 2 + rows * CHAR_H;
        const int new_x = x() - new_width / 2;
        const int new_y = y() - new_height / 2;

        position(new_x, new_y);
        size(new_width, new_height);
    }

    void draw() override
    {
        const int INNER_X = x() + OFFSET_X;
        const int INNER_Y = y() + OFFSET_Y;

        fl_color(FL_WHITE);

        fl_font(FL_COURIER, CHAR_H);

        int xx = 0;
        int yy = 0;
        char c[2] = " ";
        for (const auto& s : AOC_LOGO) {
            for (const auto ch : s) {
                c[0] = ch;
                if (ch != ' ') {
                    fl_color(get_tree_color(ch));
                    fl_draw(c, INNER_X + xx * CHAR_W, INNER_Y + yy * CHAR_H);
                }
                ++xx;
            }
            xx = 0;
            ++yy;
        }
    }
private:
    const int OFFSET_X = 20;
    const int OFFSET_Y = 20;
    const int CHAR_H = 24;
    const int CHAR_W = 14;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Title : public Fl_Group {
public:
    Title(int xx, int yy, int ww, int hh, Viewport* vp)
        : Fl_Group{xx, yy, ww, hh}, vpp{vp}
    {
        constexpr int OFFSET_X = 20;
        constexpr int OFFSET_Y = 20;
        const int XC = w() / 2;
        const int YC = h() / 2;
        const int TITLE_W = w() - 2 * OFFSET_X;
        const int TITLE_H = 48;
        const int TITLE_SZ = 40;
        const int BUTT_W = 120;
        const int BUTT_H = 48;
        const int BUTT_SZ = 28;
        const int BUTT_X = XC - BUTT_W / 2;
        const int BUTT_Y = h() - 2 * OFFSET_Y - BUTT_H;
        const int DAY_H = 40;
        const int DAY_SZ = 36;
        const int DAY_Y = BUTT_Y - 2 * OFFSET_Y - DAY_H;

        auto title = new Fl_Box{OFFSET_X, OFFSET_Y, TITLE_W, TITLE_H, EVENT};
        title->box(FL_FLAT_BOX);
        title->color(FL_BLACK);
        title->labelcolor(FL_WHITE);
        title->labelfont(FL_BOLD);
        title->labelsize(TITLE_SZ);

        auto butt = new Fl_Button{BUTT_X, BUTT_Y, BUTT_W, BUTT_H, "Start"};
        butt->box(FL_FLAT_BOX);
        butt->color(FL_WHITE);
        butt->labelsize(BUTT_SZ);
        butt->labelfont(FL_BOLD);
        butt->labelcolor(FL_BLACK);
        butt->callback(start_cb, static_cast<void*>(this));

        [[maybe_unused]] auto tree = new Tree_logo{XC, YC - 2 * OFFSET_Y, 0, 0};

        auto day = new Fl_Box{OFFSET_X, DAY_Y, TITLE_W, DAY_H, DAY};
        day->box(FL_FLAT_BOX);
        day->color(FL_BLACK);
        day->labelsize(DAY_SZ);
        day->labelfont(FL_BOLD);
        day->labelcolor(FL_WHITE);

        end();
    }

    void draw() override
    {
        Fl_Group::draw();
    }

private:
    Viewport* vpp;

    static void start_cb(Fl_Widget* w, void* data)
    {
        auto p = static_cast<Title*>(data);
        p->start();
    }

    void start()
    {
        hide();
        vpp->start();
    }
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main(int argc, char* argv[])
{
    /*
    for (const auto& s : AOC_LOGO) {
        std::cout << s << '\n';
    }
    std::cout << EVENT << '\n';
    std::cout << DAY << '\n';
    */

    auto win = new Fl_Double_Window{720, 600, "AoC 2020 Day 1"};
    win->color(FL_BLACK);
    auto vp = new Viewport{0, 0, win->w(), win->h()};
    vp->hide();
    [[maybe_unused]] auto tit = new Title{0, 0, win->w(), win->h(), vp};

    win->end();
    win->show();
    return Fl::run();
}