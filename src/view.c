#include "view.h"
#include "shpotify.h"

#include <memory.h>
#include <stdlib.h>


static bool view_create_subwin(struct view *view);
static bool view_default_redraw(struct view *view);
static bool view_default_resize(struct view *view);

static bool mainmenu_init(struct view *view);
static void mainmenu_destroy(struct view *view);
static bool mainmenu_key(struct view *view, int key);
static void mainmenu_focus(struct view *view, bool has_focus);

static bool playbar_init(struct view *view);
static void playbar_destroy(struct view *view);
static bool playbar_key(struct view *view, int key);
static void playbar_focus(struct view *view, bool has_focus);


bool view_init(struct view* view, enum view_type type)
{
	if (!view->parent_win)
		return false;

	switch (type) {
		case VIEW_TYPE_MAINMENU:
			view->f_redraw = view_default_redraw;
			view->f_key = mainmenu_key;
			view->f_resize = view_default_resize;
			view->f_focus = mainmenu_focus;
			view->type = type;
			view->spec.menu.menu = NULL;
			view->spec.menu.items = NULL;
			if (!mainmenu_init(view))
				return false;
			break;

		case VIEW_TYPE_PLAYBAR:
			view->f_redraw = view_default_redraw;
			view->f_key = playbar_key;
			view->f_resize = view_default_resize;
			view->f_focus = playbar_focus;
			view->type = type;
			if (!playbar_init(view))
				return false;
			break;

		default:
			return false;
	}

	view_focus(view, false);

	return view->f_resize(view) && view->f_redraw(view);
}

bool view_destroy(struct view *view)
{
	if (view->sub_win)
		delwin(view->sub_win);
	view->sub_win = NULL;

	bool status = false;

	switch (view->type) {
		case VIEW_TYPE_MAINMENU:
			mainmenu_destroy(view);
			break;

		case VIEW_TYPE_PLAYBAR:
			playbar_destroy(view);
			break;

		default:
			break;
	}

	return status;
}


bool
view_redraw(struct view *view)
{
	return view->f_redraw(view);
}

bool
view_resize(struct view *view)
{
	return view->f_resize(view);
}

bool
view_key(struct view *view, int key)
{
	return view->f_key(view, key);
}

void
view_focus(struct view *view, bool has_focus)
{
	view->f_focus(view, has_focus);
}



static bool
view_create_subwin(struct view *view)
{
	if (!view->parent_win) {
		fprintf(stderr, "view_create_subwin: no parent window in view\n");
		return false;
	}

	if (view->sub_win) {
		delwin(view->sub_win);
		view->sub_win = NULL;
	}

    view->sub_win = derwin(view->parent_win, view->height, view->width, view->y, view->x);
	touchwin(view->parent_win);

	return true;
}

static bool
view_default_resize(struct view *view)
{
	mvwin(view->sub_win, view->y, view->x);
	wresize(view->sub_win, view->height, view->width);
	return true;
}

static bool
view_default_redraw(struct view *view)
{
	box(view->sub_win, ACS_VLINE, ACS_HLINE);
	wnoutrefresh(view->sub_win);
	return true;
}


static bool
mainmenu_init(struct view *view)
{
    const char *menu_items[] = {
		"Search Album",
		"Search Artist",
		"Search Playlist",
		"What's new",
		"Starred",
		"Playlists",
		"Logout",
		"Exit",
    };

	int i;
	view->spec.menu.nitems = ARRAY_SIZE(menu_items);

	view->spec.menu.items = (ITEM**)calloc(view->spec.menu.nitems + 1, sizeof(ITEM*));

	for (i = 0; i < view->spec.menu.nitems; i++)
		view->spec.menu.items[i] = new_item(menu_items[i], menu_items[i]);
	view->spec.menu.items[view->spec.menu.nitems] = NULL;

	view->spec.menu.menu = new_menu(view->spec.menu.items);
    menu_opts_off(view->spec.menu.menu, O_SHOWDESC);
    set_menu_format(view->spec.menu.menu, view->height-2, 0);
    set_menu_mark(view->spec.menu.menu, " ");
    set_current_item(view->spec.menu.menu, view->spec.menu.items[0]);

	if (!view_create_subwin(view))
		return false;

    set_menu_win(view->spec.menu.menu, view->sub_win);

	view->spec.menu.sel = 0;
	post_menu(view->spec.menu.menu);

	return true;
}

static void
mainmenu_destroy(struct view *view)
{
	if (view->spec.menu.menu)
		free_menu(view->spec.menu.menu);
	view->spec.menu.menu = NULL;

	if (view->spec.menu.items) {
		int i = 0;
		while (view->spec.menu.items[i])
			free_item(view->spec.menu.items[i++]);
		free(view->spec.menu.items);
		view->spec.menu.items = NULL;
	}
}

static bool
mainmenu_key(struct view *view, int key)
{
	switch (key) {
		case 'j':
		case KEY_DOWN:
			view->spec.menu.sel++;
			if (view->spec.menu.sel > view->spec.menu.nitems)
				view->spec.menu.sel = view->spec.menu.nitems;
			break;

		case 'k':
		case KEY_UP:
			view->spec.menu.sel--;
			if (view->spec.menu.sel < 0)
				view->spec.menu.sel = 0;
			break;

		default:
			return false;
	}

	ITEM **items = view->spec.menu.items;
	MENU *menu = view->spec.menu.menu;
	set_current_item(menu, items[view->spec.menu.sel]);

	return true;
}

static void
mainmenu_focus(struct view *view, bool has_focus)
{
	int pair = COLOR_PAIR(COLOR_WINDOW);

	if (has_focus)
		pair |= A_REVERSE;

	wbkgd(view->sub_win, pair);
	set_menu_back(view->spec.menu.menu, pair);
	set_menu_fore(view->spec.menu.menu, pair ^ A_REVERSE);
}


static bool
playbar_init(struct view *view)
{
	if (!view_create_subwin(view))
		return false;

	return true;
}

static void
playbar_destroy(struct view *view)
{

}

static bool
playbar_key(struct view *view, int key)
{
	return false;
}

static void
playbar_focus(struct view *view, bool has_focus)
{
	int pair = COLOR_PAIR(COLOR_WINDOW);

	if (has_focus)
		pair |= A_REVERSE;

	wbkgd(view->sub_win, pair);
}
