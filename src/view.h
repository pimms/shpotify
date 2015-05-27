#ifndef VIEW_H_
#define VIEW_H_

#include <ncurses.h>
#include <stdbool.h>
#include <menu.h>

enum view_type {
	VIEW_TYPE_MAINMENU,
	VIEW_TYPE_PLIST,
	VIEW_TYPE_PLAYBAR,
};

struct view_mainmenu {
	MENU *menu;
	ITEM **items;
	int nitems;
	int sel;
};

struct view_playbar {

};

struct view
{
	WINDOW *parent_win;
	WINDOW *sub_win;
	int width;
	int height;
	int x;
	int y;
	enum view_type type;

	/* Redraw handler */
	bool (*f_redraw)(struct view*);
	/* Key handler */
	bool (*f_key)(struct view *view, int key);
	/* Resize handler */
	bool (*f_resize)(struct view *view);
	/* Focus handler */
	void (*f_focus)(struct view *view, bool has_focus);

	/* Specialized type-fields */
	union {
		struct view_mainmenu menu;
		struct view_playbar bar;
	} spec;
};

/**
 * Set the "constant" attributes of the view type. Event handlers
 * and other identificators which will never change are assigned.
 * This function does not create the window. Call v->f_resize(v)
 * on the view after initializing to create it.
 */
bool view_init(struct view *view, enum view_type type);
bool view_destroy(struct view *view);

/**
 * "Sugar" functions. Calls the related handler in the view.
 */
bool view_redraw(struct view *view);
bool view_resize(struct view *view);
bool view_key(struct view *view, int key);
void view_focus(struct view *view, bool has_focus);


#endif
