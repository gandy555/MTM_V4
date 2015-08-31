#ifndef __VIEW_MNG_H__
#define __VIEW_MNG_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define MAX_ICON_IMG_CNT		20
#define TXT_ALIGN_NONE			0

#define TXT_HALIGN_LEFT		0x1
#define TXT_HALIGN_CENTER		0x2
#define TXT_HALIGN_RIGHT		0x4
#define TXT_HALIGN_MASK		0x7

#define TXT_VALIGN_TOP			0x10
#define TXT_VALIGN_MIDDLE		0x20
#define TXT_VALIGN_BOTTOM		0x40
#define TXT_VALIGN_MASK		0x70

#define TXT_ALIGN_CENTER	(TXT_HALIGN_CENTER|TXT_VALIGN_MIDDLE)
#define TXT_ALIGN_RIGHT	(TXT_HALIGN_RIGHT|TXT_VALIGN_MIDDLE)
#define TXT_ALIGN_LEFT		(TXT_HALIGN_LEFT|TXT_VALIGN_MIDDLE)

typedef struct {
	int x;
	int y;
	int w;
	int h;
} rect_t;

typedef struct {
	rect_t rect;
	GR_GC_ID gc;
	GR_DRAW_ID img;
	GR_WINDOW_ID wid;
} obj_img_t;

typedef struct {
	rect_t rect;
	GR_GC_ID gc;
	GR_DRAW_ID imgs[MAX_ICON_IMG_CNT];
	GR_WINDOW_ID wid;
} obj_icon_t;

enum {
	VIEW_ID_WEATHER = 0,
	VIEW_ID_ELEVATOR,
	VIEW_ID_GAS,
	VIEW_ID_LIGHT,
	VIEW_ID_PARKING,
	VIEW_ID_SECURITY,
	VIEW_ID_SETUP,
	MAX_VIEW_ID
};

typedef void (*view_handler)(void);
typedef void (*key_handler)(u16 type, u16 code);

typedef struct {
	view_handler entry;
	view_handler draw;
	view_handler exit;
	key_handler key;
} view_handlers_t;

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void ui_register_view(u8 _id, view_handler _entry,
				view_handler _draw, view_handler _exit);
extern void ui_change_view(u8 _id);
extern void ui_draw_view(void);
extern BOOL init_ui_manager(void);
extern obj_img_t *ui_create_img_obj(int x, int y, int w, int h, char *_path);
extern obj_icon_t *ui_create_icon_obj(int x, int y, int w, int h);
extern int ui_load_icon_img(obj_icon_t *_h, int _idx, char *_path);
extern void ui_draw_image(obj_img_t *_obj_h);
extern void ui_draw_image_part(obj_img_t *_obj_h, rect_t *_dst, rect_t *_src);
extern void ui_draw_icon_image(obj_icon_t *_obj_h, int _idx);
extern void ui_draw_rect(int x, int y, int w, int h, u32 co, int fill);
extern void ui_draw_text(int x, int y, int w, int h, u32 size, u32 co, u32 align, char *str);
extern void ui_draw_text_window(GR_WINDOW_ID wid, int x, int y, int w, int h,
			u32 size, u32 co, u32 align, char *str);
extern void ui_operate_key(u32 _type, u32 _code);
extern void ui_switch_to(u8 _id);
#endif	// __VIEW_MNG_H__

