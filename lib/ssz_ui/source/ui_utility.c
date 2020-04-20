#include "ui_utility.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
//get the intersect(min),return if intersect
bool ui_rect_intersect(Rect* dest, const Rect* src)
{
	int x0 = dest->x0;
	int y0 = dest->y0;
	if (dest->x0 < src->x0) {
		dest->x0 = src->x0;
	}
	if (dest->y0 < src->y0) {
		dest->y0 = src->y0;
	}
	if (dest->x1 > src->x1) {
		dest->x1 = src->x1;
	}
	if (dest->y1 > src->y1) {
		dest->y1 = src->y1;
	}
	if (dest->x1 < dest->x0 || dest->y1 < dest->y0) {
		dest->x0=x0;
		dest->x1 = x0 - 1;
		dest->y0=y0;
		dest->y1 = y0 - 1;
		return false;
	}

	return true;
}
//get the intersect(min) of two rect, return if intersect
bool ui_rect_intersect_to(Rect* dest, const Rect* src1, const Rect* src2)
{
	dest->x0 = ssz_max(src1->x0, src2->x0);
	dest->y0 = ssz_max(src1->y0, src2->y0);
	dest->x1 = ssz_min(src1->x1, src2->x1);
	dest->y1 = ssz_min(src1->y1, src2->y1);
	if (dest->x1<dest->x0 || dest->y1 < dest->y0) {
		dest->x1 = dest->x0-1;
		dest->y1 = dest->y1-1;
		return false;
	}

	return true;
}
//get the merged rect which can contain two rect
void ui_rect_merge(Rect* dest, const Rect* src)
{
	if (dest->x0 > src->x0) {
		dest->x0 = src->x0;
	}
	if (dest->y0 > src->y0) {
		dest->y0 = src->y0;
	}
	if (dest->x1 < src->x1) {
		dest->x1 = src->x1;
	}
	if (dest->y1 < src->y1) {
		dest->y1 = src->y1;
	}
}
//get the merged rect which can contain two rect
void ui_rect_merge_to(Rect* dest, const Rect* src1, const Rect* src2)
{
	dest->x0 = ssz_min(src1->x0, src2->x0);
	dest->y0 = ssz_min(src1->y0, src2->y0);
	dest->x1 = ssz_max(src1->x1, src2->x1);
	dest->y1 = ssz_max(src1->y1, src2->y1);
}
void ui_rect_move(Rect* dest, int x_size, int y_size)
{
	dest->x0 += x_size;
	dest->y0 += y_size;
	dest->x1 += x_size;
	dest->y1 += y_size;
}

void ui_rect_move_to(Rect * dest, int x, int y)
{
	int x_size = x-dest->x0;
	int y_size = y - dest->y0;
	dest->x0 = x;
	dest->y0 = y;
	dest->x1 += x_size;
	dest->y1 += y_size;
}

bool ui_rect_is_intersect(const Rect * src1, const Rect * src2)
{
	int x0,x1,y0, y1;
	x0 = ssz_max(src1->x0, src2->x0);
	y0 = ssz_max(src1->y0, src2->y0);
	x1 = ssz_min(src1->x1, src2->x1);
	y1 = ssz_min(src1->y1, src2->y1);


	if (x1 < x0 || y1<y0) {
		return false;
	}
	else {
		return true;
	}
}

bool ui_rect_is_intersect_ex(const Rect * src1, int x, int y, int x_size, int y_size)
{
	int x0, x1, y0, y1;
	x0 = ssz_max(src1->x0, x);
	y0 = ssz_max(src1->y0, y);
	x1 = ssz_min(src1->x1, x + x_size - 1);
	y1 = ssz_min(src1->y1, y + y_size - 1);

	if (x1 < x0 || y1 < y0) {
		return false;
	}
	else {
		return true;
	}
}

bool ui_rect_is_include(const Rect * dest, const Rect * src)
{
	if (dest->x0 > src->x0 || dest->y0 > src->y0 || 
		dest->x1 < src->x1 || dest->y1 < src->y1) {
		return false;
	}
	return true;
}
