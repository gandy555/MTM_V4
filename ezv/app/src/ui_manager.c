/******************************************************************************
 * Filename:
 *   ui_manager.c
 *
 * Description:
 *   ui manager (implemented for API of GUI)
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-08-06
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "main.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
GR_WINDOW_ID g_wid = 0;
GR_GC_ID g_gc = 0;
GR_FONT_ID g_font = 0;
GR_SCREEN_INFO g_scr_info = {0,};
static u8 g_curr_view = VIEW_ID_WEATHER;
static view_handlers_t g_view_handlers[MAX_VIEW_ID] = {0,};

//------------------------------------------------------------------------------
// Function Name  : error_handler()
// Description    : 
//------------------------------------------------------------------------------
static void error_handler(GR_EVENT *ep)
{
	printf("ERROR: code=%d, name=%s\r\n", ep->error.code, ep->error.name);
}

//------------------------------------------------------------------------------
// Function Name  : init_ui_manager()
// Description    : 
//------------------------------------------------------------------------------
BOOL init_ui_manager(void)
{
	CObject* pObject;
	UINT id;

	PRINT_FUNC_CO();
	
	// start nano-x service
	if (GrOpen() < 0) {
	//	DBGMSG(DBG_MAIN, "[Failure]\r\n--> %s: GrOpen Failed!!\r\n", __func__);
		return FALSE;
	}

	GrSetErrorHandler(error_handler);
	GrGetScreenInfo(&g_scr_info);

	// prepare g_font
	g_font = GrCreateFontEx((GR_CHAR *)FONT_PATH, 18, 18, NULL);
	GrSetFontAttr(g_font, (GR_TFKERNING | GR_TFANTIALIAS), 0);

	g_wid = GrNewWindow(GR_ROOT_WINDOW_ID, 0, 0, g_scr_info.cols, g_scr_info.rows, 0, BLACK, 0);
	if (g_wid == 0) {
	//	DBGMSG(DBG_MAIN, "[Failure]\r\n--> %s: GrNewWindow failure\r\n", __func__);
		GrClose();
		return FALSE;
	}

	g_gc = GrNewGC();

	GrRaiseWindow(g_wid);
	GrMapWindow(g_wid);

	GrSelectEvents(g_wid, GR_EVENT_MASK_BUTTON_DOWN | GR_EVENT_MASK_EXPOSURE);

	GrSetGCUseBackground(g_gc, FALSE);
//	GrSetGCUseBackground(g_gc, TRUE);
	GrSetGCFont(g_gc, g_font);

//	GrSetGCBackground(g_gc, BLACK);
	GrSetGCForeground(g_gc, WHITE);

//	BuildObject();

	return TRUE;
}

//------------------------------------------------------------------------------
// Function Name  : ui_create_img_obj()
// Description    :
//------------------------------------------------------------------------------
obj_img_t *ui_create_img_obj(int x, int y, int w, int h, char *_path)
{
	obj_img_t *obj_h = NULL;

	if (_path == NULL) {
		return NULL;
	}
	
	obj_h = malloc(sizeof(obj_img_t));
	if (obj_h != NULL) {
		memset(obj_h, 0, sizeof(obj_img_t));
		obj_h->rect.x = x;
		obj_h->rect.y = y;
		obj_h->rect.w = w;
		obj_h->rect.h = h;
		obj_h->gc = g_gc;
		obj_h->wid = g_wid;
		obj_h->img = GrLoadImageFromFile(_path, 0);
	} else {
		DBG_MSG_CO(CO_RED, "<%s> obj create failed\r\n");
	}
	
	return obj_h;
}

//------------------------------------------------------------------------------
// Function Name  : ui_create_icon_obj()
// Description    :
//------------------------------------------------------------------------------
obj_icon_t *ui_create_icon_obj(int x, int y, int w, int h)
{
	obj_icon_t *obj_h = NULL;

	obj_h = malloc(sizeof(obj_img_t));
	if (obj_h != NULL) {
		memset(obj_h, 0, sizeof(obj_img_t));
		obj_h->rect.x = x;
		obj_h->rect.y = y;
		obj_h->rect.w = w;
		obj_h->rect.h = h;
		obj_h->gc = g_gc;
		obj_h->wid = g_wid;
	} else {
		DBG_MSG_CO(CO_RED, "<%s> obj create failed\r\n");
	}
	
	return obj_h;
}

//------------------------------------------------------------------------------
// Function Name  : ui_load_icon_img()
// Description    :
//------------------------------------------------------------------------------
int ui_load_icon_img(obj_icon_t *_h, int _idx, char *_path)
{
	if (_idx >= MAX_ICON_IMG_CNT) {
		DBG_MSG_CO(CO_RED, "<%s> Invalid index\r\n", __func__);
		return -1;
	}
	
	_h->imgs[_idx] = GrLoadImageFromFile(_path, 0);
	
	return 1;
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_rect()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_rect(int x, int y, int w, int h, u32 co, int fill)
{
	GrSetGCForeground(g_gc, co);

	if (fill)
		GrFillRect(g_wid, g_gc, x, y, w, h);
	else
		GrRect(g_wid, g_gc, x, y, w, h);
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_text()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_text(int x, int y, int w, int h, u32 size, u32 co, u32 align, char *str)
{
	int align_x, align_y, len, width, height, base;
	GR_GC_INFO gc_info;
	
	GrGetGCInfo(g_gc, &gc_info);
	GrSetGCUseBackground(g_gc, FALSE);
	GrSetGCForeground(g_gc, co);
	GrSetFontSizeEx(g_font, size, size);

	len = strlen(str);

	align_x = x;
	align_y = y;
	if (align != TXT_ALIGN_NONE) {
		GrGetGCTextSize(g_gc, str, len, MWTF_DBCS_EUCKR,
			&width, &height, &base);

		// horizontal align
		if (align & TXT_HALIGN_CENTER) {
			if (width < w) 
				align_x = x + ((w - width) / 2);
		} else if (align & TXT_HALIGN_RIGHT) {
			if (width < w) 
				align_x = x + (w - width);
		}

		// vertical align
		if (align & TXT_VALIGN_MIDDLE) {
			if (height < h) 
				align_y = y + ((h - height) / 2);
		} else if (align & TXT_VALIGN_BOTTOM) {
			if (height < h) 
				align_y = y + (h - height);
		}
	}

	GrText(g_wid, g_gc, align_x, align_y, str, len, MWTF_DBCS_EUCKR|GR_TFTOP);

	//Recover GC Info
	GrSetGCUseBackground(g_gc, gc_info.usebackground);
	GrSetGCForeground(g_gc, gc_info.foreground);
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_text_window()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_text_window(GR_WINDOW_ID wid, int x, int y, int w, int h,
			u32 size, u32 co, u32 align, char *str)
{
	int align_x, align_y, len, width, height, base;
	GR_GC_INFO gc_info;
	
	GrGetGCInfo(g_gc, &gc_info);
	GrSetGCUseBackground(g_gc, FALSE);
	GrSetGCForeground(g_gc, co);
	GrSetFontSizeEx(g_font, size, size);

	len = strlen(str);

	align_x = x;
	align_y = y;
	if (align != TXT_ALIGN_NONE) {
		GrGetGCTextSize(g_gc, str, len, MWTF_DBCS_EUCKR,
			&width, &height, &base);

		// horizontal align
		if (align & TXT_HALIGN_CENTER) {
			if (width < w) 
				align_x = x + ((w - width) / 2);
		} else if (align & TXT_HALIGN_RIGHT) {
			if (width < w) 
				align_x = x + (w - width);
		}

		// vertical align
		if (align & TXT_VALIGN_MIDDLE) {
			if (height < h) 
				align_y = y + ((h - height) / 2);
		} else if (align & TXT_VALIGN_BOTTOM) {
			if (height < h) 
				align_y = y + (h - height);
		}
	}

	GrText(wid, g_gc, align_x, align_y, str, len, MWTF_DBCS_EUCKR|GR_TFTOP);

	//Recover GC Info
	GrSetGCUseBackground(g_gc, gc_info.usebackground);
	GrSetGCForeground(g_gc, gc_info.foreground);
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_image()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_image(obj_img_t *_obj_h)
{
	if (_obj_h == NULL) 
		return;
	
	GrDrawImageToFit(_obj_h->wid, _obj_h->gc,
		_obj_h->rect.x, _obj_h->rect.y, _obj_h->rect.w, _obj_h->rect.h,
		_obj_h->img);
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_image_part()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_image_part(obj_img_t *_obj_h, rect_t *_dst, rect_t *_src)
{
	if (_obj_h == NULL) 
		return;
	
	GrDrawImagePartToFit(_obj_h->wid, _obj_h->gc,
		_dst->x, _dst->y, _dst->w, _dst->h,
		_src->x, _src->y, _src->w, _src->h,
		_obj_h->img);
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_icon_image()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_icon_image(obj_icon_t *_obj_h, int _idx)
{
	if (_obj_h == NULL) 
		return;
	
	GrDrawImageToFit(_obj_h->wid, _obj_h->gc,
		_obj_h->rect.x, _obj_h->rect.y, _obj_h->rect.w, _obj_h->rect.h,
		_obj_h->imgs[_idx]);
}

//------------------------------------------------------------------------------
// Function Name  : ui_view_draw()
// Description    : 
//------------------------------------------------------------------------------
void ui_change_view(u8 _id)
{
	u8 prev_id = g_curr_view;

	DBG_MSG_CO(CO_BLUE, "<%s> %d -> %d", g_curr_view, _id);
	
	g_view_handlers[prev_id].exit();
	g_view_handlers[_id].entry();
	g_curr_view = _id;
}

//------------------------------------------------------------------------------
// Function Name  : ui_view_draw()
// Description    : 
//------------------------------------------------------------------------------
void ui_register_view(u8 _id, view_handler _entry, view_handler _draw, view_handler _exit)
{
	g_view_handlers[_id].entry = _entry;
	g_view_handlers[_id].draw = _draw;
	g_view_handlers[_id].exit = _exit;
}

//------------------------------------------------------------------------------
// Function Name  : ui_draw_view()
// Description    : 
//------------------------------------------------------------------------------
void ui_draw_view(void)
{
	g_view_handlers[g_curr_view].draw();
}

