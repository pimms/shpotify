#ifndef VIEW_H_
#define VIEW_H_

#include <ncurses.h>
#include <stdbool.h>
#include <menu.h>
#include <vector>
#include <string>


struct Vec2
{
    Vec2() { x=0; y=0; }
    Vec2(int x, int y) : x(x), y(y) { }

    int x;
    int y;
};

class View
{
public:
    View(WINDOW *parent);
    virtual ~View();

    virtual void redraw();
    virtual bool onKey(int key);
    virtual void onResize();
    virtual void onFocusChanged(bool hasFocus);

    Vec2 getSize() const;
    Vec2 getPosition() const;

    void setSize(Vec2 size);
    void setPosition(Vec2 pos);

protected:
    bool initialize();
    virtual bool onInit();

    WINDOW* getParentWindow();
    WINDOW* getWindow();

private:
    WINDOW *_parentWin;
    WINDOW *_win;
    Vec2 _size;
    Vec2 _pos;
};


class MenuItem
{
public:
    MenuItem(std::string title);
};

class MenuView : public View
{
public:
    MenuView(WINDOW *parent);
    ~MenuView();

    virtual bool onKey(int key) override;

protected:
    /**
     * Recreates the menu and queries the MenuView for
     * new content.
     */
    bool recreateMenu();

    virtual bool onInit() override;
    virtual int getItemCount();
    virtual std::string getTitleForItem(int index);
    virtual void onItemSelected(int index);

private:
    void destroyMenu();

    MENU *_menu;
    std::vector<ITEM*> _items;
    int _selItem;
};


class PlaybarView : public View
{
public:
    virtual bool onKey(int key) override;
};



#endif
