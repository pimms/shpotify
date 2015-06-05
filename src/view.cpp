#include "view.h"
#include "shpotify.h"

#include <memory.h>
#include <stdlib.h>
#include <stdexcept>
#include <sstream>


// View
View::View(WINDOW *parent):
    _parentWin(parent),
    _win(nullptr),
    _size(0, 0),
    _pos(0, 0)
{
    if (!initialize())
        throw std::runtime_error("Unable to initialize View");
}

View::~View()
{
    if (_win)
		delwin(_win);
    _win = nullptr;
}

void View::redraw()
{
	box(_win, ACS_VLINE, ACS_HLINE);
	wnoutrefresh(_win);
}

bool View::onKey(int)
{
    return false;
}

void View::onResize()
{
	mvwin(_win, _pos.y, _pos.x);
	wresize(_win, _size.y, _size.x);
}

void View::onFocusChanged(bool hasFocus)
{
	int pair = COLOR_PAIR(COLOR_WINDOW);

	if (hasFocus)
		pair |= A_REVERSE;

	wbkgd(_win, pair);
}

Vec2 View::getSize() const
{
    return _size;
}

Vec2 View::getPosition() const
{
    return _pos;
}

void View::setSize(Vec2 size)
{
    _size = size;
    onResize();
}

void View::setPosition(Vec2 pos)
{
    _pos = pos;
    onResize();
}

bool View::initialize()
{
	if (!_parentWin) {
		fprintf(stderr, "View::initialize: no parent window in View\n");
		return false;
	}

	if (_win) {
		fprintf(stderr, "View::initialize: window already exists\n");
        return false;
	}

    _win = derwin(_parentWin, _size.y, _size.x, _pos.y, _pos.x);
	touchwin(_parentWin);

    return onInit();
}

bool View::onInit()
{
    /* Empty default implementation */
    return true;
}

WINDOW* View::getParentWindow()
{
    return _parentWin;
}

WINDOW* View::getWindow()
{
    return _win;
}


// MenuView
MenuView::MenuView(WINDOW *parent):
    View(parent),
    _selItem(0)
{

}

MenuView::~MenuView()
{
    destroyMenu();
}

bool MenuView::onKey(int key)
{
    if (_menu) {
        switch (key) {
            case 'j':
            case KEY_DOWN:
                _selItem = clamp<int>(_selItem + 1, 0, _items.size());
                break;

            case 'k':
            case KEY_UP:
                _selItem = clamp<int>(_selItem - 1, 0, _items.size());
                break;

            case KEY_ENTER:
                onItemSelected(_selItem);
                return true;

            default:
                return false;
        }

        set_current_item(_menu, _items[_selItem]);
        return true;
	}

    return false;
}

bool MenuView::recreateMenu()
{
    destroyMenu();

    _selItem = 0;
    int nItems = getItemCount();

    if (nItems > 0) {
        for (int i = 0; i < nItems; i++) {
            std::string title = getTitleForItem(i);
            const char *ctitle = title.c_str();

            ITEM *item = new_item(ctitle, ctitle);
            _items.push_back(item);
        }

        _menu = new_menu(_items.data());
        menu_opts_off(_menu, O_SHOWDESC);

        set_menu_format(_menu, getSize().y - 2, 0);
        set_menu_mark(_menu, " ");
        set_current_item(_menu, _items[_selItem]);

        set_menu_win(_menu, getWindow());
        post_menu(_menu);
    }

    return true;
}

bool MenuView::onInit()
{
    recreateMenu();
    return true;
}

int MenuView::getItemCount()
{
    return 0;
}

std::string MenuView::getTitleForItem(int index)
{
    std::stringstream ss;
    ss << "Item " << index;
    return ss.str();
}

void MenuView::onItemSelected(int)
{
    /* Empty default handler */
}

void MenuView::destroyMenu()
{
	if (_menu) {
		free_menu(_menu);
        _menu = NULL;
    }

    while (_items.size()) {
        free_item(_items[_items.size() - 1]);
        _items.pop_back();
    }
}


// PlaybarView
bool PlaybarView::onKey(int)
{
    return false;
}

